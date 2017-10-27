#include "TickDownAttackCooldownLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"

SE::Gameplay::Status SE::Gameplay::TickDownAttackCooldownLeaf::Update()
{
	StartProfile;

	enemyBlackboard->attackCooldown -= gameBlackboard->deltaTime;

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}

SE::Gameplay::TickDownAttackCooldownLeaf::TickDownAttackCooldownLeaf(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard) : IBehaviour(enemyBlackBoard, gameBlackBoard)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::TickDownAttackCooldownLeaf::~TickDownAttackCooldownLeaf()
{
}
