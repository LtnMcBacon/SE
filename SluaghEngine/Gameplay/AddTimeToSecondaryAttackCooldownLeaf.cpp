#include "AddTimeToSecondaryAttackCooldownLeaf.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"

SE::Gameplay::Status SE::Gameplay::AddTimeToSecondaryAttackCooldownLeaf::Update()
{
	StartProfile;

	enemyBlackboard->secondaryAttackCooldown = timeToAdd;

	ProfileReturnConst(Status::BEHAVIOUR_SUCCESS);
}

SE::Gameplay::AddTimeToSecondaryAttackCooldownLeaf::AddTimeToSecondaryAttackCooldownLeaf(GameBlackboard* gameBlackBoard,
	EnemyBlackboard* enemyBlackBoard, float time) : IBehaviour(enemyBlackBoard, gameBlackBoard), timeToAdd(time)
{
	myStatus = Status::BEHAVIOUR_SUCCESS;
}

SE::Gameplay::AddTimeToSecondaryAttackCooldownLeaf::~AddTimeToSecondaryAttackCooldownLeaf()
{
}
