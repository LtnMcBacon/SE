#ifndef SE_GAMEPLAY_SUCCEEDER_LEAF_H
#define SE_GAMPELAY_SUCCEEDER_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class GlaistigAttackLeaf : public IBehaviour
		{
		private:
			GlaistigAttackLeaf() = delete;
		public:
			GlaistigAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~GlaistigAttackLeaf()
			{

			};

			Status Update() override
			{
				return myStatus;
			};

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new GlaistigAttackLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif
