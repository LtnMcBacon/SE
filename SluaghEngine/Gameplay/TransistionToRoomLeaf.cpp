#include "TransistionToRoomLeaf.h"
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"
#include "EnemyUnit.h"
#include <Profiler.h>

SE::Gameplay::Status SE::Gameplay::TransistionToRoomLeaf::Update()
{
	StartProfile;
	if (enemyBlackboard->ownerPointer->GetCurrentRoom() != gameBlackboard->currentRoom)
	{
		if (enemyBlackboard->ownerPointer->GetCurrentRoom()->GetAdjacentRoomByDirection(
			enemyBlackboard->closestDoorDirection)->AddEnemyToRoom(enemyBlackboard->ownerPointer,
				enemyBlackboard->closestDoorDirection))
		{
			myStatus = Status::BEHAVIOUR_SUCCESS;
		}
		else
			myStatus = Status::BEHAVIOUR_FAILURE;
	}
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}
