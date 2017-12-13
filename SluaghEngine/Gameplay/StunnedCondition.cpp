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

	if (enemyBlackboard->activeCondition & Boons::CONDITIONAL_BOONS_STUN || enemyBlackboard->activeBane & Banes::CONDITIONAL_BANES_STUN)
	{
		myStatus = Status::BEHAVIOUR_SUCCESS;
		enemyBlackboard->channeling = false;
	}
	else
		myStatus = Status::BEHAVIOUR_FAILURE;

	ProfileReturnConst(myStatus);
}
