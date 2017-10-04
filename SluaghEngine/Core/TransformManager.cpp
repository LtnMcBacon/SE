#include <TransformManager.h>
#include <algorithm>
#include <Profiler.h>

#undef min
#undef max
using namespace DirectX;

SE::Core::TransformManager::TransformManager(EntityManager* em)
{
	entityManager = em;

	transformCapacity = 512;
	transformCount = 0;
	garbageCollectionIndex = 0;
	//const size_t bytes = transformCapacity * sizePerEntity;
	positions = new XMFLOAT3[transformCapacity];
	rotations = new XMFLOAT3[transformCapacity];
	scalings = new XMFLOAT3[transformCapacity];
	dirty = new uint8_t[transformCapacity];
	entities = new Entity[transformCapacity];
	Parent = new size_t[transformCapacity];
	DirtyTransform = new size_t[transformCapacity];
	Child = new size_t[transformCapacity];
	inheritRotation = new uint8_t[transformCapacity];
}

SE::Core::TransformManager::~TransformManager()
{
	delete[] positions;
	delete[] rotations;
	delete[] scalings;
	delete[] dirty;
	delete[] entities;
	delete[] Parent;
	delete[] Child;
	delete[] DirtyTransform;
	delete[] inheritRotation;
}

void SE::Core::TransformManager::Create(const Entity& e, const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale)
{
	StartProfile;
	auto& find = entityToIndex.find(e);
	if (find != entityToIndex.end())
	{
		dirty[find->second] = 1;
		ProfileReturnVoid;
	}

	if (transformCount == transformCapacity)
		ExpandTransforms();

	const uint32_t index = transformCount++;
	entityToIndex[e] = index;
	positions[index] = pos;
	rotations[index] = rotation;
	scalings[index] = scale;
	entities[index] = e;
	DirtyTransform[index] = ~0u;
	Child[index] = ~0u;
	Parent[index] = ~0u;
	inheritRotation[index] = 1u;
	dirty[index] = 1u;

	StopProfile;
}

void SE::Core::TransformManager::BindChild(const Entity & parent, const Entity & child, bool rotation)
{
	StartProfile;
	auto& findParent = entityToIndex.find(parent);
	if (findParent == entityToIndex.end())
		ProfileReturnVoid;

	auto& findChild = entityToIndex.find(child);
	if (findChild == entityToIndex.end())
		ProfileReturnVoid;

	// Setup Parent data
	Child[findParent->second] = findChild->second;

	// Setup child data
	Parent[findChild->second] = findParent->second;
	inheritRotation[findChild->second] = rotation ? 1u : 0u;
	StopProfile;

}

void SE::Core::TransformManager::SetAsDirty(const Entity & e)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	SetAsDirty(index);
}

void SE::Core::TransformManager::Move(const Entity& e, const DirectX::XMFLOAT3& dir)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	positions[index].x += dir.x;
	positions[index].y += dir.y;
	positions[index].z += dir.z;
	SetAsDirty(index);
}

void SE::Core::TransformManager::Move(const Entity& e, const DirectX::XMVECTOR& dir)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];

	XMVECTOR position = XMLoadFloat3(&positions[index]);

	XMStoreFloat3(&positions[index], position + dir);
	SetAsDirty(index);
}

void SE::Core::TransformManager::Rotate(const Entity& e, float pitch, float yaw, float roll)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	rotations[index].x += pitch;
	rotations[index].y += yaw;
	rotations[index].z += roll;
	SetAsDirty(index);
}

void SE::Core::TransformManager::Scale(const Entity& e, float scale)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	scalings[index].x *= scale;
	scalings[index].y *= scale;
	scalings[index].z *= scale;
	SetAsDirty(index);
}

void SE::Core::TransformManager::Scale(const Entity & e, const DirectX::XMFLOAT3 & scale)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	scalings[index].x *= scale.x;
	scalings[index].y *= scale.y;
	scalings[index].z *= scale.z;
	SetAsDirty(index);
}

