#include "EventManager.h"
#include <Profiler.h>


SE::Core::EventManager::EventManager(const IEventManager::InitializationInfo& info) : initInfo(info)
{

}


SE::Core::EventManager::~EventManager()
{

}

void SE::Core::EventManager::RegisterEntityEvent(const Utilz::GUID _event,
	const EntityEventCallbacks& callbacks)
{
	if (const auto find = entityEventToIndex.find(_event); find == entityEventToIndex.end())
	{
		entityEventToIndex[_event] = entityEvents.size();
		entityEvents.push_back({ _event, callbacks });
	}
	else
	{
		entityEvents[find->second].event_ = _event;
		entityEvents[find->second].callbacks = callbacks;
	}
}

void SE::Core::EventManager::RegisterTriggerEvent(const Utilz::GUID _event, const std::function<void(const Entity)>& callback)
{
	if (const auto find = triggerEventToIndex.find(_event); find == triggerEventToIndex.end())
	{
		triggerEventToIndex[_event] = triggerEvents.size();
		triggerEvents.push_back({ _event, callback });
	}
	else
	{
		triggerEvents[find->second].event_ = _event;
		triggerEvents[find->second].callback = callback;
	}
}

void SE::Core::EventManager::TriggerEvent(const Utilz::GUID _event, bool now)
{
	StartProfile;
	if (auto const te = triggerEventToIndex.find(_event); te != triggerEventToIndex.end())
	{
		if (now)
			TriggerEvent(triggerEvents[te->second]);
		else
		{
			eventsToTrigger.push_back(te->second);
		}
	}
	StopProfile;
}

void SE::Core::EventManager::RegisterEntitytoEvent(const Entity entity, const Utilz::GUID _event)
{
	StartProfile;
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;
	if(auto const find = entityToIndex.find(entity); find == entityToIndex.end())
	{
		if (auto const findE = entityEventToIndex.find(_event); findE != entityEventToIndex.end())
		{
			size_t index = entityToIndex[entity] = entires.entity.size();
			entires.entity.push_back(entity);
			entires.eventsRegisteredTo.push_back({ _event });
			entityEvents[findE->second].entitesRegistered.push_back(entity);
			ProfileReturnVoid;
		}
		if (auto const findT = triggerEventToIndex.find(_event); findT != triggerEventToIndex.end())
		{
			size_t index = entityToIndex[entity] = entires.entity.size();
			entires.entity.push_back(entity);
			entires.eventsRegisteredTo.push_back({ _event });
			triggerEvents[findT->second].entitesRegistered.push_back(entity);
			ProfileReturnVoid;
		}
	}
	else
	{
		for (auto& e : entires.eventsRegisteredTo[find->second])
			if (e == _event)
				ProfileReturnVoid;
		if (auto const findE = entityEventToIndex.find(_event); findE != entityEventToIndex.end())
		{
			entires.eventsRegisteredTo[find->second].push_back(_event);
			entityEvents[findE->second].entitesRegistered.push_back(entity);
			ProfileReturnVoid;
		}
		if (auto const findT = triggerEventToIndex.find(_event); findT != triggerEventToIndex.end())
		{		
			entires.eventsRegisteredTo[find->second].push_back(_event);
			triggerEvents[findT->second].entitesRegistered.push_back(entity);
			ProfileReturnVoid;
		}
	}
	StopProfile;
}

