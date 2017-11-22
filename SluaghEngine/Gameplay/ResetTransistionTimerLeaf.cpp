#include "ResetTransistionTimerLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"

SE::Gameplay::Status SE::Gameplay::ResetTransistionTimerLeaf::Update()
{
	StartProfile;
	
	enemyBlackboard->timeToRoomTransisition = 0.f;

	ProfileReturnConst(myStatus);
}
