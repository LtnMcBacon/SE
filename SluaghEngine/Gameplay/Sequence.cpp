#include "Sequence.h"
#include <Profiler.h>

using namespace SE;
using namespace Gameplay;

void Sequence::OnInitialization()
{
	currentChild = myChildren.begin();
}

Status Sequence::Update()
{
	StartProfile;
	while(true)
	{
		Status childStatus = (*currentChild)->Tick();
		if(childStatus != Status::BEHAVIOUR_SUCCESS)
		{
			myStatus = childStatus;
			ProfileReturn(myStatus);
		}

		if(++currentChild == myChildren.end())
		{
			myStatus = Status::BEHAVIOUR_SUCCESS;
			ProfileReturn(myStatus);
		}
	}

	myStatus = Status::BEHAVIOUR_INVALID;
	ProfileReturn(myStatus);
}

Sequence::Sequence(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IComposite(enemyBlackboard, gameBlackboard)
{

}

Sequence::~Sequence()
{

}
