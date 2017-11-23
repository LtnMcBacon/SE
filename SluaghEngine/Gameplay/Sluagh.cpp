#include "..\includes\Gameplay\Sluagh.h"
#include "ProjectileData.h"
#include <Profiler.h>
#include "CoreInit.h"

SE::Gameplay::Sluagh::Sluagh(PlayerUnit * thePlayer, char map[25][25])
	:thePlayer(thePlayer)
{
	theSluagh = new PlayerUnit(&thePlayer->GetAllSkills()[0], nullptr, 13, 13, map);
}

SE::Gameplay::Sluagh::~Sluagh()
{
	delete theSluagh;
}

void SE::Gameplay::Sluagh::Update(float dt, std::vector<ProjectileData>& projectilesOut)
{
	StartProfile;
	PlayerUnit::MovementInput moveInput;
	PlayerUnit::ActionInput actionInput;
	/*Utility function calls here*/


	/*Mouse pos = player pos => always follow the player*/
	moveInput.mousePosX = thePlayer->GetXPosition();
	moveInput.mousePosY = thePlayer->GetYPosition();


	/*Resolve player update*/
	theSluagh->Update(dt, moveInput, projectilesOut, actionInput);


	for(auto &projectile : projectilesOut)
	{
		projectile.target = ValidTarget::PLAYER;
	}
	ProfileReturnVoid;
}

bool SE::Gameplay::Sluagh::CollisionAgainstProjectile(float projectileX, float projectileY, float projectileRadius)
{
	StartProfile;
	float xDistance = projectileX - theSluagh->GetXPosition();
	float yDistance = projectileX - theSluagh->GetYPosition();
	static const float sluaghRadius = sqrtf(0.25f*0.25f*2);
	ProfileReturn((sqrtf(xDistance*xDistance + yDistance*yDistance) < projectileRadius+sluaghRadius));
}

void SE::Gameplay::Sluagh::ToggleRendering(bool render)
{
	CoreInit::managers.eventManager->ToggleVisible(theSluagh->GetEntity(), render);
}
