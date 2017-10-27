#include "FaceThePlayerLeaf.h"
#include "Gameplay/EnemyBlackboard.h"
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyUnit.h"
#include <Profiler.h>
#include "CoreInit.h"

SE::Gameplay::Status SE::Gameplay::FaceThePlayerLeaf::Update()
{
	StartProfile;
	auto enemyPtr = enemyBlackboard->ownerPointer;
	float enemyX = enemyPtr->GetXPosition();
	float enemyY = enemyPtr->GetYPosition();

	float playerX = gameBlackboard->playerPositionX;
	float playerY = gameBlackboard->playerPositionY;
	float dt = gameBlackboard->deltaTime;

	auto& forward = CoreInit::managers.transformManager->GetForward(enemyPtr->GetEntity());

	float forwardX = forward.x;
	float forwardY = forward.z;
	float forwardTot = abs(forwardX) + abs(forwardY);
	forwardX /= forwardTot;
	forwardY /= forwardTot;

	float moveX = playerX - enemyX;
	float moveY = playerY - enemyY;

	float moveTot = abs(moveX) + abs(moveY);
	if (moveTot)
	{
		moveX /= moveTot;
		moveY /= moveTot;
	}

	DirectX::XMFLOAT3 tempRot = CoreInit::managers.transformManager->GetRotation(enemyPtr->GetEntity());
	DirectX::XMFLOAT3 tempForward = CoreInit::managers.transformManager->GetForward(enemyPtr->GetEntity());

	DirectX::XMVECTOR defaultVector = { 0.0f, 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mouseVector(moveX, 0.0f, moveY);


	int side = 1;

	float rotationAmmount = atan2f(tempForward.z, tempForward.x) - atan2f(mouseVector.z, mouseVector.x);

	/*Convert to 0 -> 2PI*/
	if (rotationAmmount < 0)
		rotationAmmount = DirectX::XM_2PI + rotationAmmount;

	/*If the rotation needed is greater than PI, we should rotate counter clockwise*/
	if (rotationAmmount > DirectX::XM_PI)
		side = -1;

	if (rotationAmmount > 0.0025)
	{
		if (rotationAmmount > rotationSpeed*dt)
			rotationAmmount = rotationSpeed*dt;
		rotationAmmount *= side;
		CoreInit::managers.transformManager->Rotate(enemyPtr->GetEntity(),
			0, rotationAmmount, 0);
	}

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}
