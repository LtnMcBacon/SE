#include "TickDownSecondaryAttackCooldownLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "GameBlackboard.h"

SE::Gameplay::Status SE::Gameplay::TickDownSecondaryAttackCooldownLeaf::Update()
{
	StartProfile;

	enemyBlackboard->secondaryAttackCooldown -= gameBlackboard->deltaTime;

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}

SE::Gameplay::TickDownSecondaryAttackCooldownLeaf::TickDownSecondaryAttackCooldownLeaf(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard) : IBehaviour(enemyBlackBoard, gameBlackBoard)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::TickDownSecondaryAttackCooldownLeaf::~TickDownSecondaryAttackCooldownLeaf()
{
}
