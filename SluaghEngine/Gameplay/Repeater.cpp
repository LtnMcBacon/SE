#include "Repeater.h"
#include <Profiler.h>

using namespace SE;
using namespace GAMEPLAY;

void Repeater::OnInitialization()
{
	repeatCounter = 0;
}

Status Repeater::Update()
{
	StartProfile
	while(true)
	{
		myStatus = myChild->Tick();
		if (myStatus == Status::BEHAVIOUR_RUNNING)
			break;
		if (myStatus == Status::BEHAVIOUR_FAILURE)
		{
			myStatus = Status::BEHAVIOUR_RUNNING;
			break;
		}
		if (myStatus == Status::BEHAVIOUR_INVALID)
			break;
		if(++repeatCounter == repeatLimit || repeatLimit != uint_fast8_t(-1))
		{
			myStatus = Status::BEHAVIOUR_SUCCESS;
			break;
		}

	}

	ProfileReturn(myStatus)
}

Repeater::Repeater(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, 
	IBehaviour* child, uint_fast8_t repeatLimit) :
	IDecorator(enemyBlackboard, gameBlackboard, child),
	repeatLimit(repeatLimit)
{
	repeatCounter = 0;
}

Repeater::~Repeater()
{

}
