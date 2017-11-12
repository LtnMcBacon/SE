#include "EventManager.h"
#include <Profiler.h>


SE::Core::EventManager::EventManager(const IEventManager::InitializationInfo& info) : initInfo(info)
{
	Allocate(10);
}


SE::Core::EventManager::~EventManager()
{
}

void SE::Core::EventManager::RegisterEventCallback(const Utilz::GUID _event, 
	const std::function<void(const Entity, const std::vector<void*>&args)> _eventTriggerCallback,
	const std::function<bool(const Entity, std::vector<void*>&args)> _eventTriggerCheck)
{
}

void SE::Core::EventManager::SetLifetime(const Entity entity, float lifetime)
{
	auto const find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
	{
		size_t index = entityToIndex[entity] = eventData.used++;
		eventData.entity[index] = entity;
		initInfo.dataManager->SetValue(entity, "TimeToDeath", lifetime);
	}
}

void SE::Core::EventManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	_ASSERT(timer);
	timer->Start("EventManager");
	float dt = initInfo.window->GetDelta();
	for (size_t i = 0; i < eventData.used; i++)
	{
		float timeToDeath = std::get<float>(initInfo.dataManager->GetValue(eventData.entity[i], "TimeToDeath", -1.0f));
		if (timeToDeath >= 0)
		{
			timeToDeath -= dt;
			if (timeToDeath <= 0)
			{
				initInfo.entityManager->Destroy(eventData.entity[i]);
				Destroy(i);
				continue;
			}
			initInfo.dataManager->SetValue(eventData.entity[i], "TimeToDeath", timeToDeath);
		}
	}

	timer->Stop("EventManager");
	StopProfile;
}

void SE::Core::EventManager::Allocate(size_t size)
{
	StartProfile;
	_ASSERT(size > eventData.allocated);

	// Allocate new memory
	EventData newData;
	newData.allocated = size;
	newData.data = operator new(size * EventData::size);
	newData.used = eventData.used;

	// Setup the new pointers
	newData.entity = (Entity*)newData.data;
	newData.events = (Events*)(newData.entity + newData.allocated);

	// Copy data
	memcpy(newData.entity, eventData.entity, eventData.used * sizeof(Entity));
	memcpy(newData.events, eventData.events, eventData.used * sizeof(Events));

	// Delete old data;
	operator delete(eventData.data);
	eventData = newData;
	StopProfile;
}

void SE::Core::EventManager::Destroy(size_t index)
{
	StartProfile;
	// Temp variables
	size_t last = eventData.used - 1;
	const Entity entity = eventData.entity[index];
	const Entity last_entity = eventData.entity[last];

	// Copy the data
	eventData.entity[index] = last_entity;
	eventData.events[index] = eventData.events[last];

	// Replace the index for the last_entity 
	entityToIndex[last_entity] = index;
	entityToIndex.erase(entity);

	eventData.used--;

}

void SE::Core::EventManager::Destroy(const Entity & entity)
{
}

void SE::Core::EventManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (eventData.used > 0 && alive_in_row < 50U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, eventData.used - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager->Alive(eventData.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}
