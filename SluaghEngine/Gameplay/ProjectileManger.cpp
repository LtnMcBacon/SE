#include <Gameplay\ProjectileManager.h>
#include <Profiler.h>
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
		factory.CreateNewProjectile(newProjectiles[i]);
	}

	factory.GetNewProjectiles(projectiles);

	StopProfile;
}

void SE::Gameplay::ProjectileManager::CheckCollisionBetweenUnitAndProjectiles(GameUnit* unit, ValidTarget unitTarget)
{
	StartProfile;

	for (auto projectile : projectiles)
	{
		if (projectile.GetValidTarget() == ValidTarget::EVERYONE ||
			projectile.GetValidTarget() == unitTarget)
		{
			if (CheckCollisionHelperFunction(unit, projectile.GetBoundingRect()))
			{
				CollisionData cData;
				switch (unitTarget)
				{
				case ValidTarget::ENEMIES: cData.type = CollisionType::ENEMY; break;
				case ValidTarget::PLAYER: cData.type = CollisionType::PLAYER; break;
				default: ;
				}
				
				unit->AddDamageEvent(projectile.GetProjectileDamageEvent());
				unit->AddHealingEvent(projectile.GetProjectileHealingEvent());
				unit->AddConditionEvent(projectile.GetProjectileConditionEvent());
			}
		}
	}

	StopProfile;
}

bool SE::Gameplay::ProjectileManager::CheckCollisionHelperFunction(GameUnit* unit, BoundingRect projectileRect)
{
	StartProfile;

	bool collided = false;
	if (abs(unit->GetXPosition() - projectileRect.upperLeftX) < unit->GetExtent() &&
		abs(unit->GetYPosition() - projectileRect.upperLeftY) < unit->GetExtent())
	{
		collided = true;
	}
	else if (abs(unit->GetXPosition() - projectileRect.upperRightX) < unit->GetExtent() &&
		abs(unit->GetYPosition() - projectileRect.upperRightY) < unit->GetExtent())
	{
		collided = true;
	}


	ProfileReturnConst(collided);
}

SE::Gameplay::ProjectileManager::ProjectileManager(BehaviourPointers bPtrs)
{
	bPtrs.pManager = this;
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

void SE::Gameplay::ProjectileManager::RemoveAllProjectiles()
{

	for (int i = 0; i < projectiles.size(); i++)
	{
		projectiles[i].DestroyEntity();
	}

	projectiles.clear();
	
}
