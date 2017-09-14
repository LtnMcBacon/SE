#include "EntityManager.h"
#include <algorithm>

SE::Core::Entity::Entity(uint32_t id)
{
	this->id = id;
}

SE::Core::Entity::Entity()
{
	this->id = 0;
}

uint8_t SE::Core::Entity::Gen() const
{
	return ((this->id >> ENTITY_INDEX_BITS) & ENTITY_GENERATION_MASK);
}

uint32_t SE::Core::Entity::Index() const
{
	return (this->id & ENTITY_INDEX_MASK);
}

bool SE::Core::Entity::operator==(const Entity& rhs) const
{
	return this->id == rhs.id;
}

size_t SE::Core::EntityHasher::operator()(const Entity& e) const
{
	return std::hash<uint32_t>()(e.id);
}

SE::Core::EntityManager::EntityManager()
{
	_generationCapacity = _generationCapacityIncrement;
	_generation = new uint8_t[_generationCapacity]{};
	_generationCount = 0;

	_reusableIndicesCapacity = _reusableIndicesCapacityIncrement;
	_reusableIndices = new uint32_t[_reusableIndicesCapacity]{};
	_reusableIndicesCount = 0;

}

SE::Core::EntityManager::~EntityManager()
{
	delete[] _generation;
	delete[] _reusableIndices;
}

SE::Core::Entity SE::Core::EntityManager::Create()
{
	if(_reusableIndicesCount >= _minimumFreeIndices)
	{
		const uint32_t index = _reusableIndices[0];
		_reusableIndices[0] = _reusableIndices[--_reusableIndicesCount];
		return Entity((_generation[index] << Entity::ENTITY_INDEX_BITS) | index);
	}
	
	if (_generationCount == _generationCapacity)
		_ExpandGeneration();

	const uint32_t index = _generationCount++;
	return Entity((_generation[index] << Entity::ENTITY_INDEX_BITS) | index);

}

bool SE::Core::EntityManager::Alive(Entity e) const
{
	return _generation[(e.id & Entity::ENTITY_INDEX_MASK)] == ((e.id >> Entity::ENTITY_INDEX_BITS) & Entity::ENTITY_GENERATION_MASK);
}

void SE::Core::EntityManager::Destroy(Entity e)
{
	++_generation[e.id & Entity::ENTITY_INDEX_MASK];
	if (_reusableIndicesCount == _reusableIndicesCapacity)
		_ExpandReusable();
	_reusableIndices[_reusableIndicesCount++] = (e.id & Entity::ENTITY_INDEX_MASK);
}

void SE::Core::EntityManager::_ExpandGeneration()
{
	_ASSERT_EXPR(_generationCapacity + _generationCapacityIncrement < (1U << Entity::ENTITY_INDEX_BITS), L"Maximum number of entities reached.");
	uint8_t* newGen = new uint8_t[_generationCapacity + _generationCapacityIncrement]{};
	memcpy(newGen, _generation, sizeof(*_generation) * _generationCount);
	_generationCapacity += _generationCapacityIncrement;
	delete[] _generation;
	_generation = newGen;
}

void SE::Core::EntityManager::_ExpandReusable()
{
	_ASSERT_EXPR(_reusableIndicesCapacity + _reusableIndicesCapacityIncrement < (1U << Entity::ENTITY_INDEX_BITS), L"Number of reusable indices cannot exceed maximum number of entities");
	uint32_t* newReuse = new uint32_t[_reusableIndicesCapacity + _reusableIndicesCapacityIncrement]{};
	memcpy(newReuse, _reusableIndices, sizeof(uint32_t) * _reusableIndicesCapacity);
	_reusableIndicesCapacity += _reusableIndicesCapacityIncrement;
	delete[] _reusableIndices;
	_reusableIndices = newReuse;
}
