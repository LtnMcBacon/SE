#include "Inverter.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

Status Inverter::Update()
{
	StartProfile;
	myStatus = myChild->Tick();
	if (myStatus == Status::BEHAVIOUR_SUCCESS)
		myStatus = Status::BEHAVIOUR_FAILURE;
	else if (myStatus == Status::BEHAVIOUR_FAILURE)
		myStatus = Status::BEHAVIOUR_SUCCESS;
	ProfileReturn(myStatus);
}

Inverter::Inverter(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child) :
	IDecorator(enemyBlackboard, gameBlackboard, child)
{
	
}

Inverter::~Inverter()
{

}
