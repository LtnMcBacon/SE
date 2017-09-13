#include "Core/Engine.h"

SE::Core::Engine& SE::Core::Engine::GetInstance()
{
	static Engine instance;
	return instance;
}

int SE::Core::Engine::Init(const InitializationInfo& info)
{
	entityManager = new EntityManager;
	transformManager = new TransformManager(entityManager);

	return 0;
}

int SE::Core::Engine::Release()
{
	delete transformManager;
	delete entityManager;
	entityManager = nullptr; //Just to make ReSharper stfu about function "possibly being const"
	return 0;
}

SE::Core::EntityManager& SE::Core::Engine::GetEntityManager() const
{
	return *entityManager;
}

SE::Core::TransformManager& SE::Core::Engine::GetTransformManager() const
{
	return *transformManager;
}

SE::Core::Engine::Engine()
{
	entityManager = nullptr;
	transformManager = nullptr;
}

SE::Core::Engine::~Engine()
{

}
