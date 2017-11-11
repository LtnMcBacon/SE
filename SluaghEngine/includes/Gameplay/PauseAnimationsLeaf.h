#ifndef SE_GAMEPLAY_PAUSE_ANIMATIONS_LEAF_H
#define SE_GAMEPLAY_PAUSE_ANIMATIONS_LEAF_H
#include "IBehaviour.h"
#include "EnemyBlackboard.h"

namespace SE
{
	namespace Gameplay
	{
		class PauseAnimationsLeaf : public IBehaviour
		{
		private:
			PauseAnimationsLeaf() = delete;
		public:
			PauseAnimationsLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~PauseAnimationsLeaf()
			{

			};

			Status Update() override;
			

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new PauseAnimationsLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif

