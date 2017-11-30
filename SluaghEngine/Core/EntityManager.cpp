#include "EntityManager.h"
#include <algorithm>



SE::Core::EntityManager::EntityManager()
{
	_generationCapacity = _generationCapacityIncrement;
	_generation = new uint8_t[_generationCapacity]{};
	_destroyCallbacks = new Utilz::Event<void(const Entity&)>[_generationCapacity];
	_generationCount = 0;

	_reusableIndicesCapacity = _reusableIndicesCapacityIncrement;
	_reusableIndices = new uint32_t[_reusableIndicesCapacity]{};
	_reusableIndicesCount = 0;

}

SE::Core::EntityManager::~EntityManager()
{
	delete[] _generation;
	delete[] _destroyCallbacks;
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

bool SE::Core::EntityManager::Alive(const Entity& e) const
{
	return _generation[e.Index()] == e.Gen();
}

void SE::Core::EntityManager::Destroy(const Entity& e)
{
	if (_generation[e.Index()] == e.Gen())
	{
		++_generation[e.Index()];

		_destroyCallbacks[e.Index()].Clear();

		if (_reusableIndicesCount == _reusableIndicesCapacity)
			_ExpandReusable();
		_reusableIndices[_reusableIndicesCount++] = e.Index();
	}
}

void SE::Core::EntityManager::DestroyNow(const Entity& e)
{
	if (_generation[e.Index()] == e.Gen())
	{
		++_generation[e.Index()];

		_destroyCallbacks[e.Index()](e);
		_destroyCallbacks[e.Index()].Clear();

		if (_reusableIndicesCount == _reusableIndicesCapacity)
			_ExpandReusable();
		_reusableIndices[_reusableIndicesCount++] = e.Index();
	}
}

void SE::Core::EntityManager::DestroyAll()
{
	for (uint32_t i = 0; i < _generationCount; ++i)
	{
		Entity e = (_generation[i] << Entity::ENTITY_INDEX_BITS) | i;
		_destroyCallbacks[e.Index()](e);
		_destroyCallbacks[e.Index()].Clear();
		++_generation[i];
	}
}

void SE::Core::EntityManager::RegisterDestroyCallback(const Entity & e, const Utilz::Delegate<void(const Entity&)>& callback)
{
	_ASSERT(_generation[e.Index()] == e.Gen());

	_destroyCallbacks[e.Index()] += callback;
}

void SE::Core::EntityManager::_ExpandGeneration()
{
	_ASSERT_EXPR(_generationCapacity + _generationCapacityIncrement < (1U << Entity::ENTITY_INDEX_BITS), L"Maximum number of entities reached.");
	uint8_t* newGen = new uint8_t[_generationCapacity + _generationCapacityIncrement]{};
	auto* newDC = new Utilz::Event<void(const Entity&)>[_generationCapacity + _generationCapacityIncrement];
	memcpy(newGen, _generation, sizeof(*_generation) * _generationCount);
	for (size_t i = 0; i < _generationCapacity; i++)
		newDC[i] = _destroyCallbacks[i];
	_generationCapacity += _generationCapacityIncrement;
	delete[] _generation;
	delete[] _destroyCallbacks;
	_generation = newGen;
	_destroyCallbacks = newDC;
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
