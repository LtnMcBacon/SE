#include "RandomSequence.h"
#include <Profiler.h>
#include <algorithm>

using namespace SE;
using namespace GAMEPLAY;

void RandomSequence::OnInitialization()
{
	std::random_shuffle(myChildren.begin(), myChildren.end());
	currentChild = myChildren.begin();
}

Status RandomSequence::Update()
{
	StartProfile
	while (true)
	{
		Status childStatus = (*currentChild)->Tick();
		if (childStatus != Status::BEHAVIOUR_SUCCESS)
		{
			myStatus = childStatus;
			ProfileReturn(myStatus);
		}

		if (++currentChild == myChildren.end())
		{
			myStatus = Status::BEHAVIOUR_SUCCESS;
			ProfileReturn(myStatus);
		}
	}

	myStatus = Status::BEHAVIOUR_INVALID;
	ProfileReturn(myStatus);
}

RandomSequence::RandomSequence(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IComposite(enemyBlackboard, gameBlackboard)
{

}

RandomSequence::~RandomSequence()
{

}
