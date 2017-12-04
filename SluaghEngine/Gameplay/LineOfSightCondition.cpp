#include "LineOfSightCondition.h"
#include "Room.h"
#include <EnemyUnit.h>
#include "GameBlackboard.h"
#include "EnemyBlackboard.h"
#include <Profiler.h>


SE::Gameplay::LineOfSightCondition::LineOfSightCondition(EnemyBlackboard* enemyBlackboard,
	GameBlackboard* gameBlackboard) : IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::LineOfSightCondition::~LineOfSightCondition()
{

}

SE::Gameplay::Status SE::Gameplay::LineOfSightCondition::Update()
{
	StartProfile;
	if (gameBlackboard->currentRoom->CheckLineOfSightBetweenPoints(
		enemyBlackboard->ownerPointer->GetXPosition(),
		enemyBlackboard->ownerPointer->GetYPosition(),
		gameBlackboard->playerPositionX,
		gameBlackboard->playerPositionY))
	{
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}
	ProfileReturnConst(Status::BEHAVIOUR_FAILURE);
}
