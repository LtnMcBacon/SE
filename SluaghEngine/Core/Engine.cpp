#include "Core/Engine.h"
#include "Core/EntityManager.h"

SE::Core::Engine& SE::Core::Engine::GetInstance()
{
	static Engine instance;
	return instance;
}

SE::Core::EntityManager& SE::Core::Engine::GetEntityManager() const
{
	return *_entityManager;
}

SE::Core::Engine::Engine()
{
	_entityManager = new EntityManager;
}

SE::Core::Engine::~Engine()
{
	delete _entityManager;
}
