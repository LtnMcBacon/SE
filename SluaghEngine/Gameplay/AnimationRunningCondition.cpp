#include "AnimationRunningCondition.h"
#include <Profiler.h>
#include "CoreInit.h"
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"

SE::Gameplay::AnimationRunningCondition::AnimationRunningCondition(EnemyBlackboard* enemyBlackboard,
	GameBlackboard* gameBlackboard, std::vector<Utilz::GUID> animations) : IBehaviour(enemyBlackboard, gameBlackboard), animationsToCheck(animations)
{

}

SE::Gameplay::AnimationRunningCondition::~AnimationRunningCondition()
{

}

SE::Gameplay::Status SE::Gameplay::AnimationRunningCondition::Update()
{
	myStatus = Status::BEHAVIOUR_FAILURE;
	StartProfile;
	if (CoreInit::managers.animationManager->IsAnimationPlaying(enemyBlackboard->ownerPointer->GetEntity(), &animationsToCheck[0], animationsToCheck.size()))
		myStatus = Status::BEHAVIOUR_SUCCESS;
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}


