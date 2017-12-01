#include "CalculateTimeToRoomTransistionLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"
#include "Room.h"
#include <EnemyUnit.h>

SE::Gameplay::Status SE::Gameplay::CalculateTimeToRoomTransistionLeaf::Update()
{
	StartProfile;

	if (enemyBlackboard->timeToRoomTransisition > 0.f)
		myStatus = Status::BEHAVIOUR_FAILURE;
	else
	{
		float distance = enemyBlackboard->ownerPointer->GetCurrentRoom()->DistanceToClosestDoor(
			enemyBlackboard->ownerPointer->GetXPosition(),
			enemyBlackboard->ownerPointer->GetYPosition(),
			enemyBlackboard->closestDoorDirection
		);

		enemyBlackboard->timeToRoomTransisition = distance/2.5f;

		myStatus = Status::BEHAVIOUR_SUCCESS;
	}


	ProfileReturnConst(myStatus);
}
