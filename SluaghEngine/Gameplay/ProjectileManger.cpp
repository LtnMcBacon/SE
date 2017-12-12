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
			projectiles[i].DestroyEntity();
			projectiles[i] = projectiles.back();
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

	for (auto& projectile : projectiles)
	{
		if (projectile.GetValidTarget() == ValidTarget::EVERYONE ||
			projectile.GetValidTarget() == unitTarget && !projectile.CheckIfAlreadyHit(unit))
		{
			if (CheckCollisionHelperFunction(unit, projectile))
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
				projectile.AddToHit(unit);
				projectile.SetCollisionData(cData);
			}
		}
	}

	StopProfile;
}

bool SE::Gameplay::ProjectileManager::CheckCollisionHelperFunction(GameUnit* unit, Projectile &projectile)
{
	StartProfile;

	bool collided = false;
	float xDiff = projectile.GetXPosition() - unit->GetXPosition();
	float yDiff = projectile.GetYPosition() - unit->GetYPosition();
	
	if (sqrtf((xDiff*xDiff) + (yDiff*yDiff)) <= projectile.GetBoundingRect().radius + sqrtf(2)*unit->GetExtent())
		collided = true;


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
