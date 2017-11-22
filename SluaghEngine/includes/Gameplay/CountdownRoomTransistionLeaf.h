#ifndef SE_GAMEPLAY_COUNTDOWN_ROOM_TRANSISTION_LEAF_H
#define SE_GAMEPLAY_COUNTDOWN_ROOM_TRANSISTION_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class CountdownRoomTransistionLeaf : public IBehaviour
		{
		private:
			CountdownRoomTransistionLeaf() = delete;
		public:
			CountdownRoomTransistionLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~CountdownRoomTransistionLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new CountdownRoomTransistionLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif

