#include "Succeeder.h"
#include <Profiler.h>

using namespace SE;
using namespace GAMEPLAY;

Status Succeeder::Update()
{
	StartProfile
	myChild->Tick();
	ProfileReturn(myStatus);
}

Succeeder::Succeeder(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child) :
	IDecorator(enemyBlackboard, gameBlackboard, child)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

Succeeder::~Succeeder()
{
}
