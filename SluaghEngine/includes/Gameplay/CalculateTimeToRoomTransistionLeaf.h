#ifndef SE_GAMEPLAY_CALCULATE_TIME_TO_ROOM_TRANSISTION_LEAF_H
#define SE_GAMEPLAY_CALCULATE_TIME_TO_ROOM_TRANSISTION_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class CalculateTimeToRoomTransistionLeaf : public IBehaviour
		{
		private:
			CalculateTimeToRoomTransistionLeaf() = delete;
		public:
			CalculateTimeToRoomTransistionLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~CalculateTimeToRoomTransistionLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new CalculateTimeToRoomTransistionLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