void SE::Core::TransformManager::SetPosition(const Entity& e, const DirectX::XMFLOAT3& pos)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	positions[index] = pos;
	SetAsDirty(index);
}

void SE::Core::TransformManager::SetRotation(const Entity& e, float pitch, float yaw, float roll)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	rotations[index] = { pitch, yaw, roll };
	SetAsDirty(index);
}

void SE::Core::TransformManager::SetScale(const Entity& e, float scale)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	scalings[index].x = scale;
	scalings[index].y = scale;
	scalings[index].z = scale;
	SetAsDirty(index);
}

void SE::Core::TransformManager::SetScale(const Entity & e, const DirectX::XMFLOAT3 & scale)
{
	_ASSERT_EXPR(entityToIndex.find(e) != entityToIndex.end(), "Undefined entity referenced in transform manager");
	const uint32_t index = entityToIndex[e];
	scalings[index] = scale;
	SetAsDirty(index);
}

const DirectX::XMFLOAT3& SE::Core::TransformManager::GetPosition(const Entity& e) const
{
	auto entry = entityToIndex.find(e);
	_ASSERT_EXPR(entry != entityToIndex.end(), "Undefined entity referenced in transform manager");
	return positions[entry->second];
}

const DirectX::XMFLOAT3& SE::Core::TransformManager::GetRotation(const Entity& e) const
{
	auto entry = entityToIndex.find(e);
	_ASSERT_EXPR(entry != entityToIndex.end(), "Undefined entity referenced in transform manager");
	return rotations[entry->second];
}

const DirectX::XMFLOAT3& SE::Core::TransformManager::GetScale(const Entity& e) const
{
	auto entry = entityToIndex.find(e);
	_ASSERT_EXPR(entry != entityToIndex.end(), "Undefined entity referenced in transform manager");
	return scalings[entry->second];
}

const DirectX::XMFLOAT4X4 SE::Core::TransformManager::GetTransform(const Entity& e) const
{
	auto entry = entityToIndex.find(e);
	_ASSERT_EXPR(entry != entityToIndex.end(), "Undefined entity referenced in transform manager");
	auto& pos = positions[entry->second];
	auto& rot = rotations[entry->second];
	auto& scale = scalings[entry->second];
	XMFLOAT4X4 transform;
	XMStoreFloat4x4(&transform, XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z));
	return transform;

}

int SE::Core::TransformManager::GarbageCollection()
{
	StartProfile;
	const uint32_t upperIndex = std::min(garbageCollectionIndex + 4, transformCount);
	const uint32_t priorCount = transformCount;

	for(int i = garbageCollectionIndex; i < upperIndex; i++)
	{
		auto iterator = entityToIndex.begin();
		std::advance(iterator, i);
		if(!entityManager->Alive(iterator->first))
		{
			--transformCount;
			positions[iterator->second] = positions[transformCount];
			rotations[iterator->second] = rotations[transformCount];
			scalings[iterator->second] = scalings[transformCount];
			dirty[iterator->second] = dirty[transformCount];
			Parent[iterator->second] = Parent[transformCount];
			Child[iterator->second] = Child[transformCount];
			DirtyTransform[iterator->second] = DirtyTransform[transformCount];
			inheritRotation[iterator->second] = inheritRotation[transformCount];

			const Entity occupyingLastSlot = entities[transformCount];
			entityToIndex[occupyingLastSlot] = iterator->second;
			entities[iterator->second] = occupyingLastSlot;
			entityToIndex.erase(iterator);
		}
	}
	garbageCollectionIndex = upperIndex;
	if (garbageCollectionIndex >= transformCount)
		garbageCollectionIndex = 0;
	ProfileReturn(priorCount - transformCount);
}

uint32_t SE::Core::TransformManager::ActiveTransforms() const
{
	return transformCount;
}

