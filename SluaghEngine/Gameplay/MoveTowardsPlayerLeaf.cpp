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

	float moveX = playerX - enemyX;
	float moveY = playerY - enemyY;

	float moveTot = abs(moveX) + abs(moveY);
	if(moveTot)
	{
		moveX /= moveTot;
		moveY /= moveTot;
	}

	auto angle = DirectX::XMVectorGetX(DirectX::XMVector2AngleBetweenVectors(
		DirectX::XMLoadFloat3(&forward), { moveX, 1.0f, moveY, 1.0f }));

	

	if(angle < 0)
	{
		if(abs(angle) < rotationSpeed*dt)
		{
			angle = -rotationSpeed*dt;
		}
	}
	else
	{
		if (abs(angle) > rotationSpeed*dt)
		{
			angle = rotationSpeed*dt;
		}
	}
	if (angle > 0.025f || angle < 0.025f)
		transformManager->Rotate(enemyPtr->GetEntity(), 0.f, angle, 0.f);

	enemyPtr->MoveEntity(moveX*dt, moveY*dt);

	return Status::BEHAVIOUR_SUCCESS;
}
