#include "PechRangedAttackLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"
#include "EnemyUnit.h"
#include "ProjectileData.h"

const SE::Utilz::GUID SE::Gameplay::PechRangedAttackLeaf::PechMeleeAttackFileGUID = Utilz::GUID("PechRangedProjectile.SEP");
SE::Gameplay::PechRangedAttackLeaf::PechRangedAttackLeaf(EnemyBlackboard* enemyBlackboard,
	GameBlackboard* gameBlackboard) : IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::Status SE::Gameplay::PechRangedAttackLeaf::Update()
{
	StartProfile;

	ProjectileData newProjectile;

	

	newProjectile.fileNameGuid = PechMeleeAttackFileGUID;
	newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
	newProjectile.target = ValidTarget::PLAYER;
	newProjectile.startPosX = enemyBlackboard->ownerPointer->GetXPosition();
	newProjectile.startPosY = enemyBlackboard->ownerPointer->GetYPosition();
	newProjectile.eventDamage = DamageEvent(DamageSources::DAMAGE_SOURCE_RANGED,
		enemyBlackboard->ownerPointer->GetDamageType(), enemyBlackboard->ownerPointer->GetNewDamage());

	gameBlackboard->enemyProjectiles.push_back(newProjectile);


	myStatus = Status::BEHAVIOUR_SUCCESS;


	ProfileReturnConst(myStatus);
}
