#include "RandomSelector.h"
#include <algorithm>
#include <Profiler.h>

void SE::Gameplay::RandomSelector::OnInitialization()
{
	StartProfile;
	std::shuffle(myChildren.begin(), myChildren.end(), generator);
	currentChild = myChildren.begin();
	StopProfile;
}

SE::Gameplay::Status SE::Gameplay::RandomSelector::Update()
{
	StartProfile;
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

SE::Gameplay::RandomSelector::RandomSelector(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IComposite(enemyBlackboard, gameBlackboard), generator(0)
{

}

SE::Gameplay::RandomSelector::~RandomSelector()
{

}

size_t SE::Gameplay::RandomSelector::SizeOfBehaviour() const
{
	size_t sizeOfChildrens = 0;
	for (auto child : myChildren)
		sizeOfChildrens += child->SizeOfBehaviour();

	return sizeOfChildrens + sizeof(*this) + myChildren.size()*sizeof(IBehaviour*);
}
