#ifndef SE_GAMEPLAY_BODACH_MELEE_ATTACK_LEAF_H
#define SE_GAMPELAY_BODACH_MELEE_ATTACK_LEAF_H
#include "IBehaviour.h"
#include "Utilz/GUID.h"

namespace SE
{

	namespace Gameplay
	{
		class BodachMeleeAttackLeaf : public IBehaviour
		{
		private:
			BodachMeleeAttackLeaf() = delete;
		public:
			BodachMeleeAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~BodachMeleeAttackLeaf()
			{

			};
			static const Utilz::GUID BodachMeleeAttackFileGUID;
			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new BodachMeleeAttackLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif
