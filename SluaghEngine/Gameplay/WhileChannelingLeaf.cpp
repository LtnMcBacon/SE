#include "WhileChannelingLeaf.h"
#include "EnemyBlackboard.h"
#include <Profiler.h>

SE::Gameplay::Status SE::Gameplay::WhileChannelingLeaf::Update()
{
	StartProfile;
	if (enemyBlackboard->channeling)
		ProfileReturnConst(Status::BEHAVIOUR_RUNNING);
	ProfileReturnConst(Status::BEHAVIOUR_FAILURE);
}
