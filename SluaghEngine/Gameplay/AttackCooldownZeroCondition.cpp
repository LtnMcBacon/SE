#include "AttackCooldownZeroCondition.h"
#include "EnemyBlackboard.h"
#include <Profiler.h>

SE::Gameplay::Status SE::Gameplay::AttackCooldownZeroCondition::Update()
{
	StartProfile;
	if (enemyBlackboard->attackCooldown <= 0.f)
		myStatus = Status::BEHAVIOUR_SUCCESS;
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}

SE::Gameplay::AttackCooldownZeroCondition::AttackCooldownZeroCondition(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard) : IBehaviour(enemyBlackBoard, gameBlackBoard)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::AttackCooldownZeroCondition::~AttackCooldownZeroCondition()
{

}
