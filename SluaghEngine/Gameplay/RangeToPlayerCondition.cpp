#include "RangeToPlayerCondition.h"
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyBlackboard.h"
#include "EnemyUnit.h"
#include <cmath>
#include <Profiler.h>

SE::Gameplay::Status SE::Gameplay::RangeToPlayerCondition::Update()
{
	StartProfile;
	float range = sqrt(
		powf((gameBlackboard->playerPositionX - enemyBlackboard->ownerPointer->GetXPosition()), 2.0f) +
		powf(gameBlackboard->playerPositionY - enemyBlackboard->ownerPointer->GetYPosition(), 2.0f)
	);

	if (minRange <= range && range <= maxRange)
		myStatus = Status::BEHAVIOUR_SUCCESS;
	else
		myStatus = Status::BEHAVIOUR_FAILURE;
	ProfileReturnConst(myStatus);
}

SE::Gameplay::IBehaviour* SE::Gameplay::RangeToPlayerCondition::CopyBehaviour(GameBlackboard* gameBlackboard,
	EnemyBlackboard* enemyBlackboard) const
{
	return new RangeToPlayerCondition(gameBlackboard, enemyBlackboard, minRange, maxRange);
}

SE::Gameplay::RangeToPlayerCondition::RangeToPlayerCondition(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard, float min, float max) : IBehaviour(enemyBlackBoard, gameBlackBoard)
{
	minRange = min;
	maxRange = max;
}

SE::Gameplay::RangeToPlayerCondition::~RangeToPlayerCondition()
{

}
