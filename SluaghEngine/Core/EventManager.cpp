#include "EventManager.h"
#include <Profiler.h>


SE::Core::EventManager::EventManager(const IEventManager::InitializationInfo& info) : initInfo(info)
{
	Allocate(10);
}


SE::Core::EventManager::~EventManager()
{
	operator delete(eventData.data);
}

void SE::Core::EventManager::RegisterEventCallback(const Utilz::GUID _event, const EventCallbacks& callbacks)
{
	eventToCallbacks[_event] = callbacks;
}

void SE::Core::EventManager::RegisterEntitytoEvent(const Entity entity, const Utilz::GUID _event, void* userData)
{
	StartProfile;
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;
	auto const find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
	{
		auto const findE = eventToCallbacks.find(_event);
		if (findE == eventToCallbacks.end())
			ProfileReturnVoid;

		if (eventData.used + 1 > eventData.allocated)
			Allocate(eventData.allocated * 2);
		size_t index = entityToIndex[entity] = eventData.used++;
		eventData.entity[index] = entity;

		eventData.events[index].nrOfEvents = 0;
		eventData.events[index].event_[eventData.events[index].nrOfEvents] = _event;
		eventData.events[index].userData[eventData.events[index].nrOfEvents++] = userData;
	}
	else
	{
		size_t index = entityToIndex[entity];
		_ASSERT(eventData.events[index].nrOfEvents + 1 <= Events::MAX);

		for (uint8_t i = 0; i < eventData.events[index].nrOfEvents; i++)
			if (eventData.events[index].event_[i] == _event)
				ProfileReturnVoid;

		eventData.events[index].event_[eventData.events[index].nrOfEvents] = _event;
		eventData.events[index].userData[eventData.events[index].nrOfEvents++] = userData;
	}
	StopProfile;
}

void SE::Core::EventManager::UnregisterEntitytoEvent(const Entity entity, const Utilz::GUID _event)
{
	StartProfile;
	const auto find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
	{
		auto& e = eventData.events[find->second];
		for (uint8_t i = 0; i < e.nrOfEvents; i++)
		{
			if (e.event_[i] == _event)
			{
				e.event_[i] = e.event_[e.nrOfEvents - 1];
				e.nrOfEvents--;
				break;
			}
		}
	}
	StopProfile;
}

void SE::Core::EventManager::SetLifetime(const Entity entity, float lifetime)
{
	StartProfile;
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;
	auto const find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
	{
		if (eventData.used + 1 > eventData.allocated)
			Allocate(eventData.allocated * 2);
		size_t index = entityToIndex[entity] = eventData.used++;
		eventData.entity[index] = entity;
		eventData.events[index].nrOfEvents = 0;
	}
	initInfo.dataManager->SetValue(entity, "TimeToDeath", lifetime);
	StopProfile;
}

void SE::Core::EventManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	_ASSERT(timer);
	GarbageCollection();
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

		uint8_t fs = eventData.events[i].nrOfEvents;
		for(uint8_t e = 0; e < fs; e++)
		{
			const auto& cb = eventToCallbacks[eventData.events[i].event_[e]];
			if (cb.triggerCheck(eventData.entity[i], eventData.events[i].userData[e]))
			{
				cb.triggerCallback(eventData.entity[i], eventData.events[i].userData[e]);
				if (fs > eventData.events[i].nrOfEvents)
					e--;
			}
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
