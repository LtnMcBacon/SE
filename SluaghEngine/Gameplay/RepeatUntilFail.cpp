#include "RepeatUntilFail.h"
#include <Profiler.h>

using namespace SE;
using namespace Gameplay;


Status RepeatUntilFail::Update()
{
	StartProfile
	while (true)
	{
		myStatus = myChild->Tick();
		if (myStatus != Status::BEHAVIOUR_FAILURE)
		{
			if (myStatus != Status::BEHAVIOUR_SUCCESS)
			{
				myStatus = Status::BEHAVIOUR_RUNNING;
				break;
			}
		}
		else
		{
			myStatus = Status::BEHAVIOUR_SUCCESS;
			break;
		}

	}
	ProfileReturn(myStatus)
}

RepeatUntilFail::RepeatUntilFail(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard,
	IBehaviour* child) :
	IDecorator(enemyBlackboard, gameBlackboard, child)
{
}

RepeatUntilFail::~RepeatUntilFail()
{

}
