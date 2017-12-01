#ifndef SE_GAMEPLAY_MOVE_TOWARDS_PLAYER_LEAF_H
#define SE_GAMEPLAY_MOVE_TOWARDS_PLAYER_LEAF_H
#include "IBehaviour.h"
#include <Core/IEngine.h>

namespace SE
{
	namespace Core
	{
		class ITransformManager;
	}
	namespace Gameplay
	{
		class MoveTowardsPlayerLeaf : public IBehaviour
		{
		private:
			MoveTowardsPlayerLeaf() = delete;
			Core::ITransformManager* transformManager;
			float rotationSpeed = DirectX::XM_2PI;
		public:
			MoveTowardsPlayerLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~MoveTowardsPlayerLeaf();


			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new MoveTowardsPlayerLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

