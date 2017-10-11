#include "EnemyUnit.h"
#include <Profiler.h>
#include "Flowfield.h"
#include <Core\CollisionManager.h>
#include "Core/Engine.h"
#include <Gameplay/EnemyBlackboard.h>
#include <Gameplay/BehaviouralTree.h>

void SE::Gameplay::EnemyUnit::ResolveEvents()
{
	StartProfile;
	/*
	 * Code body
	 */
	ProfileReturnVoid;

}

void SE::Gameplay::EnemyUnit::DecideAction()
{
	StartProfile;
	/*
	* Code body
	*/
	entityAction = EnemyActions::ENEMY_ACTION_MOVE;
	if (myBehaviouralTree)
		myBehaviouralTree->Tick();
	ProfileReturnVoid;
}

void SE::Gameplay::EnemyUnit::PerformAction(float dt)
{
	StartProfile;
	/*
	* Code body
	*/
	if (!myBehaviouralTree)
	{
		switch (entityAction)
		{
		case EnemyActions::ENEMY_ACTION_NOTHING:

			break;

		case EnemyActions::ENEMY_ACTION_MOVE:
			float xMovement;
			float yMovement;
			float xMovementTot = previousMovement[0];
			float yMovementTot = previousMovement[1];


			/*Sample from the 9 cells around us, starting with the center point
			 * Add the movements from the cell to total movements (one for x, one for y)
			 * If applicable, divide by the distance (0.5-1.5)
			 * Walls pushes away the unit with half force
			 */
			pos myPos;
			myPos.x = xPos;
			myPos.y = yPos;

			flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);
			xMovementTot += xMovement;
			yMovementTot += yMovement;

			if (!sample)
			{
				/*Right*/
				myPos.x = xPos + 1.f;
				myPos.y = yPos;
				flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);

				float distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
				xMovementTot += xMovement / distanceX;
				yMovementTot += yMovement / distanceX;

				/*Up-Left*/
				myPos.x = xPos + 1.f;
				myPos.y = yPos - 1.f;
				flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);

				distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
				float distanceY = abs(yPos - (floor(myPos.y) + 0.5f));
				xMovementTot += xMovement / distanceX;
				yMovementTot += yMovement / distanceY;

				/*Up-Right*/
				myPos.x = xPos + 1.f;
				myPos.y = yPos + 1.f;
				flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);
				distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
				distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

				xMovementTot += xMovement / distanceX;
				yMovementTot += yMovement / distanceY;

				/*Left*/
				myPos.x = xPos - 1.f;
				myPos.y = yPos;
				flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);
				distanceX = abs(xPos - (floor(myPos.x) + 0.5f));

				xMovementTot += xMovement / distanceX;
				yMovementTot += yMovement / distanceX;

				/*Down-Left*/
				myPos.x = xPos - 1.f;
				myPos.y = yPos - 1.f;
				flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);
				distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
				distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

				xMovementTot += xMovement / distanceX;
				yMovementTot += yMovement / distanceY;

				/*Down-Right*/
				myPos.x = xPos - 1.f;
				myPos.y = yPos + 1.f;
				flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);
				distanceX = abs(xPos - (floor(myPos.x) + 0.5f));
				distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

				xMovementTot += xMovement / distanceX;
				yMovementTot += yMovement / distanceY;

				/*Up*/
				myPos.x = xPos;
				myPos.y = yPos + 1.f;
				flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);
				distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

				xMovementTot += xMovement / distanceY;
				yMovementTot += yMovement / distanceY;

				/*Down*/
				myPos.x = xPos;
				myPos.y = yPos - 1.f;
				flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);
				distanceY = abs(yPos - (floor(myPos.y) + 0.5f));

				xMovementTot += xMovement / distanceY;
				yMovementTot += yMovement / distanceY;
			}
			sample = (sample + 1) % 60;
			/*Check if we would collide in a wall
			 * See CorrectCollision for information
			 */
			CorrectCollision(dt, xMovementTot, yMovementTot);

			/*Normalize the movement vector*/
			float moveTot = abs(xMovementTot) + abs(yMovementTot);
			if (moveTot != 0.0f)
			{
				xMovementTot /= moveTot;
				yMovementTot /= moveTot;
			}
			/*Move the entity in the normalized direction*/

			MoveEntity(xMovementTot*dt, yMovementTot*dt);


			/*Save the direction for next movement*/
			previousMovement[0] = xMovementTot;
			previousMovement[1] = yMovementTot;


			break;
		}
	}
	ProfileReturnVoid;
}

bool SE::Gameplay::EnemyUnit::CorrectCollision(float dt, float &xMov, float &yMov)
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
	xMovementTot *= dt;
	yMovementTot *= dt;


	/*float sampleX = 0.f;
	float sampleY = 0.f;*/

	float localExtent = extends + 0.15;

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

void SE::Gameplay::EnemyUnit::Update(float dt)
{
	StartProfile;
	/*
	* Code body
	*/
	ResolveEvents();
	DecideAction();
	PerformAction(dt);
	ProfileReturnVoid;
}

void SE::Gameplay::EnemyUnit::AddForce(float force[2])
{
	forcesToApply[0] += force[0];
	forcesToApply[1] += force[1];
}

SE::Gameplay::EnemyUnit::EnemyUnit(const FlowField* roomFlowField, float xPos, float yPos, float maxHealth) :
	GameUnit(xPos, yPos, maxHealth),
	flowFieldForRoom(roomFlowField),
	extraSampleCoords{xPos, yPos},
	previousMovement{0,0},
	sample(0)
{
	extends = 0.25f; /*Should not be hardcoded! Obviously*/
}

SE::Gameplay::EnemyUnit::~EnemyUnit()
{
	
	/*
	* Code body
	*/
	delete myBehaviouralTree;
	delete myBlackboard;
}

