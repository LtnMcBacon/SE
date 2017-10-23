#include "GlaistigAttackLeaf.h"
#include <Utilz/GUID.h>
#include <Gameplay/GameBlackboard.h>
#include <Gameplay/EnemyBlackboard.h>
#include <Gameplay/EnemyUnit.h>

const SE::Utilz::GUID SE::Gameplay::GlaistigAttackLeaf::glaistigAttackFileGUID = Utilz::GUID("GlaistigAttack.SEP");

SE::Gameplay::GlaistigAttackLeaf::GlaistigAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{
	
}

SE::Gameplay::Status SE::Gameplay::GlaistigAttackLeaf::Update()
{
	ProjectileData newProjectile;

	newProjectile.fileNameGuid = glaistigAttackFileGUID;
	newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
	newProjectile.target = ValidTarget::PLAYER;
	newProjectile.startPosX = enemyBlackboard->ownerPointer->GetXPosition();
	newProjectile.startPosY = enemyBlackboard->ownerPointer->GetYPosition();
	newProjectile.eventDamage = DamageEvent(DamageEvent::DamageSources::DAMAGE_SOURCE_MAGICAL,
		DamageEvent::DamageTypes::DAMAGE_TYPE_MAGICAL, 10);
	
	gameBlackboard->enemyProjectiles.push_back(newProjectile);

	return SE::Gameplay::Status::BEHAVIOUR_SUCCESS;
}
