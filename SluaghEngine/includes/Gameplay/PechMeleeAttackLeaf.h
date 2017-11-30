#ifndef SE_GAMEPLAY_PECH_MELEE_ATTACK_LEAF_H
#define SE_GAMPELAY_PECH_MELEE_ATTACK_LEAF_H
#include "IBehaviour.h"
#include "Utilz/GUID.h"

namespace SE
{

	namespace Gameplay
	{
		class PechMeleeAttackLeaf : public IBehaviour
		{
		private:
			PechMeleeAttackLeaf() = delete;
		public:
			PechMeleeAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~PechMeleeAttackLeaf()
			{

			};
			static const Utilz::GUID PechMeleeAttackFileGUID;
			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new PechMeleeAttackLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif

