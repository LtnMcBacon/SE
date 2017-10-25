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

	auto& forward = transformManager->GetForward(enemyPtr->GetEntity());

	float forwardX = forward.x;
	float forwardY = forward.z;
	float forwardTot = abs(forwardX) + abs(forwardY);
	forwardX /= forwardTot;
	forwardY /= forwardTot;

	float moveX = playerX - enemyX;
	float moveY = playerY - enemyY;

	float moveTot = abs(moveX) + abs(moveY);
	if(moveTot)
	{
		moveX /= moveTot;
		moveY /= moveTot;
	}

	DirectX::XMFLOAT3 tempRot = transformManager->GetRotation(enemyPtr->GetEntity());
	DirectX::XMFLOAT3 tempForward = transformManager->GetForward(enemyPtr->GetEntity());

	DirectX::XMVECTOR defaultVector = { 0.0f, 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mouseVector(moveX, 0.0f, moveY);// = { moveX, 0.0f, moveY, 0.0f };


	int side = 1;

	float rotationAmmount = atan2f(tempForward.z, tempForward.x) - atan2f(mouseVector.z, mouseVector.x);
	
	/*Convert to 0 -> 2PI*/
	if (rotationAmmount < 0)
		rotationAmmount = DirectX::XM_2PI + rotationAmmount;

	/*If the rotation needed is greater than PI, we should rotate counter clockwise*/
	if (rotationAmmount > DirectX::XM_PI)
		side = -1;
	
	if (rotationAmmount > 0.00025)
	{
		if (rotationAmmount > rotationSpeed*dt)
			rotationAmmount = rotationSpeed*dt;
		rotationAmmount *= side;
		transformManager->Rotate(enemyPtr->GetEntity(),
			0, rotationAmmount, 0);
	}

	enemyPtr->MoveEntity(moveX*dt, moveY*dt);

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}
