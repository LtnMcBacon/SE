#include "PechMeleeAttackLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"
#include "EnemyUnit.h"
#include "ProjectileData.h"

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
	newProjectile.eventDamage = DamageEvent(DamageSources::DAMAGE_SOURCE_RANGED,
		DamageType::NATURE, 10);

	gameBlackboard->enemyProjectiles.push_back(newProjectile);


	myStatus = Status::BEHAVIOUR_SUCCESS;


	ProfileReturnConst(myStatus);
}
