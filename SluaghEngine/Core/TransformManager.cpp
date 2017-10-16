#include <TransformManager.h>
#include <algorithm>
#include <Profiler.h>

#undef min
#undef max
using namespace DirectX;

SE::Core::TransformManager::TransformManager(EntityManager* em)
{
	entityManager = em;
	Allocate(512);
	lookUpTableSize = 512;
	lookUpTable = new int32_t[lookUpTableSize];
	memset(lookUpTable, -1, sizeof(int32_t) * lookUpTableSize);
	
	
}

SE::Core::TransformManager::~TransformManager()
{
	operator delete(data.data);
	delete[] lookUpTable;
}

void SE::Core::TransformManager::Create(const Entity& e, const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale)
{
	StartProfile;
	const uint32_t lookUpTableIndex = e.Index();
	if(lookUpTableIndex >= lookUpTableSize)
	{
		int32_t* newTable = new int32_t[lookUpTableSize + std::min(lookUpTableSize, size_t(512))];
		memcpy(newTable, lookUpTable, sizeof(int32_t) * lookUpTableSize);
		memset(newTable + lookUpTableSize, -1, std::min(lookUpTableSize, size_t(512)));
		delete[] lookUpTable;
		lookUpTable = newTable;
	}
	if (lookUpTable[e.Index()] != -1)
		ProfileReturnVoid;
	if (data.used == data.allocated)
		Allocate(data.allocated + std::min(data.allocated, size_t(256)));

	lookUpTable[lookUpTableIndex] = data.used;
	data.entities[data.used] = e;
	data.positions[data.used] = pos;
	data.rotations[data.used] = rotation;
	data.scalings[data.used] = scale;
	data.childIndex[data.used] = -1;
	data.siblingIndex[data.used] = -1;
	data.parentIndex[data.used] = -1;
	data.flags[data.used] = TransformFlags::DIRTY | TransformFlags::INHERIT_TRANSLATION;
	++data.used;

	ProfileReturnVoid;
}

void SE::Core::TransformManager::BindChild(const Entity & parent, const Entity & child, bool rotation)
{
	StartProfile;
	_ASSERT(parent.Index() < lookUpTableSize);
	_ASSERT(child.Index() < lookUpTableSize);

	const int32_t parentIndex = lookUpTable[parent.Index()];
	const int32_t childIndex = lookUpTable[child.Index()];

	data.parentIndex[childIndex] = parentIndex;
	//There might already be children under the parent.
	if(data.childIndex[parentIndex] >= 0)
	{
		//If so, put the child as the last sibling.
		int32_t sibling = data.childIndex[parentIndex];
		while(data.siblingIndex[sibling] != -1)
		{
			sibling = data.siblingIndex[sibling];
		}
		data.siblingIndex[sibling] = childIndex;
	}
	else
	{
		data.childIndex[parentIndex] = childIndex;
	}

	if (rotation)
		data.flags[childIndex] |= TransformFlags::INHERIT_ROTATION;

	ProfileReturnVoid;

}

void SE::Core::TransformManager::SetAsDirty(const Entity & e)
{
	queueLock.lock();

	queueLock.unlock();
}

void SE::Core::TransformManager::Move(const Entity& e, const DirectX::XMFLOAT3& dir)
{
	//Recursive function so no profiler
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];

	data.positions[index].x += dir.x;
	data.positions[index].y += dir.y;
	data.positions[index].z += dir.z;
	data.flags[index] |= TransformFlags::DIRTY;

	int32_t child = data.childIndex[index];
	while(child != -1)
	{
		if(data.flags[child] & TransformFlags::INHERIT_TRANSLATION)
			Move(data.entities[child], dir);
		child = data.siblingIndex[child];
	}
	
}

void SE::Core::TransformManager::Move(const Entity& e, const DirectX::XMVECTOR& dir)
{
	XMFLOAT3 fdir;
	XMStoreFloat3(&fdir, dir);
	Move(e, fdir);
}

