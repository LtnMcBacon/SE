#include "TransistionCountdownDoneCondition.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"

SE::Gameplay::Status SE::Gameplay::TransistionCountdownDoneCondition::Update()
{
	StartProfile;
	if (enemyBlackboard->timeToRoomTransisition <= 0.f)
		myStatus = Status::BEHAVIOUR_SUCCESS;
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}
