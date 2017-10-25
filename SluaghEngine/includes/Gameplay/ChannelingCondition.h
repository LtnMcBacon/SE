#ifndef SE_GAMEPLAY_CHANNELING_CONDITION_LEAF_H
#define SE_GAMPELAY_CHANNELING_CONDITION_LEAF_H
#include "IBehaviour.h"
namespace SE
{
	namespace Gameplay
	{
		class ChannelingCondition : public IBehaviour
		{
		private:
			ChannelingCondition() = delete;
		public:
			ChannelingCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~ChannelingCondition()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new ChannelingCondition(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif

