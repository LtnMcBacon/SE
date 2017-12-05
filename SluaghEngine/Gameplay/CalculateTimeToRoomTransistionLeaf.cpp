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
		
		Room::DirectionToAdjacentRoom directionToPlayer;
		bool foundRoom = false;
		for(int i = 0; i < 4; i++)
		{
			auto direction = Room::DirectionToAdjacentRoom(i);
			if(enemyBlackboard->ownerPointer->GetCurrentRoom() == gameBlackboard->currentRoom->GetAdjacentRoomByDirection(direction))
			{
				directionToPlayer = Room::ReverseDirection(direction);
				foundRoom = true;
				break;
			}
		}


		if (foundRoom)
		{
			float distance = enemyBlackboard->ownerPointer->GetCurrentRoom()->DistanceToDoorInDirection(
				enemyBlackboard->ownerPointer->GetXPosition(),
				enemyBlackboard->ownerPointer->GetYPosition(),
				directionToPlayer
			);
			distance += distance / 2.5f;
			distance /= 5.f;
			enemyBlackboard->timeToRoomTransisition = distance;
			enemyBlackboard->closestDoorDirection = directionToPlayer;
			myStatus = Status::BEHAVIOUR_SUCCESS;
		}
		else
		{
			enemyBlackboard->timeToRoomTransisition = 0.f;
			myStatus = Status::BEHAVIOUR_FAILURE;
		}
	}


	ProfileReturnConst(myStatus);
}
