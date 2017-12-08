#include "FlowFieldMovementLeaf.h"
#include "CoreInit.h"
#include "Gameplay/Flowfield.h"
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyBlackboard.h"
#include "EnemyUnit.h"
#include "Room.h"
#include <Profiler.h>

using namespace SE;
using namespace Gameplay;

FlowFieldMovementLeaf::FlowFieldMovementLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{
	sampleRate = CoreInit::subSystems.window->GetRand() % 40 + 40;
}

FlowFieldMovementLeaf::~FlowFieldMovementLeaf()
{

}

void FlowFieldMovementLeaf::SampleFromMap(float &xMovementTot, float &yMovementTot)
{
	StartProfile;
	const FlowField* flowField = enemyBlackboard->ownerPointer->GetCurrentRoom()->GetFlowFieldMap();
	float yMovement = 0;
	float xMovement = 0;

	float xPos = enemyBlackboard->ownerPointer->GetXPosition();
	float yPos = enemyBlackboard->ownerPointer->GetYPosition();

	pos myPos;
	/*Right*/
	myPos.x = xPos + 1.f;
	myPos.y = yPos;
	flowField->SampleFromMap(myPos, xMovement, yMovement);

	float distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
	xMovementTot += xMovement / distanceX;
	yMovementTot += yMovement / distanceX;

	/*Up-Left*/
	myPos.x = xPos + 1.f;
	myPos.y = yPos - 1.f;
	flowField->SampleFromMap(myPos, xMovement, yMovement);

	distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
	float distanceY = abs(yPos - (floor(myPos.y) + 0.5f));
	xMovementTot += xMovement / distanceX;
	yMovementTot += yMovement / distanceY;

	/*Up-Right*/
	myPos.x = xPos + 1.f;
	myPos.y = yPos + 1.f;
	flowField->SampleFromMap(myPos, xMovement, yMovement);
	distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
	distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

	xMovementTot += xMovement / distanceX;
	yMovementTot += yMovement / distanceY;

	/*Left*/
	myPos.x = xPos - 1.f;
	myPos.y = yPos;
	flowField->SampleFromMap(myPos, xMovement, yMovement);
	distanceX = abs(xPos - (floor(myPos.x) + 0.5f));

	xMovementTot += xMovement / distanceX;
	yMovementTot += yMovement / distanceX;

	/*Down-Left*/
	myPos.x = xPos - 1.f;
	myPos.y = yPos - 1.f;
	flowField->SampleFromMap(myPos, xMovement, yMovement);
	distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
	distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

	xMovementTot += xMovement / distanceX;
	yMovementTot += yMovement / distanceY;

	/*Down-Right*/
	myPos.x = xPos - 1.f;
	myPos.y = yPos + 1.f;
	flowField->SampleFromMap(myPos, xMovement, yMovement);
	distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
	distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

	xMovementTot += xMovement / distanceX;
	yMovementTot += yMovement / distanceY;

	/*Up*/
	myPos.x = xPos;
	myPos.y = yPos + 1.f;
	flowField->SampleFromMap(myPos, xMovement, yMovement);
	distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

	xMovementTot += xMovement / distanceY;
	yMovementTot += yMovement / distanceY;

	/*Down*/
	myPos.x = xPos;
	myPos.y = yPos - 1.f;
	flowField->SampleFromMap(myPos, xMovement, yMovement);
	distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

	xMovementTot += xMovement / distanceY;
	yMovementTot += yMovement / distanceY;
	StopProfile;
}

Status FlowFieldMovementLeaf::Update()
{
	StartProfile;
	const FlowField* flowField = enemyBlackboard->ownerPointer->GetCurrentRoom()->GetFlowFieldMap();
	

	float xMovement;
	float yMovement;
	float xMovementTot = enemyBlackboard->previousMovement[0];
	float yMovementTot = enemyBlackboard->previousMovement[1];

	float xPos = enemyBlackboard->ownerPointer->GetXPosition();
	float yPos = enemyBlackboard->ownerPointer->GetYPosition();

	pos myPos;
	myPos.x = xPos;
	myPos.y = yPos;


	flowField->SampleFromMap(myPos, xMovement, yMovement);
	/*Inside the player or a wall, move towards closest non-blocked wall*/
	int numberOfSamples = 0;
	//if(xMovement == 0.f && yMovement == 0.f)
	if(ceil(xPos) == ceil(gameBlackboard->playerPositionX) && ceil(yPos) == ceil(gameBlackboard->playerPositionY))
	{
		for(int i = 0; i < 9; i++)
		{
			if(i%3 == 0)
			{
				myPos.x = xPos - 1.f;
				if (i == 0)
					myPos.y -= 1.f;
				else
					myPos.y += 1.f;
			}
			else
			{
				myPos.x++;
			}
			flowField->SampleFromMap(myPos, xMovement, yMovement);
			if(xMovement != 0.f || yMovement != 0.f)
			{
				xMovement = -2*xMovement;
				yMovement = -2*yMovement;
				break;
			}
		}
	}
	xMovementTot += xMovement;
	yMovementTot += yMovement;


	if (!sample)
		SampleFromMap(yMovementTot, xMovementTot);

	sample = (sample + 1) % sampleRate;
	/*Check if we would collide in a wall
	* See CorrectCollision for information
	*/
	CorrectCollision(xMovementTot, yMovementTot);

	/*Normalize the movement vector*/
	float moveTot = abs(xMovementTot) + abs(yMovementTot);
	if (moveTot != 0.0f)
	{
		xMovementTot /= moveTot;
		yMovementTot /= moveTot;
		myStatus = Status::BEHAVIOUR_SUCCESS;
	}
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	/*Move the entity in the normalized direction*/

	enemyBlackboard->ownerPointer->MoveEntity(xMovementTot*gameBlackboard->deltaTime*enemyBlackboard->movementSpeedPercent, 
		yMovementTot*gameBlackboard->deltaTime*enemyBlackboard->movementSpeedPercent);


	/*Save the direction for next movement*/
	enemyBlackboard->previousMovement[0] = xMovementTot;
	enemyBlackboard->previousMovement[1] = yMovementTot;


	ProfileReturnConst(myStatus);
}

bool FlowFieldMovementLeaf::CorrectCollision(float& xMov, float& yMov)
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

	const FlowField* flowFieldForRoom = enemyBlackboard->ownerPointer->GetCurrentRoom()->GetFlowFieldMap();

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
