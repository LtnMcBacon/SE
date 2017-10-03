#include "Selector.h"
#include <Profiler.h>

using namespace SE;
using namespace GAMEPLAY;

void Selector::OnInitialization()
{
	currentChild = myChildren.begin();
}

Status Selector::Update()
{
	StartProfile
	while (true)
	{
		Status childStatus = (*currentChild)->Tick();
		if (childStatus != Status::BEHAVIOUR_FAILURE)
		{
			myStatus = childStatus;
			ProfileReturn(myStatus);
		}

		if (++currentChild == myChildren.end())
		{
			myStatus = Status::BEHAVIOUR_FAILURE;
			ProfileReturn(myStatus);
		}
	}

	myStatus = Status::BEHAVIOUR_INVALID;
	ProfileReturn(myStatus);
}

Selector::Selector(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IComposite(enemyBlackboard, gameBlackboard)
{

}

Selector::~Selector()
{

}
