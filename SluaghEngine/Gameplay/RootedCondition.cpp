#include "RootedCondition.h"
#include "EnemyBlackboard.h"
#include <Profiler.h>

SE::Gameplay::RootedCondition::RootedCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard)
	:IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::RootedCondition::~RootedCondition()
{
}

SE::Gameplay::Status SE::Gameplay::RootedCondition::Update()
{
	StartProfile;

	if (enemyBlackboard->activeCondition == Boons::CONDITIONAL_BOONS_ROOT)
		myStatus = Status::BEHAVIOUR_SUCCESS;

	else
		myStatus = Status::BEHAVIOUR_FAILURE;

	ProfileReturnConst(myStatus);
}
