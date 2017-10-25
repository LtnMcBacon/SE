#include "StunnedCondition.h"
#include "EnemyBlackboard.h"
#include <Profiler.h>

SE::Gameplay::StunnedCondition::StunnedCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard)
	:IBehaviour(enemyBlackboard, gameBlackboard)
{
	
}

SE::Gameplay::StunnedCondition::~StunnedCondition()
{
}

SE::Gameplay::Status SE::Gameplay::StunnedCondition::Update()
{
	StartProfile;

	if (enemyBlackboard->activeCondition == ConditionEvent::ConditionTypes::CONDITION_TYPE_STUN)
		myStatus = Status::BEHAVIOUR_SUCCESS;

	else
		myStatus = Status::BEHAVIOUR_FAILURE;

	ProfileReturnConst(myStatus);
}
