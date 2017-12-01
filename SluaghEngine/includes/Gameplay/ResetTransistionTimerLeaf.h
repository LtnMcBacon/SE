#ifndef SE_GAMEPLAY_RESET_TRANSISTION_TIMER_LEAF_H
#define SE_GAMEPLAY_RESET_TRANSISTION_TIMER_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class ResetTransistionTimerLeaf : public IBehaviour
		{
		private:
			ResetTransistionTimerLeaf() = delete;
		public:
			ResetTransistionTimerLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~ResetTransistionTimerLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new ResetTransistionTimerLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

