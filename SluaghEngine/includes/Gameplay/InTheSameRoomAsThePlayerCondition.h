#ifndef SE_GAMEPLAY_IN_THE_SAME_ROOM_AS_THE_PLAYER_CONDITION_H
#define SE_GAMEPLAY_IN_THE_SAME_ROOM_AS_THE_PLAYER_CONDITION_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class InTheSameRoomAsThePlayerCondition : public IBehaviour
		{
		private:
			InTheSameRoomAsThePlayerCondition() = delete;
			
		public:
			InTheSameRoomAsThePlayerCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~InTheSameRoomAsThePlayerCondition();


			Status Update() override;



			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new InTheSameRoomAsThePlayerCondition(enemyBlackboard, gameBlackboard);
			};
		};
	}
}
#endif