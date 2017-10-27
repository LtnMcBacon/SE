#include "MakeVulnerableLeaf.h"
#include "Gameplay/EnemyBlackboard.h"
#include <Profiler.h>

SE::Gameplay::Status SE::Gameplay::MakeVulnerableLeaf::Update()
{
	StartProfile;

	enemyBlackboard->invurnerable = false;

	ProfileReturnConst(myStatus);
}
