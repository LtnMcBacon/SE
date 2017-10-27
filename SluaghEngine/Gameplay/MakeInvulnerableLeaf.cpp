#include "MakeInvulnerableLeaf.h"
#include <Profiler.h>
#include "GameBlackboard.h"
#include "EnemyBlackboard.h"

SE::Gameplay::Status SE::Gameplay::MakeInvulnerableLeaf::Update()
{
	StartProfile;
	
	enemyBlackboard->invurnerable = true;

	ProfileReturnConst(myStatus)
}
