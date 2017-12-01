#ifndef SE_GAMEPLAY_TICK_DOWN_ATTACK_COOLDOWN_LEAF_H
#define SE_GAMEPLAY_TICK_DOWN_ATTACK_COOLDOWN_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class TickDownAttackCooldownLeaf : public IBehaviour
		{
		private:
			TickDownAttackCooldownLeaf() = delete;
			
		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new TickDownAttackCooldownLeaf(gameBlackboard, enemyBlackboard);
			};

			TickDownAttackCooldownLeaf(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard);
			~TickDownAttackCooldownLeaf() override;

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}

#endif