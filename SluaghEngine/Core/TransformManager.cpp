#include "TransformManager.h"
#include <algorithm>
#include <Profiler.h>
#include "Engine.h"

#undef min
#undef max
using namespace DirectX;

SE::Core::TransformManager::TransformManager(const ITransformManager::InitializationInfo& initInfo) : initInfo(initInfo)
{

	this->initInfo = initInfo;

	_ASSERT(initInfo.entityManager);
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
	if (!initInfo.entityManager->Alive(e))
		ProfileReturnVoid;
	const uint32_t lookUpTableIndex = e.Index();
	if(lookUpTableIndex >= lookUpTableSize)
	{
		const size_t newSize = lookUpTableIndex + std::min(lookUpTableSize, size_t(512));
		int32_t* newTable = new int32_t[newSize];
		memset(newTable, -1, sizeof(int32_t) * newSize);
		memcpy(newTable, lookUpTable, sizeof(int32_t) * lookUpTableSize);
		lookUpTableSize = newSize;
		delete[] lookUpTable;
		lookUpTable = newTable;
	}
	if (lookUpTable[lookUpTableIndex] != -1)
	{
		//The Garbage collection might not have catched up with the creation
		if(e.Gen() != data.entities[lookUpTable[lookUpTableIndex]].Gen())
		{
			Destroy(lookUpTable[lookUpTableIndex]);
		}
		else
		{
			//The exact same entity has been created twice. (This is OK, just mark as dirty and return)
			SetAsDirty(e);
			ProfileReturnVoid;
		}
	}

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
	dirtyTransforms.resize(data.used);
	ProfileReturnVoid;
}

void SE::Core::TransformManager::BindChild(const Entity & parent, const Entity & child, bool rotation, bool translateToParent)
{
	StartProfile;
	_ASSERT(parent.Index() < lookUpTableSize);
	_ASSERT(child.Index() < lookUpTableSize);

	const int32_t parentIndex = lookUpTable[parent.Index()];
	const int32_t childIndex = lookUpTable[child.Index()];

	_ASSERT(parentIndex >= 0);
	_ASSERT(childIndex >= 0);

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
	{
		data.flags[childIndex] |= TransformFlags::INHERIT_ROTATION;
	}
	if(translateToParent)
	{
		Move(child, data.positions[parentIndex]);
	}

	ProfileReturnVoid;

}

