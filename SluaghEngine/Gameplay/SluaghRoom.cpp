#include "SluaghRoom.h"
#include <Profiler.h>
#include "CoreInit.h"

SE::Gameplay::SluaghRoom::SluaghRoom(const Utilz::GUID& fileName, PlayerUnit* thePlayer, ProjectileManager* projectileManagers)
	:Room(fileName), projectileManagers(projectileManagers)
{
	theSluagh = new Sluagh(thePlayer, this);
	for(int i = 0; i < 4; i++)
	{
		DoorArr[i].active = false;
	}
}

SE::Gameplay::SluaghRoom::~SluaghRoom()
{

}

void SE::Gameplay::SluaghRoom::Update(float dt, float playerX, float playerY)
{
	StartProfile;
	if (theSluagh->GetSluagh()->IsAlive() && theSluagh->GetSluagh()->GetHealth() > 0.f)
	{
		theSluagh->ToggleRendering(true);
		std::vector<ProjectileData> projectiles;
		theSluagh->Update(dt, projectiles);

		projectileManagers->AddProjectiles(projectiles);
	}
	StopProfile;
}

void SE::Gameplay::SluaghRoom::InitSluagh()
{
	StartProfile;
	theSluagh->InitializeSluagh();
	theSluagh->ToggleRendering(false);
	StopProfile;
}

bool SE::Gameplay::SluaghRoom::ProjectileAgainstEnemies(Projectile& projectile)
{
	StartProfile;
	
	CollisionData cData; 
	if (theSluagh->CollisionAgainstProjectile(projectile.GetXPosition(), projectile.GetYPosition(), projectile.GetBoundingRect().radius))
	{
		if (projectile.GetValidTarget() == ValidTarget::PLAYER)
			ProfileReturnConst(false);
		auto sluagh = theSluagh->GetSluagh();
		sluagh->AddDamageEvent(projectile.GetProjectileDamageEvent());
		sluagh->AddHealingEvent(projectile.GetProjectileHealingEvent());
		sluagh->AddConditionEvent(projectile.GetProjectileConditionEvent());
		cData.hitUnit = sluagh->GetSharedPtr();
		cData.type = CollisionType::ENEMY;
	}

	if (cData.type != CollisionType::NONE)
	{
		projectile.SetCollisionData(cData);
		ProfileReturnConst(true);
	}

	ProfileReturnConst(false);
}
