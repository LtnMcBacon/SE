#include "GlaistigAttackLeaf.h"
#include <Utilz/GUID.h>
#include <Gameplay/GameBlackboard.h>
#include <Gameplay/EnemyBlackboard.h>
#include <Gameplay/EnemyUnit.h>

const SE::Utilz::GUID SE::Gameplay::GlaistigAttackLeaf::glaistigAttackFileGUID = Utilz::GUID("GlaistigProjectile.SEP");

SE::Gameplay::GlaistigAttackLeaf::GlaistigAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{
	
}

SE::Gameplay::Status SE::Gameplay::GlaistigAttackLeaf::Update()
{
	if(enemyBlackboard->attackCooldown <= 0.f)
	{
		ProjectileData newProjectile;

		newProjectile.fileNameGuid = glaistigAttackFileGUID;
		newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
		newProjectile.target = ValidTarget::PLAYER;
		newProjectile.startPosX = gameBlackboard->playerPositionX;
		newProjectile.startPosY = gameBlackboard->playerPositionY;
		newProjectile.eventDamage = DamageEvent(DamageEvent::DamageSources::DAMAGE_SOURCE_MAGICAL,
			DamageEvent::DamageTypes::DAMAGE_TYPE_MAGICAL, 10);

		gameBlackboard->enemyProjectiles.push_back(newProjectile);
		myStatus = Status::BEHAVIOUR_SUCCESS;

	}
	else
	{
		myStatus = Status::BEHAVIOUR_RUNNING;
	}
	return myStatus;
}
