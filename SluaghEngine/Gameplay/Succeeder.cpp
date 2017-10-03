#include "Succeeder.h"

using namespace SE;
using namespace GAMEPLAY;

Status Succeeder::Update()
{
	myChild->Tick();
	return myStatus;
}

Succeeder::Succeeder(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child) :
	IDecorator(enemyBlackboard, gameBlackboard, child)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

Succeeder::~Succeeder()
{
}
