#include "BodachMeleeAttackLeaf.h"
#include "ProjectileData.h"
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"
#include "EnemyUnit.h"
#include <Profiler.h>

const SE::Utilz::GUID SE::Gameplay::BodachMeleeAttackLeaf::BodachMeleeAttackFileGUID = Utilz::GUID("BodachMeleeProjectile.SEP");

SE::Gameplay::BodachMeleeAttackLeaf::BodachMeleeAttackLeaf(EnemyBlackboard* enemyBlackboard,
	GameBlackboard* gameBlackboard) : IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::Status SE::Gameplay::BodachMeleeAttackLeaf::Update()
{
	StartProfile;
	
	ProjectileData newProjectile;

	newProjectile.fileNameGuid = BodachMeleeAttackFileGUID;
	newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
	newProjectile.target = ValidTarget::PLAYER;
	newProjectile.startPosX = gameBlackboard->playerPositionX;
	newProjectile.startPosY = gameBlackboard->playerPositionY;
	newProjectile.eventDamage = DamageEvent(DamageEvent::DamageSources::DAMAGE_SOURCE_MAGICAL,
		DamageEvent::DamageTypes::DAMAGE_TYPE_MAGICAL, 10);

	gameBlackboard->enemyProjectiles.push_back(newProjectile);

	
	myStatus = Status::BEHAVIOUR_SUCCESS;


	ProfileReturnConst(myStatus);
}
