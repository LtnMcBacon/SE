#include "DataManager.h"
#include <Profiler.h>


SE::Core::DataManager::DataManager(const IDataManager::InitializationInfo& info) : initInfo(info)
{
}


SE::Core::DataManager::~DataManager()
{

}

void SE::Core::DataManager::SetValue(const Entity entity, const Utilz::GUID key, const values value)
{
	StartProfile;
	auto const find = entityToIndex.find(entity);

	if (find == entityToIndex.end())
		CreateData(entity);
	
	entries[entityToIndex[entity]].values[key] = value;
	
	StopProfile;
}

SE::Core::DataManager::values SE::Core::DataManager::GetValue(const Entity entity, const Utilz::GUID key, const values default_value)
{
	StartProfile;
	const auto find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
		ProfileReturnConst(default_value);
	auto const findV = entries[find->second].values.find(key);
	if (findV == entries[find->second].values.end())
		ProfileReturnConst(default_value);
	ProfileReturnConst(findV->second);
}

void SE::Core::DataManager::Frame(Utilz::TimeCluster * timer)
{
	_ASSERT(timer);
	StartProfile;
	timer->Start("DataManager");
	GarbageCollection();

	timer->Stop("DataManager");
	StopProfile;
}

void SE::Core::DataManager::Destroy(size_t index)
{
	StartProfile;
	uint32_t last = entries.size() - 1;
	const Entity e = entries[index].entity;
	const Entity last_e = entries[last].entity;

	entries[index].entity = entries[last].entity;
	entries[index].values = entries[last].values;

	entityToIndex[last_e] = index;
	entityToIndex.erase(e);

	entries.pop_back();
	StopProfile;
}

void SE::Core::DataManager::Destroy(const Entity & entity)
{
}

void SE::Core::DataManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (entries.size() > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, entries.size() - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager->Alive(entries[i].entity))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

void SE::Core::DataManager::CreateData(const Entity entity)
{
	StartProfile;
	size_t index = entityToIndex[entity] = entityToIndex.size();
	entries.push_back({ entity });
	ProfileReturnVoid;
}
