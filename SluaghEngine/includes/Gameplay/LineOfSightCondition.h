#ifndef SE_GAMEPLAY_LINE_OF_SIGHT_LEAF_H
#define SE_GAMEPLAY_LINE_OF_SIGHT_LEAF_H

#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class LineOfSightCondition : public IBehaviour
		{
		private:
			LineOfSightCondition() = delete;
		public:
			LineOfSightCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~LineOfSightCondition();


			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new LineOfSightCondition(enemyBlackboard, gameBlackboard);
			};
		};
	}
}
#endif
