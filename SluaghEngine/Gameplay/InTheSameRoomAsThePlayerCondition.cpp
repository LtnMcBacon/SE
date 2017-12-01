#include "InTheSameRoomAsThePlayerCondition.h"
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"
#include "EnemyUnit.h"
#include <Profiler.h>

SE::Gameplay::InTheSameRoomAsThePlayerCondition::InTheSameRoomAsThePlayerCondition(EnemyBlackboard* enemyBlackboard,
	GameBlackboard* gameBlackboard) : IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::InTheSameRoomAsThePlayerCondition::~InTheSameRoomAsThePlayerCondition()
{

}

SE::Gameplay::Status SE::Gameplay::InTheSameRoomAsThePlayerCondition::Update()
{
	StartProfile;
	if (enemyBlackboard->ownerPointer->GetCurrentRoom() == gameBlackboard->currentRoom)
	{
		myStatus = Status::BEHAVIOUR_SUCCESS;
		enemyBlackboard->timeToRoomTransisition = 0.f;
	}
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}
