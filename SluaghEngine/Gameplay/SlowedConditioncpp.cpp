#include "SlowedCondition.h"
#include "EnemyBlackboard.h"
#include <Profiler.h>

SE::Gameplay::SlowedCondition::SlowedCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard)
	:IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::SlowedCondition::~SlowedCondition()
{
}

SE::Gameplay::Status SE::Gameplay::SlowedCondition::Update()
{
	StartProfile;

	if (enemyBlackboard->activeCondition == Boons::CONDITIONAL_BOONS_SLOW)
	{
		myStatus = Status::BEHAVIOUR_SUCCESS;
		enemyBlackboard->movementSpeedPercent = 1.f - enemyBlackboard->conditionAmmount;
	}
	else
	{
		myStatus = Status::BEHAVIOUR_FAILURE;
		enemyBlackboard->movementSpeedPercent = 1.f;
	}
	ProfileReturnConst(myStatus);
}