void SE::Core::TransformManager::Rotate(const Entity& e, float pitch, float yaw, float roll)
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	data.rotations[index].x += pitch;
	data.rotations[index].y += yaw;
	data.rotations[index].z += roll;

	int32_t child = data.childIndex[index];
	while (child != -1)
	{
		if(data.flags[child] & TransformFlags::INHERIT_ROTATION)
		{
			XMVECTOR childPos = XMLoadFloat3(&data.positions[child]); XMVectorSetW(childPos, 1.0f);
			XMVECTOR parentPos = XMLoadFloat3(&data.positions[index]); XMVectorSetW(parentPos, 1.0f);
			XMVECTOR parentToChild = childPos - parentPos;
			XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
			XMVECTOR rotatedVector = XMVector3Transform(parentToChild, rot);
			XMVECTOR newPos = parentPos + rotatedVector;
			XMVECTOR translationVector = newPos - childPos;

			Move(data.entities[child], translationVector);
			Rotate(data.entities[child], pitch, yaw, roll);
		}
		child = data.siblingIndex[child];
	}
}

void SE::Core::TransformManager::Scale(const Entity& e, float scale)
{
	Scale(e, { scale, scale, scale });
}

void SE::Core::TransformManager::Scale(const Entity & e, const DirectX::XMFLOAT3 & scale)
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];

	data.scalings[index].x *= scale.x;
	data.scalings[index].y *= scale.y;
	data.scalings[index].z *= scale.z;

	int32_t child = data.childIndex[index];
	while(child != -1)
	{
		if(data.flags[child] & TransformFlags::INHERIT_SCALE)
			Scale(data.entities[child], scale);
		child = data.siblingIndex[child];
	}
}

void SE::Core::TransformManager::SetPosition(const Entity& e, const DirectX::XMFLOAT3& pos)
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	XMVECTOR position = XMLoadFloat3(&data.positions[index]);
	XMVECTOR newPos = XMLoadFloat3(&pos);
	XMVECTOR translation = newPos - position;
	Move(e, translation);

}

void SE::Core::TransformManager::SetRotation(const Entity& e, float pitch, float yaw, float roll)
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	const float pitchDiff = pitch - data.rotations[index].x;
	const float yawDiff = yaw - data.rotations[index].y;
	const float rollDiff = roll -data.rotations[index].z;
	Rotate(e, pitchDiff, yawDiff, rollDiff);
}

void SE::Core::TransformManager::SetScale(const Entity& e, float scale)
{
	SetScale(e, { scale, scale, scale });
}

void SE::Core::TransformManager::SetScale(const Entity & e, const DirectX::XMFLOAT3 & scale)
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	const float xDiff = scale.x / data.scalings[index].x;
	const float yDiff = scale.y / data.scalings[index].y;
	const float zDiff = scale.z / data.scalings[index].z;
	Scale(e, { xDiff, yDiff, zDiff });

}

const DirectX::XMFLOAT3& SE::Core::TransformManager::GetPosition(const Entity& e) const
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	return data.positions[index];
}

const DirectX::XMFLOAT3& SE::Core::TransformManager::GetRotation(const Entity& e) const
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	return data.rotations[index];
}

const DirectX::XMFLOAT3& SE::Core::TransformManager::GetScale(const Entity& e) const
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	return data.rotations[index];
}

const DirectX::XMFLOAT4X4 SE::Core::TransformManager::GetTransform(const Entity& e) const
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	const auto& pos = data.positions[index];
	const auto& rot = data.rotations[index];
	const auto& scale = data.scalings[index];
	XMFLOAT4X4 transform;
	XMStoreFloat4x4(&transform, XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z));
	return transform;

}

