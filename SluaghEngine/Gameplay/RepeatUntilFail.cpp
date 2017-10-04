#include "RepeatUntilFail.h"
#include <Profiler.h>


SE::Gameplay::Status SE::Gameplay::RepeatUntilFail::Update()
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

SE::Gameplay::RepeatUntilFail::RepeatUntilFail(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard,
	IBehaviour* child) :
	IDecorator(enemyBlackboard, gameBlackboard, child)
{

}

SE::Gameplay::RepeatUntilFail::~RepeatUntilFail()
{

}
