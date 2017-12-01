#include "Selector.h"
#include <Profiler.h>

using namespace SE;
using namespace Gameplay;

void Selector::OnInitialization()
{
	currentChild = myChildren.begin();
}

Status Selector::Update()
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

Selector::Selector(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IComposite(enemyBlackboard, gameBlackboard)
{

}

Selector::~Selector()
{

}

size_t SE::Gameplay::Selector::SizeOfBehaviour() const
{
	size_t sizeOfChildrens = 0;
	for (auto child : myChildren)
		sizeOfChildrens += child->SizeOfBehaviour();

	return sizeOfChildrens + sizeof(*this) + myChildren.size() * sizeof(IBehaviour*);
}