const DirectX::XMFLOAT3 SE::Core::TransformManager::GetForward(const Entity & e) const
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	const auto forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const auto rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&data.rotations[index]));
	XMFLOAT3 f;
	XMStoreFloat3(&f, XMVector3TransformNormal(forward, rotation));

	return f;
}

const void SE::Core::TransformManager::SetForward(const Entity & e, const DirectX::XMFLOAT3 & forward)
{
	XMVECTOR ndir = XMVector3Normalize(XMLoadFloat3(&forward));
	SetForward(e, ndir);
}

const void SE::Core::TransformManager::SetForward(const Entity & e, const DirectX::XMVECTOR & forward)
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];

	XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	XMVECTOR projToXZ = XMVector3Normalize(forward - (defaultUp * XMVector3Dot(defaultUp, forward)));
	float angleY = acosf(XMVectorGetX(XMVector3Dot(projToXZ, defaultForward)));
	if (XMVectorGetX(projToXZ) < 0)
		angleY = -angleY;

	XMVECTOR projToZY = XMVector3Normalize(forward - (defaultForward * XMVector3Dot(defaultForward, forward)));
	float angleX = acosf(XMVectorGetX(XMVector3Dot(projToZY, defaultForward)));
	float angleZ = acosf(XMVectorGetX(XMVector3Dot(projToZY, defaultUp)));
	if (XMVectorGetY(projToZY) < 0)
		angleX = -angleX;
	if (XMVectorGetZ(projToZY) < 0)
		angleZ = -angleZ;

	SetRotation(e, angleX, angleY, angleZ);
}

int SE::Core::TransformManager::GarbageCollection()
{
	StartProfile;
	uint32_t aliveInRow = 0;
	while(data.used > 0 && aliveInRow < 40U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, data.used - 1U);
		size_t i = distribution(generator);
		if(entityManager->Alive(data.entities[i]))
		{
			++aliveInRow;
			continue;
		}
		aliveInRow = 0;
		Destroy(i);
	}
	ProfileReturnConst(0);
}

uint32_t SE::Core::TransformManager::ActiveTransforms() const
{
	return data.used;
}

void SE::Core::TransformManager::Frame()
{
	StartProfile;
	//queueLock.lock();
	//for (auto& e : entityStack)
	//{
	//	auto& find = entityToIndex.find(e);
	//	if(find != entityToIndex.end())
	//		SetAsDirty(find->second);

	//}
	//entityStack.clear();
	//queueLock.unlock();
	//dirtyTransforms.clear();
	//parentDeferred.clear();
	//for (size_t i = 0; i < transformCount; i++)
	//{
	//	if (dirty[i])
	//	{
	//		UpdateTransform(i);
	//		
	//	}
	//}

	GarbageCollection();
	StopProfile;
}

inline void SE::Core::TransformManager::SetAsDirty(size_t index)
{
	_ASSERT(index < data.used);
	data.flags[index] |= TransformFlags::DIRTY;
	int32_t child = data.childIndex[index];
	while (child != -1)
	{
		SetAsDirty(data.entities[child]);
		child = data.siblingIndex[child];
	}

}

void SE::Core::TransformManager::UpdateTransform(size_t index)
{
	StartProfile;
	XMFLOAT4X4 transform;
	const auto& translation = XMMatrixTranslationFromVector(XMLoadFloat3(&data.positions[index]));
	const auto& rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&data.rotations[index]));
	const auto& scale = XMMatrixScalingFromVector(XMLoadFloat3(&data.scalings[index]));
	XMStoreFloat4x4(&transform, scale*rotation*translation);
	dirtyTransforms.push_back(transform);
	data.flags[index] = ~TransformFlags::DIRTY & data.flags[index];
	StopProfile;
}

