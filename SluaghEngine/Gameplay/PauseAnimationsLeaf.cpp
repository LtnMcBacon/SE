#include "PauseAnimationsLeaf.h"
#include <Profiler.h>
#include "CoreInit.h"
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"

SE::Gameplay::Status SE::Gameplay::PauseAnimationsLeaf::Update()
{
	StartProfile;
	CoreInit::managers.animationManager->StopAllAnimations(enemyBlackboard->ownerPointer->GetEntity());
	ProfileReturnConst(myStatus);
}
