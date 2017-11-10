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
	for (auto guid : animationsToCheck)
	{
		if (CoreInit::managers.animationManager->IsAnimationPlaying(enemyBlackboard->ownerPointer->GetEntity(), guid))
		{
			myStatus = Status::BEHAVIOUR_SUCCESS;
		}
	}
	ProfileReturnConst(myStatus);
}


