#include "DataManager.h"



SE::Core::DataManager::DataManager()
{
}


SE::Core::DataManager::~DataManager()
{
}

void SE::Core::DataManager::AddValue(const Entity entity, const Utilz::GUID key, const values value)
{
}

SE::Core::DataManager::values SE::Core::DataManager::GetValue(const Entity entity, const Utilz::GUID key, const values default_value)
{
	return values();
}

void SE::Core::DataManager::Frame(Utilz::TimeCluster * timer)
{
}

void SE::Core::DataManager::Allocate(size_t size)
{
}

void SE::Core::DataManager::Destroy(size_t index)
{
}

void SE::Core::DataManager::Destroy(const Entity & entity)
{
}

void SE::Core::DataManager::GarbageCollection()
{
}
