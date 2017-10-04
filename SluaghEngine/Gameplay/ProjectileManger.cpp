#include <Gameplay\ProjectileManager.h>
#include <Profiler.h>
#include <Core\CollisionManager.h>
#include "Core/Engine.h"

void SE::Gameplay::ProjectileManager::UpdateProjectilePositions(float dt)
{
	StartProfile;

	for (int i = 0; i < projectiles.size(); i++)
	{
		projectiles[i].UpdateMovement(dt);
	}

	StopProfile;
}

void SE::Gameplay::ProjectileManager::UpdateProjectileActions(float dt)
{
	StartProfile;

	for (int i = 0; i < projectiles.size(); i++)
	{
		projectiles[i].UpdateActions(dt);
	}

	StopProfile;

}

void SE::Gameplay::ProjectileManager::AddProjectiles(std::vector<ProjectileData>& newProjectiles)
{
	StartProfile;

	for (int i = 0; i < newProjectiles.size(); i++)
	{
		projectiles.push_back(factory.CreateNewProjectile(newProjectiles[i]));
	}

	StopProfile;
}

SE::Gameplay::ProjectileManager::ProjectileManager()
{

}

SE::Gameplay::ProjectileManager::~ProjectileManager()
{
	StartProfile;
	
	for (int i = 0; i < projectiles.size(); i++)
	{
		projectiles[i].DestroyEntity();
	}

	ProfileReturnVoid;
}