void SE::Core::TransformManager::Allocate(size_t count)
{
	StartProfile;
	_ASSERT(count > data.allocated);

	TransformData newData;
	newData.allocated = count;
	newData.data = operator new(count * TransformData::size);
	newData.used = data.used;

	newData.entities = (Entity*)newData.data;
	newData.positions = (XMFLOAT3*)(newData.entities + count);
	newData.rotations = (XMFLOAT3*)(newData.positions + count);
	newData.scalings = (XMFLOAT3*)(newData.rotations + count);
	newData.childIndex = (int32_t*)(newData.scalings + count);
	newData.siblingIndex = (int32_t*)(newData.childIndex + count);
	newData.parentIndex = (int32_t*)(newData.siblingIndex + count);
	newData.flags = (int16_t*)(newData.parentIndex + count);

	memcpy(newData.entities, data.entities, data.used * sizeof(Entity));
	memcpy(newData.positions, data.positions, data.used * sizeof(XMFLOAT3));
	memcpy(newData.rotations, data.rotations, data.used * sizeof(XMFLOAT3));
	memcpy(newData.scalings, data.scalings, data.used * sizeof(XMFLOAT3));
	memcpy(newData.childIndex, data.childIndex, data.used * sizeof(int32_t));
	memcpy(newData.siblingIndex, data.siblingIndex, data.used * sizeof(int32_t));
	memcpy(newData.parentIndex, data.parentIndex, data.used * sizeof(int32_t));
	memcpy(newData.flags, data.flags, data.used * sizeof(int16_t));

	operator delete(data.data);
	data = newData;
	ProfileReturnVoid;
}

void SE::Core::TransformManager::Destroy(const size_t index)
{
	_ASSERT(index < data.used);
	lookUpTable[index] = -1;
	const int32_t last = data.used - 1;

	if(data.parentIndex[index] >= 0)
	{
		//If destroyed entity is the first child
		//Parent's first child becomes first sibling if it exists
		//Otherwise parents child becomes -1 if there are no siblings
		if(data.childIndex[data.parentIndex[index]] == index)
		{
			data.childIndex[data.parentIndex[index]] = data.siblingIndex[index]; //Will be -1 if no siblings
		}
		else
		{
			//If this entity is not the first child, we must find the sibling that points to this entity
			int32_t sibling = data.childIndex[data.parentIndex[index]];
			while (data.siblingIndex[sibling] != index)
				sibling = data.siblingIndex[sibling];
			data.siblingIndex[sibling] = data.siblingIndex[data.siblingIndex[sibling]];
		}
	}
	//If there is no parent, there are no siblings.
	//If we have children, those children lose their parent :(
	if(data.childIndex[index] >= 0)
	{
		int32_t child = data.childIndex[index];
		int32_t nextChild = data.siblingIndex[child];
		while(child != -1)
		{
			data.parentIndex[child] = -1;
			child = nextChild;
			nextChild = data.siblingIndex[child];
		}
	}
	

	data.entities[index] = data.entities[last];
	data.positions[index] = data.positions[last];
	data.rotations[index] = data.rotations[last];
	data.scalings[index] = data.scalings[last];
	data.childIndex[index] = data.childIndex[last];
	data.siblingIndex[index] = data.siblingIndex[last];
	data.parentIndex[index] = data.parentIndex[last];
	data.flags[index] = data.flags[last];
	--data.used;

	lookUpTable[last] = index;

	//The entity we replaced the destroyed entity with might have had parent/siblings/children.
	//Fix those relations.
	if(data.parentIndex[index] >= 0)
	{
		if (data.childIndex[data.parentIndex[index]] == last)
			data.childIndex[data.parentIndex[index]] = index;
		else
		{
			int32_t sibling = data.childIndex[data.parentIndex[index]];
			while (data.siblingIndex[sibling] != last)
				sibling = data.siblingIndex[sibling];
			data.siblingIndex[sibling] = index;
		}
	}
	//Fix children
	if(data.childIndex[index] >= 0)
	{
		int32_t child = data.childIndex[index];
		while (child != -1)
		{
			data.parentIndex[child] = index;
			child = data.siblingIndex[child];
		}
	}


}


