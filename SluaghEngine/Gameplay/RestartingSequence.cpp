#include "RestartingSequence.h"
#include <Profiler.h>

using namespace SE;
using namespace Gameplay;

void RestartingSequence::OnInitialization()
{
	currentChild = myChildren.begin();
}

Status RestartingSequence::Update()
{
	StartProfile;
	currentChild = myChildren.begin();
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

RestartingSequence::RestartingSequence(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IComposite(enemyBlackboard, gameBlackboard)
{

}

RestartingSequence::~RestartingSequence()
{

}

size_t SE::Gameplay::RestartingSequence::SizeOfBehaviour() const
{
	size_t sizeOfChildrens = 0;
	for (auto child : myChildren)
		sizeOfChildrens += child->SizeOfBehaviour();

	return sizeOfChildrens + sizeof(*this) + myChildren.size() * sizeof(IBehaviour*);
}