void SE::Core::TransformManager::Frame()
{
	StartProfile;
	dirtyTransforms.clear();
	parentDeferred.clear();
	for (size_t i = 0; i < transformCount; i++)
	{
		if (dirty[i])
		{
			UpdateTransform(i);
			
		}
	}
	for (auto i : parentDeferred)
	{
		XMMATRIX local = XMLoadFloat4x4(&dirtyTransforms[DirtyTransform[i.Index]]);
		XMMATRIX parent;
		if (inheritRotation[i.Index])
			parent = XMLoadFloat4x4(&dirtyTransforms[DirtyTransform[i.parentIndex]]);
		else
			parent = XMMatrixTranslationFromVector(XMLoadFloat3(&positions[i.parentIndex]));
		
		auto newTrans = local*parent;
		XMStoreFloat4x4(&dirtyTransforms[DirtyTransform[i.Index]], newTrans);
	}
	StopProfile;
}

inline void SE::Core::TransformManager::SetAsDirty(size_t index)
{
	dirty[index] = 1u;
	if (Parent[index] != ~0u)
		dirty[Parent[index]] = 1u;
	if (Child[index] != ~0u)
		dirty[Child[index]] = 1u;

}

void SE::Core::TransformManager::UpdateTransform(size_t index)
{
	StartProfile;
	XMFLOAT4X4 transform;
	auto translation = XMMatrixTranslationFromVector(XMLoadFloat3(&positions[index]));
	auto rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotations[index]));
	auto scale = XMMatrixScalingFromVector(XMLoadFloat3(&scalings[index]));
	XMStoreFloat4x4(&transform, scale*rotation*translation);
	dirtyTransforms.push_back(transform);
	auto ti = dirtyTransforms.size() - 1;
	DirtyTransform[index] = ti;
	if (Parent[index] != ~0u)
		parentDeferred.push_back({ index, Parent[index] });
	SetDirty(entities[index], ti);
	dirty[index] = false;
	StopProfile;
}

void SE::Core::TransformManager::ExpandTransforms()
{
	StartProfile;
	//const uint32_t bytes = (transformCapacity + transformCapacityIncrement) * sizePerEntity;
	const uint32_t newCapacity = transformCapacity + transformCapacityIncrement;

	XMFLOAT3* newPos = new XMFLOAT3[newCapacity];
	XMFLOAT3* newRot = new XMFLOAT3[newCapacity];
	XMFLOAT3* newScale = new XMFLOAT3[newCapacity];
	uint8_t* newDirty = new uint8_t[newCapacity];
	Entity* newEntities = new Entity[newCapacity];
	size_t* newParent = new size_t[newCapacity];
	size_t* newDirtyTransform = new size_t[newCapacity];
	size_t* newChild = new size_t[newCapacity];
	uint8_t* newInheritRotation = new uint8_t[newCapacity];

	memcpy(newPos, positions, sizeof(XMFLOAT3) * transformCount);
	memcpy(newRot, rotations, sizeof(XMFLOAT3) * transformCount);
	memcpy(newScale, scalings, sizeof(XMFLOAT3) * transformCount);
	memcpy(newDirty, dirty, sizeof(uint8_t) * transformCount);
	memcpy(newEntities, entities, sizeof(Entity) * transformCount);
	memcpy(newParent, Parent, sizeof(size_t) * transformCount);
	memcpy(newDirtyTransform, DirtyTransform, sizeof(size_t) * transformCount);
	memcpy(newChild, Child, sizeof(size_t) * transformCount);
	memcpy(newInheritRotation, inheritRotation, sizeof(uint8_t) * transformCount);

	delete[] positions;
	delete[] rotations;
	delete[] scalings;
	delete[] dirty;
	delete[] entities;
	delete[] Parent;
	delete[] DirtyTransform;
	delete[] Child;
	delete[] inheritRotation;

	positions = newPos;
	rotations = newRot;
	scalings = newScale;
	dirty = newDirty;
	entities = newEntities;
	Parent = newParent;
	DirtyTransform = newDirtyTransform;
	Child = newChild;
	inheritRotation = newInheritRotation;

	transformCapacity = newCapacity;
	ProfileReturnVoid;
}
