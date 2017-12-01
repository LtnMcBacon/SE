#ifndef SE_GAMEPLAY_PECH_RANGED_ATTACK_LEAF_H
#define SE_GAMPELAY_PECH_RANGED_ATTACK_LEAF_H
#include "IBehaviour.h"
#include "Utilz/GUID.h"

namespace SE
{

	namespace Gameplay
	{
		class PechRangedAttackLeaf : public IBehaviour
		{
		private:
			PechRangedAttackLeaf() = delete;
		public:
			PechRangedAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~PechRangedAttackLeaf()
			{

			};
			static const Utilz::GUID PechMeleeAttackFileGUID;
			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new PechRangedAttackLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

