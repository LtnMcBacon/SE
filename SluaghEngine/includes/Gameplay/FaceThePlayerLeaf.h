#ifndef SE_GAMEPLAY_FACE_THE_PLAYER_LEAF_H
#define SE_GAMPELAY_FACE_THE_PLAYER_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class FaceThePlayerLeaf : public IBehaviour
		{
		private:
			FaceThePlayerLeaf() = delete;
			float rotationSpeed = 6.28318530718;
		public:
			FaceThePlayerLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~FaceThePlayerLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new FaceThePlayerLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

