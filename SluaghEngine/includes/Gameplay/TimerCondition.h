#ifndef SE_GAMEPLAY_TIMER_CONDITION_H
#define SE_GAMPELAY_TIMER_CONDITION_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class TimerCondition : public IBehaviour
		{
		private:
			TimerCondition() = delete;
			float startTime = 0.f;
			float currentTime = 0.f;

		protected:
			void OnInitialization() override;
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override;

			TimerCondition(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard, float startTime);
			~TimerCondition() override;
		};


		
	}
}






#endif