#include "CurrentAnimationAllowsBlendingCondition.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"
#include "../../Gameplay/CoreInit.h"

SE::Gameplay::CurrentAnimationAllowsBlendingCondition::CurrentAnimationAllowsBlendingCondition(
	EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard)
	:IBehaviour(enemyBlackboard, gameBlackboard)
{
}

SE::Gameplay::CurrentAnimationAllowsBlendingCondition::~CurrentAnimationAllowsBlendingCondition()
{

}

SE::Gameplay::Status SE::Gameplay::CurrentAnimationAllowsBlendingCondition::Update()
{
	StartProfile;

	myStatus = CoreInit::managers.animationManager->CurrentAnimationAllowsBlending(
		enemyBlackboard->ownerPointer->GetEntity()) ? Status::BEHAVIOUR_SUCCESS : Status::BEHAVIOUR_FAILURE;
	
	ProfileReturnConst(myStatus); 
}
