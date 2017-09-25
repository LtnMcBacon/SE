#include "DebugRenderManager.h"

SE::Core::DebugRenderManager::DebugRenderManager(Graphics::IRenderer* renderer, const EntityManager& entityManager,
	TransformManager* transformManager) : entityManager(entityManager), transformManager(transformManager), renderer(renderer)
{

}

SE::Core::DebugRenderManager::~DebugRenderManager()
{
}
