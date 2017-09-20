#include "EnemyUnit.h"
#include <Profiler.h>
#include "Flowfield.h"

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
	ProfileReturnVoid;
}

void SE::Gameplay::EnemyUnit::PerformAction(float dt)
{
	StartProfile;
	/*
	* Code body
	*/
	switch(entityAction)
	{
	case EnemyActions::ENEMY_ACTION_NOTHING:

		break;

	case EnemyActions::ENEMY_ACTION_MOVE:
		float xMovement;
		float yMovement;
		float xMovementTot = .0f;
		float yMovementTot = .0f;
		pos myPos;
		myPos.x = xPos;
		myPos.y = yPos;
		flowFieldForRoom->SampleFromMap(myPos, xMovement, yMovement);
		xMovementTot += xMovement;
		yMovementTot += yMovement;
		
		float moveTot = abs(xMovementTot + yMovementTot);
		if (moveTot != 0.0f)
		{
			xMovementTot /= moveTot;
			yMovementTot /= moveTot;
		}
		xPos += xMovementTot*dt;
		yPos += yMovementTot*dt;

		break;
	}
	ProfileReturnVoid;
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

SE::Gameplay::EnemyUnit::EnemyUnit(FlowField* roomFlowField, float xPos, float yPos, float maxHealth, Core::Entity *entity) :
	GameUnit(xPos, yPos, maxHealth, entity),
	flowFieldForRoom(roomFlowField)
{

}

SE::Gameplay::EnemyUnit::~EnemyUnit()
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}
