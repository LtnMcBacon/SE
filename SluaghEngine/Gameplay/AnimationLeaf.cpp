#include "AnimationLeaf.h"
#include "CoreInit.h"
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"
#include <Profiler.h>

SE::Gameplay::AnimationLeaf::AnimationLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard,
	const std::vector<Utilz::GUID> &animations, float animationDuration, const Core::AnimationFlags &animationFlags)
	: IBehaviour(enemyBlackboard, gameBlackboard), animationGUID(animations), animationFlags(animationFlags), 
		animationDuration(animationDuration)
{

}

SE::Gameplay::AnimationLeaf::~AnimationLeaf()
{

}

SE::Gameplay::Status SE::Gameplay::AnimationLeaf::Update()
{
	StartProfile;
	if (CoreInit::managers.animationManager->IsAnimationPlaying(enemyBlackboard->ownerPointer->GetEntity(), &animationGUID[0], animationGUID.size()))
		myStatus = Status::BEHAVIOUR_SUCCESS;
	else
		if (CoreInit::managers.animationManager->Start(enemyBlackboard->ownerPointer->GetEntity(), &animationGUID[0], animationGUID.size(), animationDuration, animationFlags))
			myStatus = Status::BEHAVIOUR_SUCCESS;
		else
			myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}
