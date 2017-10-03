#include "RandomSelector.h"
#include <algorithm>
#include <Profiler.h>

void SE::GAMEPLAY::RandomSelector::OnInitialization()
{
	std::random_shuffle(myChildren.begin(), myChildren.end());
	currentChild = myChildren.begin();
}

SE::GAMEPLAY::Status SE::GAMEPLAY::RandomSelector::Update()
{
	StartProfile
	while (true)
	{
		myStatus = (*currentChild)->Tick();
		if (myStatus != Status::BEHAVIOUR_FAILURE)
		{
			ProfileReturn(myStatus);
		}

		if (++currentChild == myChildren.end())
		{
			ProfileReturn(myStatus);
		}
	}
	myStatus = Status::BEHAVIOUR_INVALID;
	ProfileReturn(myStatus);
}

SE::GAMEPLAY::RandomSelector::RandomSelector(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IComposite(enemyBlackboard, gameBlackboard)
{

}

SE::GAMEPLAY::RandomSelector::~RandomSelector()
{

}
