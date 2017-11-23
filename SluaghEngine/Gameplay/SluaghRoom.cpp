#include "SluaghRoom.h"
#include <Profiler.h>
#include "CoreInit.h"

SE::Gameplay::SluaghRoom::SluaghRoom(const Utilz::GUID& fileName, PlayerUnit* thePlayer, ProjectileManager* projectileManagers)
	:Room(fileName), projectileManagers(projectileManagers)
{
	theSluagh = new Sluagh(thePlayer, tileValues);
	theSluagh->ToggleRendering(false);
	for(int i = 0; i < 4; i++)
	{
		DoorArr[i].active = false;
	}
	CoreInit::managers.transformManager->Move(theSluagh->GetSluagh()->GetEntity(), DirectX::XMFLOAT3{ 0.f, 0.75f,0.f });
}

SE::Gameplay::SluaghRoom::~SluaghRoom()
{

}

void SE::Gameplay::SluaghRoom::Update(float dt, float playerX, float playerY)
{
	StartProfile;
	theSluagh->ToggleRendering(true);
	std::vector<ProjectileData> projectiles;
	theSluagh->Update(dt, projectiles);

	!theSluagh->GetSluagh()->IsAlive();

	projectileManagers->AddProjectiles(projectiles);
	StopProfile;
}

bool SE::Gameplay::SluaghRoom::ProjectileAgainstEnemies(Projectile& projectile)
{
	StartProfile;
	
	CollisionData cData; 
	if (theSluagh->CollisionAgainstProjectile(projectile.GetXPosition(), projectile.GetYPosition(), projectile.GetBoundingRect().radius))
	{
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
