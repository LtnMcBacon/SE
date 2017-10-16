#include "MoveTowardsPlayerLeaf.h"
#include "Gameplay/EnemyBlackboard.h"
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyUnit.h"
#include "Core/Engine.h"

SE::Gameplay::MoveTowardsPlayerLeaf::MoveTowardsPlayerLeaf(EnemyBlackboard * enemyBlackboard, GameBlackboard * gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{
	transformManager = &SE::Core::Engine::GetInstance().GetTransformManager();
}

SE::Gameplay::MoveTowardsPlayerLeaf::~MoveTowardsPlayerLeaf()
{

}

SE::Gameplay::Status SE::Gameplay::MoveTowardsPlayerLeaf::Update()
{
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

	DirectX::XMVECTOR defaultVector = { 0.0f, 0.0f, 1.0f, 0.0f };
	DirectX::XMVECTOR mouseVector = { moveX, 0.0f, moveY, 0.0f };


	int side;

	if (playerX < enemyX)
		side = -1;
	else
		side = 1;

	float rotationAmmount = DirectX::XMVectorGetY(
			DirectX::XMVector3AngleBetweenVectors(
				defaultVector, mouseVector)
	);
	float currentRotation = tempRot.y;

	rotationAmmount = abs(rotationAmmount - currentRotation);
	
	if (rotationAmmount > 0.00025)
	{
		if (rotationAmmount > rotationSpeed*dt)
			rotationAmmount = rotationSpeed*dt;
		rotationAmmount *= side;
		transformManager->SetRotation(enemyPtr->GetEntity(),
			tempRot.x, tempRot.y+rotationAmmount, tempRot.z);
	}
	else
	{
		int a = 0;
	}
	enemyPtr->MoveEntity(moveX*dt, moveY*dt);

	return Status::BEHAVIOUR_SUCCESS;
}
