#ifndef SE_GAMEPLAY_NUCKELAVEE_NORMAL_ATTACK_LEAF_H
#define SE_GAMEPLAY_NUCKELAVEE_NORMAL_ATTACK_LEAF_H
#include "IBehaviour.h"
#include "Utilz/GUID.h"

namespace SE
{

	namespace Gameplay
	{
		class NuckelaveeNormalAttackLeaf : public IBehaviour
		{
		private:
			NuckelaveeNormalAttackLeaf() = delete;
		public:
			NuckelaveeNormalAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~NuckelaveeNormalAttackLeaf()
			{

			};
			static const Utilz::GUID NuckelaveeNormalAttackFileGUID;
			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new NuckelaveeNormalAttackLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif
