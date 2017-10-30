#ifndef SE_RANGE_TO_PLAYER_CONDITION_H
#define SE_RANGE_TO_PLAYER_CONDITION_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class RangeToPlayerCondition : public IBehaviour
		{
		private:
			RangeToPlayerCondition() = delete;
			float maxRange;
			float minRange;

		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override;

			RangeToPlayerCondition(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard, float min, float max);
			~RangeToPlayerCondition() override;
		};



	}
}






#endif