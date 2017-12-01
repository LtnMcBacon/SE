#include "FailUntilSuccess.h"
#include <Profiler.h>

using namespace SE;
using namespace Gameplay;

Status FailUntilSuccess::Update()
{
	StartProfile;
	auto childStatus = myChild->Tick();
	if (childStatus == Status::BEHAVIOUR_SUCCESS)
	{
		myStatus = Status::BEHAVIOUR_SUCCESS;
	}
	else
		myStatus = Status::BEHAVIOUR_FAILURE;

	ProfileReturn(myStatus);
}

FailUntilSuccess::FailUntilSuccess(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child) :
	IDecorator(enemyBlackboard, gameBlackboard, child)
{

}

FailUntilSuccess::~FailUntilSuccess()
{

}

size_t SE::Gameplay::FailUntilSuccess::SizeOfBehaviour() const
{
	return sizeof(*this) + myChild->SizeOfBehaviour();
}
