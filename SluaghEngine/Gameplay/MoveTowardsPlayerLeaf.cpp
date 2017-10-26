#include "MoveTowardsPlayerLeaf.h"
#include "Gameplay/EnemyBlackboard.h"
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyUnit.h"
#include <Profiler.h>
#include "CoreInit.h"


SE::Gameplay::MoveTowardsPlayerLeaf::MoveTowardsPlayerLeaf(EnemyBlackboard * enemyBlackboard, GameBlackboard * gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{
	transformManager = CoreInit::managers.transformManager;
}

SE::Gameplay::MoveTowardsPlayerLeaf::~MoveTowardsPlayerLeaf()
{

}

SE::Gameplay::Status SE::Gameplay::MoveTowardsPlayerLeaf::Update()
{
	StartProfile;
	auto enemyPtr = enemyBlackboard->ownerPointer;
	float enemyX = enemyPtr->GetXPosition();
	float enemyY = enemyPtr->GetYPosition();

	float playerX = gameBlackboard->playerPositionX;
	float playerY = gameBlackboard->playerPositionY;
	float dt = gameBlackboard->deltaTime;


	float moveX = playerX - enemyX;
	float moveY = playerY - enemyY;

	float moveTot = abs(moveX) + abs(moveY);
	if(moveTot)
	{
		moveX /= moveTot;
		moveY /= moveTot;
	}

	enemyPtr->MoveEntity(moveX*dt, moveY*dt);

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}
