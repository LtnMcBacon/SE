#include "LeapAtPlayerLeaf.h"
#include <Profiler.h>
#include "GameBlackboard.h"
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"
#include "FlowField.h"

const SE::Utilz::GUID SE::Gameplay::LeapAtPlayerLeaf::leapAtPlayerAttackFileGUID = Utilz::GUID("BodachLeapProjectile.SEP");

bool SE::Gameplay::LeapAtPlayerLeaf::CorrectCollision(float& xMov, float& yMov)
{
	StartProfile;
	float moveTot = abs(xMov) + abs(yMov);
	float xMovementTot = xMov;
	float yMovementTot = yMov;
	if (moveTot != 0.0f)
	{
		xMovementTot /= moveTot;
		yMovementTot /= moveTot;
	}
	xMovementTot *= gameBlackboard->deltaTime;
	yMovementTot *= gameBlackboard->deltaTime;

	float localExtent = enemyBlackboard->extents + 0.15;

	float xPos = enemyBlackboard->ownerPointer->GetXPosition();
	float yPos = enemyBlackboard->ownerPointer->GetYPosition();

	const FlowField* flowFieldForRoom = gameBlackboard->roomFlowField;

	pos myPos;
	/*Checking collision in left x-axis*/
	myPos.x = xPos + xMovementTot - localExtent;
	myPos.y = yPos - localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		xMov = 0;
		ProfileReturnConst(true);
	}
	myPos.y = yPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		xMov = 0;
		ProfileReturnConst(true);
	}

	/*Checking collision in right x-axis*/
	myPos.x = xPos + xMovementTot + localExtent;
	myPos.y = yPos - localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		xMov = 0;
		ProfileReturnConst(true);
	}
	myPos.y = yPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		xMov = 0;
		ProfileReturnConst(true);
	}

	/*Checking collision in down y-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos + yMovementTot - localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		yMov = 0;
		ProfileReturnConst(true);
	}
	myPos.x = xPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		yMov = 0;
		ProfileReturnConst(true);
	}

	/*Checking collision in up y-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos + yMovementTot + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		yMov = 0;
		ProfileReturnConst(true);
	}
	myPos.x = xPos + localExtent;

	if (flowFieldForRoom->IsBlocked(myPos.x, myPos.y))
	{
		yMov = 0;
		ProfileReturnConst(true);
	}

	ProfileReturnConst(false);
}

SE::Gameplay::LeapAtPlayerLeaf::LeapAtPlayerLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard)
	: IBehaviour(enemyBlackboard, gameBlackboard)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::Status SE::Gameplay::LeapAtPlayerLeaf::Update()
{
	StartProfile;

	if(myStatus == Status::BEHAVIOUR_SUCCESS)
	{
		xMov = gameBlackboard->playerPositionX - enemyBlackboard->ownerPointer->GetXPosition();
		yMov = gameBlackboard->playerPositionY - enemyBlackboard->ownerPointer->GetYPosition();

		xTarget = gameBlackboard->playerPositionX;
		yTarget = gameBlackboard->playerPositionY;
		
		myStatus = Status::BEHAVIOUR_RUNNING;

		enemyBlackboard->ownerPointer->MoveEntity((xMov*2.5*gameBlackboard->deltaTime),
			(yMov*2.5*gameBlackboard->deltaTime));
	}
	else
	{
		float xDistance = xTarget - enemyBlackboard->ownerPointer->GetXPosition();
		float yDistance = yTarget - enemyBlackboard->ownerPointer->GetYPosition();

		float distance = sqrtf(xDistance*xDistance + yDistance*yDistance);
		if(distance < 0.25f)
		{
			enemyBlackboard->ownerPointer->PositionEntity(xTarget, yTarget);

			ProjectileData newProjectile;

			newProjectile.fileNameGuid = leapAtPlayerAttackFileGUID;
			newProjectile.ownerUnit = enemyBlackboard->ownerPointer->GetSharedPtr();
			newProjectile.target = ValidTarget::PLAYER;
			newProjectile.startPosX = xTarget;
			newProjectile.startPosY = yTarget;
			newProjectile.eventDamage = DamageEvent(DamageSources::DAMAGE_SOURCE_MELEE,
				DamageTypes::DAMAGE_TYPE_PHYSICAL, 10);

			gameBlackboard->enemyProjectiles.push_back(newProjectile);

			myStatus = Status::BEHAVIOUR_SUCCESS;
		}
		else
		{
			enemyBlackboard->ownerPointer->MoveEntity((xMov*2.5*gameBlackboard->deltaTime),
				(yMov*2.5*gameBlackboard->deltaTime));
			
		}
	}

	ProfileReturnConst(myStatus);
}
