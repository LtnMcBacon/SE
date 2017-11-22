#include "CountdownRoomTransistionLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"

SE::Gameplay::Status SE::Gameplay::CountdownRoomTransistionLeaf::Update()
{
	StartProfile;
	
	enemyBlackboard->timeToRoomTransisition -= gameBlackboard->deltaTime;

	if (enemyBlackboard->timeToRoomTransisition < 0.f)
		enemyBlackboard->timeToRoomTransisition = 0.f;

	ProfileReturnConst(myStatus);
}
