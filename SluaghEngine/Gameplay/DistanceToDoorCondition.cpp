#include "DistanceToDoorCondition.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"

SE::Gameplay::Status SE::Gameplay::DistanceToDoorCondition::Update()
{
	StartProfile;

	if (enemyBlackboard->ownerPointer->GetCurrentRoom()->DistanceToClosestDoor(
		enemyBlackboard->ownerPointer->GetXPosition(),
		enemyBlackboard->ownerPointer->GetYPosition(),
		enemyBlackboard->closestDoorDirection) <= maxDistance)
		myStatus = Status::BEHAVIOUR_SUCCESS;
	else
		myStatus = Status::BEHAVIOUR_FAILURE;

	ProfileReturnConst(myStatus);
}
