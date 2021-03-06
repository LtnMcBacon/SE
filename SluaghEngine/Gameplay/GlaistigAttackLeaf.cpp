#include "GlaistigAttackLeaf.h"
#include <Utilz/GUID.h>
#include <Gameplay/GameBlackboard.h>
#include <Gameplay/EnemyBlackboard.h>
#include <Gameplay/EnemyUnit.h>
#include <Profiler.h>
#include "CoreInit.h"

const SE::Utilz::GUID SE::Gameplay::GlaistigAttackLeaf::glaistigAttackFileGUID = Utilz::GUID("GlaistigProjectile.SEP");

SE::Gameplay::GlaistigAttackLeaf::GlaistigAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{
	
}

SE::Gameplay::Status SE::Gameplay::GlaistigAttackLeaf::Update()
{
	StartProfile;
	if(!enemyBlackboard->channeling)
	{
		ProjectileData newProjectile;

		newProjectile.fileNameGuid = glaistigAttackFileGUID;
		newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
		newProjectile.target = ValidTarget::PLAYER;
		newProjectile.startPosX = enemyBlackboard->ownerPointer->GetXPosition();
		newProjectile.startPosY = enemyBlackboard->ownerPointer->GetYPosition();
		newProjectile.startPosZ = 1.0f;
		newProjectile.startRotation = CoreInit::managers.transformManager->GetRotation(enemyBlackboard->ownerPointer->GetEntity()).y;;
		newProjectile.eventDamage = DamageEvent(Gameplay::DamageSources::DAMAGE_SOURCE_RANGED,
			enemyBlackboard->ownerPointer->GetDamageType(), enemyBlackboard->ownerPointer->GetNewDamage());

		gameBlackboard->enemyProjectiles.push_back(newProjectile);

		enemyBlackboard->channeling = true;
	}
	myStatus = Status::BEHAVIOUR_SUCCESS;

	
	ProfileReturnConst(myStatus);
}