void SE::Core::EventManager::UnregisterEntitytoEvent(const Entity entity, const Utilz::GUID _event)
{
	StartProfile;
	
	if (const auto find = entityToIndex.find(entity); find != entityToIndex.end())
	{
		bool found = false;
		auto& reg = entires.eventsRegisteredTo[find->second];
		for (size_t i = 0; i < reg.size(); i++)
		{
			if (reg[i] == _event)
			{
				reg[i] = reg[reg.size() - 1];
				reg.pop_back();
				found = true;
				break;
			}
		}
		if(!found)
			ProfileReturnVoid;

		if (auto const findE = entityEventToIndex.find(_event); findE != entityEventToIndex.end())
		{
			auto& reg = entityEvents[findE->second].entitesRegistered;
			for (size_t i = 0; i < reg.size(); i++)
				if (reg[i] == entity)
				{
					reg[i] = reg[reg.size() - 1];
					reg.pop_back();
					break;
				}
			ProfileReturnVoid;
		}
		if (auto const findT = triggerEventToIndex.find(_event); findT != triggerEventToIndex.end())
		{
			auto& reg = triggerEvents[findT->second].entitesRegistered;
			for (size_t i = 0; i < reg.size(); i++)
				if (reg[i] == entity)
				{
					reg[i] = reg[reg.size() - 1];
					reg.pop_back();
					break;
				}
			ProfileReturnVoid;
		}
	
		
	}
	StopProfile;
}

void SE::Core::EventManager::SetLifetime(const Entity entity, float lifetime)
{
	StartProfile;
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;
	
	if (auto const find = entityToIndex.find(entity); find == entityToIndex.end())
	{
		entires.entity.push_back(entity);
		entires.eventsRegisteredTo.push_back({});
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


	TriggerEventStruct* odes = nullptr;
	if (auto onDeathEvent = triggerEventToIndex.find("OnDeath"); onDeathEvent != triggerEventToIndex.end())
	{
		odes  = &triggerEvents[onDeathEvent->second];
	}
	float dt = initInfo.window->GetDelta();
	for (size_t i = 0; i < entires.entity.size(); i++)
	{
		float timeToDeath = std::get<float>(initInfo.dataManager->GetValue(entires.entity[i], "TimeToDeath", -1.0f));
		if (timeToDeath >= 0)
		{
			timeToDeath -= dt;
			if (timeToDeath <= 0)
			{
				if (odes)
					for (auto& e : odes->entitesRegistered)
						if (e == entires.entity[i])
							odes->callback(e);
				initInfo.entityManager->Destroy(entires.entity[i]);
				Destroy(i);
				continue;
			}
			initInfo.dataManager->SetValue(entires.entity[i], "TimeToDeath", timeToDeath);
		}
	}

	for (auto& _event : entityEvents)
	{
		auto reg = _event.entitesRegistered;
		for (auto& ent : reg)
		{
			if (_event.callbacks.triggerCheck(ent))
			{
				_event.callbacks.triggerCallback(ent);
			}
				
		}
	}

	for (auto& _event : eventsToTrigger)
	{
		for (auto& ent : triggerEvents[_event].entitesRegistered)
			triggerEvents[_event].callback(ent);
	}



	timer->Stop("EventManager");
	StopProfile;
}

void SE::Core::EventManager::Destroy(size_t index)
{
	StartProfile;
	// Temp variables
	size_t last = entires.entity.size() - 1;
	const Entity entity = entires.entity[index];
	const Entity last_entity = entires.entity[last];

	for (auto& e : entires.eventsRegisteredTo[index])
		UnregisterEntitytoEvent(entity, e);

	// Copy the data
	entires.entity[index] = last_entity;
	entires.eventsRegisteredTo[index] = entires.eventsRegisteredTo[last];

	// Replace the index for the last_entity 
	entityToIndex[last_entity] = index;
	entityToIndex.erase(entity);

	entires.entity.pop_back();
	entires.eventsRegisteredTo.pop_back();

	StopProfile;
}

void SE::Core::EventManager::Destroy(const Entity & entity)
{
}

void SE::Core::EventManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (entires.entity.size() > 0 && alive_in_row < 50U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, entires.entity.size() - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager->Alive(entires.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

void SE::Core::EventManager::TriggerEvent(const TriggerEventStruct& tvs)
{
	StartProfile;
	for (auto& ent : tvs.entitesRegistered)
		tvs.callback(ent);
	//for (size_t i = 0; i < eventData.used; i++)
	//{
	//	for (uint8_t e = 0; e < eventData.events[i].nrOfEvents; e++)
	//	{
	//		if(eventData.events[i].event_[e] == et.event_)
	//			et.callback(eventData.entity[i], eventData.events[i].userData[e])
	//	}
	//	
	//}

	StopProfile;
}
