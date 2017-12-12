#include "NuckelaveeNormalAttackLeaf.h"
#include <Utilz/GUID.h>
#include <Gameplay/GameBlackboard.h>
#include <Gameplay/EnemyBlackboard.h>
#include <Gameplay/EnemyUnit.h>
#include <Profiler.h>
#include "CoreInit.h"

const SE::Utilz::GUID SE::Gameplay::NuckelaveeNormalAttackLeaf::NuckelaveeNormalAttackFileGUID = Utilz::GUID("NuckelaveeMeleeProjectile.SEP");

SE::Gameplay::NuckelaveeNormalAttackLeaf::NuckelaveeNormalAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::Status SE::Gameplay::NuckelaveeNormalAttackLeaf::Update()
{
	StartProfile;

	ProjectileData newProjectile;

	newProjectile.startRotation = CoreInit::managers.transformManager->GetRotation(enemyBlackboard->ownerPointer->GetEntity()).y;
	newProjectile.fileNameGuid = NuckelaveeNormalAttackFileGUID;
	newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
	newProjectile.target = ValidTarget::PLAYER;
	newProjectile.startPosX = enemyBlackboard->ownerPointer->GetXPosition();
	newProjectile.startPosY = enemyBlackboard->ownerPointer->GetYPosition();
	newProjectile.eventDamage = DamageEvent(DamageSources::DAMAGE_SOURCE_MELEE,
		enemyBlackboard->ownerPointer->GetDamageType(), enemyBlackboard->ownerPointer->GetNewDamage());

	gameBlackboard->enemyProjectiles.push_back(newProjectile);

	myStatus = Status::BEHAVIOUR_SUCCESS;


	ProfileReturnConst(myStatus);

}
