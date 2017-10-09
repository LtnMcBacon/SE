#include "FlowFieldMovementLeaf.h"

#include "Gameplay/Flowfield.h"
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyBlackboard.h"
#include "EnemyUnit.h"
#include <Profiler.h>

using namespace SE;
using namespace Gameplay;

FlowFieldMovementLeaf::FlowFieldMovementLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{

}

FlowFieldMovementLeaf::~FlowFieldMovementLeaf()
{

}

Status FlowFieldMovementLeaf::Update()
{
	StartProfile;
	FlowField* flowField = gameBlackboard->roomFlowField;
	

	float xMovement;
	float yMovement;
	float xMovementTot = enemyBlackboard->previousMovement[0];
	float yMovementTot = enemyBlackboard->previousMovement[1];
	float distanceX = 0.f;
	float distanceY = 0.f;

	/*Sample from the 9 cells around us, starting with the center point
	* Add the movements from the cell to total movements (one for x, one for y)
	* If applicable, divide by the distance (0.5-1.5)
	* Walls pushes away the unit with half force
	*/

	float xPos = enemyBlackboard->ownerPointer->GetXPosition();
	float yPos = enemyBlackboard->ownerPointer->GetYPosition();

	pos myPos;
	myPos.x = xPos;
	myPos.y = yPos;


	flowField->SampleFromMap(myPos, xMovement, yMovement);
	xMovementTot += xMovement;
	yMovementTot += yMovement;

	if (!sample)
	{
		/*Right*/
		myPos.x = xPos + 1.f;
		myPos.y = yPos;
		flowField->SampleFromMap(myPos, xMovement, yMovement);

		distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
		xMovementTot += xMovement / distanceX;
		yMovementTot += yMovement / distanceX;

		/*Up-Left*/
		myPos.x = xPos + 1.f;
		myPos.y = yPos - 1.f;
		flowField->SampleFromMap(myPos, xMovement, yMovement);

		distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
		distanceY = abs(yPos - (floor(myPos.y) + 0.5f));
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
	}
	sample = (sample + 1) % 60;
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
	}
	/*Move the entity in the normalized direction*/

	enemyBlackboard->ownerPointer->MoveEntity(xMovementTot*gameBlackboard->deltaTime, yMovementTot*gameBlackboard->deltaTime);


	/*Save the direction for next movement*/
	enemyBlackboard->previousMovement[0] = xMovementTot;
	enemyBlackboard->previousMovement[1] = yMovementTot;

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
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


	float sampleX = 0.f;
	float sampleY = 0.f;

	float localExtent = enemyBlackboard->extents + 0.15;

	float xPos = enemyBlackboard->ownerPointer->GetXPosition();
	float yPos = enemyBlackboard->ownerPointer->GetYPosition();

	FlowField* flowFieldForRoom = gameBlackboard->roomFlowField;

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
