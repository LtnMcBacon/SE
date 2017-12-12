#include "PoisonCloudLeaf.h"
#include <Utilz/GUID.h>
#include <Gameplay/GameBlackboard.h>
#include <Gameplay/EnemyBlackboard.h>
#include <Gameplay/EnemyUnit.h>
#include <Profiler.h>
#include "CoreInit.h"

const SE::Utilz::GUID SE::Gameplay::PoisonCloudLeaf::poisonAttackFileGUID = Utilz::GUID("NuckPoisonCloudProjectile.SEP");

SE::Gameplay::PoisonCloudLeaf::PoisonCloudLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::Status SE::Gameplay::PoisonCloudLeaf::Update()
{
	StartProfile;
	
	ProjectileData newProjectile;

	newProjectile.fileNameGuid = poisonAttackFileGUID;
	newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
	newProjectile.target = ValidTarget::EVERYONE;
	newProjectile.startPosX = enemyBlackboard->ownerPointer->GetXPosition();
	newProjectile.startPosY = enemyBlackboard->ownerPointer->GetYPosition();
	newProjectile.startPosZ = 1.0f;
	newProjectile.startRotation = CoreInit::managers.transformManager->GetRotation(enemyBlackboard->ownerPointer->GetEntity()).y;;
	newProjectile.eventDamage = DamageEvent(Gameplay::DamageSources::DAMAGE_SOURCE_RANGED, Gameplay::DamageType::NATURE, 75);

	gameBlackboard->enemyProjectiles.push_back(newProjectile);

	
	myStatus = Status::BEHAVIOUR_SUCCESS;


	ProfileReturnConst(myStatus);
}
