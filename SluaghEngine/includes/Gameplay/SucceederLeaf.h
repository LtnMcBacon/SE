#ifndef SE_GAMEPLAY_SUCCEEDER_LEAF_H
#define SE_GAMPELAY_SUCCEEDER_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class SucceederLeaf : public IBehaviour
		{
		private:
			SucceederLeaf() = delete;
		public:
			SucceederLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~SucceederLeaf()
			{
				
			};

			Status Update() override
			{
				return myStatus;
			};

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new SucceederLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif
