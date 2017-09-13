#include "Core/Engine.h"
#include "Core/EntityManager.h"

SE::Core::Engine& SE::Core::Engine::GetInstance()
{
	static Engine instance;
	return instance;
}

int SE::Core::Engine::Init(const InitializationInfo& info)
{
	_entityManager = new EntityManager;
	return 0;
}

int SE::Core::Engine::Release()
{
	delete _entityManager;
	return 0;
}

SE::Core::EntityManager& SE::Core::Engine::GetEntityManager() const
{
	return *_entityManager;
}

SE::Core::Engine::Engine()
{
	
}

SE::Core::Engine::~Engine()
{

}
