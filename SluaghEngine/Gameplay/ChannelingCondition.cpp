#include "ChannelingCondition.h"
#include "EnemyBlackboard.h"
#include <Profiler.h>

SE::Gameplay::Status SE::Gameplay::ChannelingCondition::Update()
{
	StartProfile;
	if (enemyBlackboard->channeling)
		ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
	ProfileReturnConst(Status::BEHAVIOUR_FAILURE);
}
