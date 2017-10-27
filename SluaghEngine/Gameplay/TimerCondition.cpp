#include "TimerCondition.h"
#include "Gameplay/GameBlackboard.h"
#include <Profiler.h>

void SE::Gameplay::TimerCondition::OnInitialization()
{
	this->currentTime = startTime;
}

SE::Gameplay::Status SE::Gameplay::TimerCondition::Update()
{
	StartProfile;
	this->currentTime -= gameBlackboard->deltaTime;
	if (this->currentTime < 0.f)
		myStatus = SE::Gameplay::Status::BEHAVIOUR_SUCCESS;
	else
		myStatus = SE::Gameplay::Status::BEHAVIOUR_RUNNING;

	ProfileReturnConst(myStatus);
}

SE::Gameplay::IBehaviour* SE::Gameplay::TimerCondition::CopyBehaviour(GameBlackboard* gameBlackboard,
	EnemyBlackboard* enemyBlackboard) const
{
	return new TimerCondition(gameBlackboard, enemyBlackboard, startTime);
}

SE::Gameplay::TimerCondition::TimerCondition(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard,
	float startTime) : IBehaviour(enemyBlackBoard, gameBlackBoard)
{
	this->startTime = startTime;
}

SE::Gameplay::TimerCondition::~TimerCondition()
{
}
