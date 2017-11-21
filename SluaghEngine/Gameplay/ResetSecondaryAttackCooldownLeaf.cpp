#include "ResetSecondaryAttackCooldownLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"

SE::Gameplay::Status SE::Gameplay::ResetSecondaryAttackCooldownLeaf::Update()
{
	StartProfile;

	enemyBlackboard->secondaryAttackCooldown = 0.f;

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}

SE::Gameplay::ResetSecondaryAttackCooldownLeaf::ResetSecondaryAttackCooldownLeaf(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard) : IBehaviour(enemyBlackBoard, gameBlackBoard)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::ResetSecondaryAttackCooldownLeaf::~ResetSecondaryAttackCooldownLeaf()
{
}
