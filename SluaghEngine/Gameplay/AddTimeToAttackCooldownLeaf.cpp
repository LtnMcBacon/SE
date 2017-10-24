#include "AddTimeToAttackCooldownLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"

SE::Gameplay::Status SE::Gameplay::AddTimeToAttackCooldownLeaf::Update()
{
	StartProfile;

	enemyBlackboard->attackCooldown += timeToAdd;

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}

SE::Gameplay::AddTimeToAttackCooldownLeaf::AddTimeToAttackCooldownLeaf(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard, float time) : IBehaviour(enemyBlackBoard, gameBlackBoard), timeToAdd(time)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::AddTimeToAttackCooldownLeaf::~AddTimeToAttackCooldownLeaf()
{
}
