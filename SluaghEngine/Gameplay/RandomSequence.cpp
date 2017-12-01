#include "RandomSequence.h"
#include <Profiler.h>
#include <algorithm>

using namespace SE;
using namespace Gameplay;

void RandomSequence::OnInitialization()
{
	StartProfile;
	std::shuffle(myChildren.begin(), myChildren.end(), generator);
	currentChild = myChildren.begin();
	StopProfile;
}

Status RandomSequence::Update()
{
	StartProfile;
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
	IComposite(enemyBlackboard, gameBlackboard), generator(0)
{

}

RandomSequence::~RandomSequence()
{

}

size_t SE::Gameplay::RandomSequence::SizeOfBehaviour() const
{
	size_t sizeOfChildrens = 0;
	for (auto child : myChildren)
		sizeOfChildrens += child->SizeOfBehaviour();

	return sizeOfChildrens + sizeof(*this) + myChildren.size() * sizeof(IBehaviour*);
}
