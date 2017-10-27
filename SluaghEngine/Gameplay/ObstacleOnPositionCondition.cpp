#include "ObstacleOnPositionCondition.h"
#include "EnemyUnit.h"
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"
#include "Flowfield.h"
#include <Profiler.h>

SE::Gameplay::ObstacleOnPositionCondition::ObstacleOnPositionCondition(EnemyBlackboard* enemyBlackboard,
	GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{
}

SE::Gameplay::ObstacleOnPositionCondition::~ObstacleOnPositionCondition()
{
}

SE::Gameplay::Status SE::Gameplay::ObstacleOnPositionCondition::Update()
{
	StartProfile;
	if(enemyBlackboard->checkedThisFrame)
	{
		if (enemyBlackboard->onObstacle)
		{
			myStatus = Status::BEHAVIOUR_SUCCESS;
		}
		else
		{
			myStatus = Status::BEHAVIOUR_FAILURE;
		}
		ProfileReturnConst(myStatus);
	}
	enemyBlackboard->checkedThisFrame = true;
	float localExtent = enemyBlackboard->extents + 0.15;

	float xPos = enemyBlackboard->ownerPointer->GetXPosition();
	float yPos = enemyBlackboard->ownerPointer->GetYPosition();

	const FlowField* flowFieldForRoom = gameBlackboard->roomFlowField;

	pos myPos;
	/*Checking collision in left x-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos - localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		enemyBlackboard->onObstacle = true;
		myStatus = Status::BEHAVIOUR_SUCCESS;
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}
	myPos.y = yPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		enemyBlackboard->onObstacle = true;
		myStatus = Status::BEHAVIOUR_SUCCESS;
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}

	/*Checking collision in right x-axis*/
	myPos.x = xPos + localExtent;
	myPos.y = yPos - localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		enemyBlackboard->onObstacle = true;
		myStatus = Status::BEHAVIOUR_SUCCESS;
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}
	myPos.y = yPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		enemyBlackboard->onObstacle = true;
		myStatus = Status::BEHAVIOUR_SUCCESS;
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}

	/*Checking collision in down y-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos - localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		enemyBlackboard->onObstacle = true;
		myStatus = Status::BEHAVIOUR_SUCCESS;
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}
	myPos.x = xPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		enemyBlackboard->onObstacle = true;
		myStatus = Status::BEHAVIOUR_SUCCESS;
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}

	/*Checking collision in up y-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		enemyBlackboard->onObstacle = true;
		myStatus = Status::BEHAVIOUR_SUCCESS;
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}
	myPos.x = xPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		enemyBlackboard->onObstacle = true;
		myStatus = Status::BEHAVIOUR_SUCCESS;
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	}
	enemyBlackboard->onObstacle = false;
	myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(Status::BEHAVIOUR_FAILURE);
}
