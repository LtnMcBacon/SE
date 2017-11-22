#include "SluaghRoom.h"
#include <Profiler.h>

SE::Gameplay::SluaghRoom::SluaghRoom(const Utilz::GUID& fileName, PlayerUnit* thePlayer, ProjectileManager* projectileManagers)
	:Room(fileName), projectileManagers(projectileManagers)
{
	theSluagh = new Sluagh(thePlayer);
}

SE::Gameplay::SluaghRoom::~SluaghRoom()
{

}

void SE::Gameplay::SluaghRoom::Update(float dt, float playerX, float playerY)
{
	std::vector<ProjectileData> projectiles;
	theSluagh->Update(dt, projectiles);

	projectileManagers->AddProjectiles(projectiles);
}

bool SE::Gameplay::SluaghRoom::ProjectileAgainstEnemies(Projectile& projectile)
{
	StartProfile;
	
	CollisionData cData; 
	if (theSluagh->CollisionAgainstProjectile(projectile.GetXPosition(), projectile.GetYPosition(), projectile.GetBoundingRect().radius))
	{
		cData.hitUnit = theSluagh->GetSharedPtr();
		cData.type = CollisionType::ENEMY;
	}

	if (cData.type != CollisionType::NONE)
	{
		projectile.SetCollisionData(cData);
		ProfileReturnConst(true);
	}

	ProfileReturnConst(false);
}
