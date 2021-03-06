#include "PechMeleeAttackLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"
#include "EnemyUnit.h"
#include "ProjectileData.h"
#include "CoreInit.h"

const SE::Utilz::GUID SE::Gameplay::PechMeleeAttackLeaf::PechMeleeAttackFileGUID = Utilz::GUID("PechMeleeProjectile.SEP");

SE::Gameplay::PechMeleeAttackLeaf::PechMeleeAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::Status SE::Gameplay::PechMeleeAttackLeaf::Update()
{
	StartProfile;

	ProjectileData newProjectile;

	newProjectile.fileNameGuid = PechMeleeAttackFileGUID;
	newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
	newProjectile.target = ValidTarget::PLAYER;
	newProjectile.startPosX = gameBlackboard->playerPositionX;
	newProjectile.startPosY = gameBlackboard->playerPositionY;
	newProjectile.startRotation = CoreInit::managers.transformManager->GetRotation(enemyBlackboard->ownerPointer->GetEntity()).y;
	newProjectile.eventDamage = DamageEvent(DamageSources::DAMAGE_SOURCE_RANGED,
		enemyBlackboard->ownerPointer->GetDamageType(), enemyBlackboard->ownerPointer->GetNewDamage());

	gameBlackboard->enemyProjectiles.push_back(newProjectile);


	myStatus = Status::BEHAVIOUR_SUCCESS;


	ProfileReturnConst(myStatus);
}
