#ifndef SE_GAMEPLAY_CHANGE_ROOM_LEAF_H
#define SE_GAMEPLAY_CHANGE_ROOM_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class ChangeRoomLeaf : public IBehaviour
		{
		private:
			ChangeRoomLeaf() = delete;
		public:
			ChangeRoomLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~ChangeRoomLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new ChangeRoomLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