void SE::Core::TransformManager::SetAsDirty(const Entity & e)
{
	_ASSERT(e.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[e.Index()];
	_ASSERT(index != -1);
	data.flags[index] |= TransformFlags::DIRTY;
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

	float a, b, c;
	a = data.rotations[index].x+ pitch;
	b = data.rotations[index].y+ yaw;
	c = data.rotations[index].z+ roll;

	data.rotations[index].x = a;
	data.rotations[index].y = b;
	data.rotations[index].z = c;
	data.flags[index] |= TransformFlags::DIRTY;

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

			Rotate(data.entities[child], pitch, yaw, roll);
			Move(data.entities[child], translationVector);
			
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
	data.flags[index] |= TransformFlags::DIRTY;

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
	const float rollDiff = roll - data.rotations[index].z;
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
	return data.scalings[index];
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

void SE::Core::TransformManager::SetTransform(const Entity& entity, const DirectX::XMFLOAT4X4 & matrix)
{
	_ASSERT(entity.Index() < lookUpTableSize);
	const int32_t index = lookUpTable[entity.Index()];
	_ASSERT(index >= 0);

	dirtyTransforms[index] = matrix;
	SetDirty(entity, index);
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

DirectX::XMFLOAT3 SE::Core::TransformManager::GetRight(const Entity& e) const
{
	XMVECTOR defRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT3 rot = GetRotation(e);
	XMMATRIX rotm = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rot));
	XMVECTOR right = XMVector3TransformNormal(defRight, rotm);
	XMFLOAT3 r;
	XMStoreFloat3(&r, right);
	return r;
}

const void SE::Core::TransformManager::SetForward(const Entity & e, const DirectX::XMFLOAT3 & forward)
{
	XMVECTOR ndir = XMLoadFloat3(&forward);
	SetForward(e, ndir);
}

const void SE::Core::TransformManager::SetForward(const Entity & e, const DirectX::XMVECTOR & forward)
{
	XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR newForward = XMVector3Normalize(forward);

	XMVECTOR projToXZ = XMVector3Normalize(newForward - (defaultUp * XMVector3Dot(defaultUp, newForward)));
	float angleY = acosf(XMVectorGetX(XMVector3Dot(projToXZ, defaultForward)));
	if (XMVectorGetX(projToXZ) < 0)
		angleY = -angleY;

	XMVECTOR projToZY = XMVector3Normalize(newForward - (defaultForward * XMVector3Dot(defaultForward, newForward)));
	float angleX = acosf(XMVectorGetX(XMVector3Dot(projToZY, defaultForward)));
	float angleZ = acosf(XMVectorGetX(XMVector3Dot(projToZY, defaultUp)));
	if (XMVectorGetY(projToZY) > 0)
		angleX = -angleX;
	if (XMVectorGetZ(projToZY) < 0)
		angleZ = -angleZ;

	SetRotation(e, angleX, angleY, angleZ);
}

void SE::Core::TransformManager::GarbageCollection()
{
	StartProfile;
	uint32_t aliveInRow = 0;
	const uint32_t quitWhenReached = std::max((uint32_t)(data.used * 0.02f), 40U);
	while(data.used > 0 && aliveInRow < quitWhenReached)
	{
		const std::uniform_int_distribution<size_t> distribution(0U, data.used - 1U);
		size_t i = distribution(generator);
		if(initInfo.entityManager->Alive(data.entities[i]))
		{
			++aliveInRow;
			continue;
		}
		aliveInRow = 0;
		Destroy(i);
	}
	ProfileReturnVoid;
}

uint32_t SE::Core::TransformManager::ActiveTransforms() const
{
	return data.used;
}



void SE::Core::TransformManager::Frame(Utilz::TimeCluster* timer)
{
	_ASSERT(timer);
	StartProfile;
	GarbageCollection();
	timer->Start(("TransformManager"));
	auto LoopDirty = [this](int start, int end)
	{
		for (int i = start; i < end; ++i)
		{
			if (data.flags[i] & TransformFlags::DIRTY)
			{
				XMFLOAT4X4 transform;
				const auto& translation = XMMatrixTranslationFromVector(XMLoadFloat3(&data.positions[i]));
				const auto& rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&data.rotations[i]));
				const auto& scale = XMMatrixScalingFromVector(XMLoadFloat3(&data.scalings[i]));
				XMStoreFloat4x4(&transform, scale*rotation*translation);
				dirtyTransforms[i] = transform;
			}
		}
		return true;
	};
	
	/*auto job1 = initInfo.threadPool->Enqueue(LoopDirty, 0, data.used / 2);
	auto job2 = initInfo.threadPool->Enqueue(LoopDirty, data.used / 4, data.used / 2);
	auto job3 = initInfo.threadPool->Enqueue(LoopDirty, data.used / 2, (data.used / 4) * 3);*/
	LoopDirty(0, data.used);//(data.used / 4) * 3, data.used);
	/*job1.get();
	job2.get();
	job3.get();*/
	

	for (int i = 0; i < data.used; ++i)
	{
		if (data.flags[i] & TransformFlags::DIRTY)
		{
			SetDirty(data.entities[i], i);
			data.flags[i] &= ~TransformFlags::DIRTY;
		}
	}

	
	timer->Stop(("TransformManager"));
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
	_ASSERT(index == lookUpTable[data.entities[index].Index()]);
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
	
	if (index != last)
	{
		lookUpTable[data.entities[index].Index()] = -1;
		data.entities[index] = data.entities[last];
		data.positions[index] = data.positions[last];
		data.rotations[index] = data.rotations[last];
		data.scalings[index] = data.scalings[last];
		data.childIndex[index] = data.childIndex[last];
		data.siblingIndex[index] = data.siblingIndex[last];
		data.parentIndex[index] = data.parentIndex[last];
		data.flags[index] = data.flags[last];
		lookUpTable[data.entities[last].Index()] = index;
	}
	else
	{
		lookUpTable[data.entities[index].Index()] = -1;
	}
	--data.used;
	
	

	//The entity we replaced the destroyed entity with might have had parent/siblings/children.
	//Fix those relations.
	if(data.parentIndex[index] >= 0 && index != last)
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
	if(data.childIndex[index] >= 0 && index != last)
	{
		int32_t child = data.childIndex[index];
		while (child != -1)
		{
			data.parentIndex[child] = index;
			child = data.siblingIndex[child];
		}
	}


}

void SE::Core::TransformManager::Destroy(const Entity & e)
{
	if (e.Index() < lookUpTableSize)
	{
		const int32_t index = lookUpTable[e.Index()];
		if(index >= 0)
		{
			Destroy(index);
		}
	}
	
}


