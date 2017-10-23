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
		if (projectiles[i].GetActive() == true)
			projectiles[i].UpdateActions(dt);

		if (projectiles[i].GetActive() == false)
		{
			//SE::Core::Engine::GetInstance().GetRenderableManager().ToggleRenderableObject(projectiles[i].GetEntity(), false);
			projectiles[i].DestroyEntity();
			//std::swap(projectiles[i], projectiles[projectiles.size() - 1]);
			projectiles[i] = projectiles.back();
			//projectiles[projectiles.size() - 1].DestroyEntity();
			projectiles.pop_back();
		}

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

SE::Gameplay::ProjectileManager::ProjectileManager(BehaviourPointers bPtrs)
{
	factory.SetBehaviourPtrs(bPtrs);
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

