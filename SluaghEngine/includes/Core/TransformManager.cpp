#include "TransformManager.h"

using namespace DirectX;

SE::Core::TransformManager::TransformManager(EntityManager* em)
{
	entityManager = em;

	transformCapacity = 512;
	transformCount = 0;
	const size_t bytes = transformCapacity * sizePerEntity;
	data = operator new[](bytes);
	memset(data, 0, bytes);
	positions = static_cast<XMFLOAT3*>(data);
	rotations = positions + transformCapacity;
	scalings = reinterpret_cast<float*>(rotations + transformCapacity);


}

SE::Core::TransformManager::~TransformManager()
{
	operator delete[](data);
}

void SE::Core::TransformManager::Create(const Entity& e, const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& rotation, float scale)
{
	if (transformCount == transformCapacity)
		ExpandTransforms();

	const uint32_t index = transformCount;
	entityToIndex[e] = index;
	positions[index] = pos;
	rotations[index] = rotation;
	scalings[index] = scale;
}

void SE::Core::TransformManager::Move(const Entity& e, const DirectX::XMFLOAT3& dir)
{
	const uint32_t index = entityToIndex[e];
	positions[index].x += dir.x;
	positions[index].y += dir.y;
	positions[index].z += dir.z;
}

DirectX::XMFLOAT3 SE::Core::TransformManager::GetPosition(const Entity& e) const
{
	const auto entry = entityToIndex.find(e);
	//Assert that the entity has an entry later...
	if(entry != entityToIndex.end())
	{
		const uint32_t index = entry->second;
		return positions[index];
	}
	return { 0.0f,0.0f,0.0f };
}

void SE::Core::TransformManager::ExpandTransforms()
{
	const uint32_t bytes = (transformCapacity + transformCapacityIncrement) * sizePerEntity;
	const uint32_t newCapacity = transformCapacity + transformCapacityIncrement;
	void* newData = operator new[](bytes);

	XMFLOAT3* newPositions = static_cast<XMFLOAT3*>(newData);
	XMFLOAT3* newRotations = newPositions + newCapacity;
	float* newScalings = reinterpret_cast<float*>(newRotations + newCapacity);

	memcpy(newPositions, positions, transformCapacity * sizeof(*newPositions));
	memcpy(newRotations, rotations, transformCapacity * sizeof(*newRotations));
	memcpy(newScalings, scalings, transformCapacity * sizeof(*newScalings));

	operator delete[](data);
	data = newData;
	transformCapacity = newCapacity;

}
