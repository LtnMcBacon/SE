#ifndef SE_GAMEPLAY_STOP_ANIMATIONS_LEAF_H
#define SE_GAMEPLAY_STOP_ANIMATIONS_LEAF_H
#include "IBehaviour.h"
#include "EnemyBlackboard.h"
#include "EnemyUnit.h"
#include <Profiler.h>
#include "../../Gameplay/CoreInit.h"

namespace SE
{
	namespace Gameplay
	{
		class StopAnimationsLeaf : public IBehaviour
		{
		private:
			StopAnimationsLeaf() = delete;
		public:
			StopAnimationsLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~StopAnimationsLeaf()
			{

			};

			Status Update() override
			{
				StartProfile;
				CoreInit::managers.animationManager->StopAllAnimations(enemyBlackboard->ownerPointer->GetEntity());
				ProfileReturnConst(myStatus);
			};

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new StopAnimationsLeaf(enemyBlackboard, gameBlackboard);
			};


			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

