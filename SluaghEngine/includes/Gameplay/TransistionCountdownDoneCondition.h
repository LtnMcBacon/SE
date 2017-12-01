#ifndef SE_TRANSISTION_COUNTDOWN_DONE_CONDITION_H
#define SE_TRANSISTION_COUNTDOWN_DONE_CONDITION_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class TransistionCountdownDoneCondition : public IBehaviour
		{
		private:
			TransistionCountdownDoneCondition() = delete;
		public:
			TransistionCountdownDoneCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~TransistionCountdownDoneCondition()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new TransistionCountdownDoneCondition(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};



	}
}






#endif