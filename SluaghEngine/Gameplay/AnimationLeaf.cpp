#include "AnimationLeaf.h"
#include "CoreInit.h"
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"
#include <Profiler.h>

SE::Gameplay::AnimationLeaf::AnimationLeaf(EnemyBlackboard* enemyBlackbodar, GameBlackboard* gameBlackboard,
	const Core::IAnimationManager::AnimationPlayInfo& playInfo) : IBehaviour(enemyBlackbodar, gameBlackboard)
{
	this->playInfo = playInfo;
}

SE::Gameplay::AnimationLeaf::AnimationLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard,
	int numberOfLayers, std::string animationNames[], float animationSpeeds[], float startKeyFrame[], bool looping[])
	: IBehaviour(enemyBlackboard, gameBlackboard)
{
	this->playInfo.nrOfLayers = 0;
	for (int i = 0; i < numberOfLayers; i++)
	{
		this->playInfo.animations[i] = animationNames[i];
		this->playInfo.animationSpeed[i] = animationSpeeds[i];
		this->playInfo.timePos[i] = startKeyFrame[i];
		this->playInfo.looping[i] = looping[i];
	}
}

SE::Gameplay::AnimationLeaf::~AnimationLeaf()
{

}

SE::Gameplay::Status SE::Gameplay::AnimationLeaf::Update()
{
	StartProfile;
	CoreInit::managers.animationManager->Start(enemyBlackboard->ownerPointer->GetEntity(), playInfo);
	myStatus = Status::BEHAVIOUR_SUCCESS;
	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}
