#ifndef SE_GAMEPLAY_POISON_CLOUD_LEAF_H
#define SE_GAMEPLAY_POISON_CLOUD_LEAF_H
#include "IBehaviour.h"
#include "Utilz/GUID.h"

namespace SE
{

	namespace Gameplay
	{
		class PoisonCloudLeaf : public IBehaviour
		{
		private:
			PoisonCloudLeaf() = delete;
		public:
			PoisonCloudLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~PoisonCloudLeaf()
			{

			};
			static const Utilz::GUID poisonAttackFileGUID;
			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new PoisonCloudLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif
