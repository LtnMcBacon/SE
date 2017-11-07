#include "AnimationRunningCondition.h"
#include <Profiler.h>
#include "CoreInit.h"
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"

SE::Gameplay::AnimationRunningCondition::AnimationRunningCondition(EnemyBlackboard* enemyBlackboard,
	GameBlackboard* gameBlackboard) : IBehaviour(enemyBlackboard, gameBlackboard)
{

}

SE::Gameplay::AnimationRunningCondition::~AnimationRunningCondition()
{

}

SE::Gameplay::Status SE::Gameplay::AnimationRunningCondition::Update()
{
	StartProfile;
	if (CoreInit::managers.animationManager->IsAnimationPlaying(enemyBlackboard->ownerPointer->GetEntity()))
		myStatus = Status::BEHAVIOUR_SUCCESS;
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}


