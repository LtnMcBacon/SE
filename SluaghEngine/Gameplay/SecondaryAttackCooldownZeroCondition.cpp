#include "SecondaryAttackCooldownZeroCondition.h"
#include "EnemyBlackboard.h"
#include <Profiler.h>

SE::Gameplay::Status SE::Gameplay::SecondaryAttackCooldownZeroCondition::Update()
{
	StartProfile;
	if (enemyBlackboard->secondaryAttackCooldown <= 0.f)
		myStatus = Status::BEHAVIOUR_SUCCESS;
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}

SE::Gameplay::SecondaryAttackCooldownZeroCondition::SecondaryAttackCooldownZeroCondition(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard) : IBehaviour(enemyBlackBoard, gameBlackBoard)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::SecondaryAttackCooldownZeroCondition::~SecondaryAttackCooldownZeroCondition()
{

}
