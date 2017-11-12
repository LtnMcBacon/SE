#include "EventManager.h"



SE::Core::EventManager::EventManager(const IEventManager::InitializationInfo& info)
{
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
}

void SE::Core::EventManager::Frame(Utilz::TimeCluster * timer)
{
}

void SE::Core::EventManager::Destroy(size_t index)
{
}

void SE::Core::EventManager::Destroy(const Entity & entity)
{
}

void SE::Core::EventManager::GarbageCollection()
{
}
