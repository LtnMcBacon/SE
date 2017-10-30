#include "ResetAttackCooldownLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"

SE::Gameplay::Status SE::Gameplay::ResetAttackCooldownLeaf::Update()
{
	StartProfile;

	enemyBlackboard->attackCooldown = 0.f;

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}

SE::Gameplay::ResetAttackCooldownLeaf::ResetAttackCooldownLeaf(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard) : IBehaviour(enemyBlackBoard, gameBlackBoard)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::ResetAttackCooldownLeaf::~ResetAttackCooldownLeaf()
{
}
