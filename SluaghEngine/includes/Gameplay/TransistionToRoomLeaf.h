#ifndef SE_GAMEPLAY_TRANSISTION_TO_ROOM_LEAF_H
#define SE_GAMEPLAY_TRANSISTION_TO_ROOM_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class TransistionToRoomLeaf : public IBehaviour
		{
		private:
			TransistionToRoomLeaf() = delete;
		public:
			TransistionToRoomLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~TransistionToRoomLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new TransistionToRoomLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

