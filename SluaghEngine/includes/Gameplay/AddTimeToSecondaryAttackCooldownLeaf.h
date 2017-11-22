#ifndef SE_GAMEPLAY_ADD_TIME_TO_SECONDARY_ATTACK_COOLDOWN_LEAF_H
#define SE_GAMPELAY_ADD_TIME_TO_SECONDARY_ATTACK_COOLDOWN_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class AddTimeToSecondaryAttackCooldownLeaf : public IBehaviour
		{
		private:
			AddTimeToSecondaryAttackCooldownLeaf() = delete;
			float timeToAdd = 0.f;
		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new AddTimeToSecondaryAttackCooldownLeaf(gameBlackboard, enemyBlackboard, timeToAdd);
			};

			AddTimeToSecondaryAttackCooldownLeaf(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard, float time);
			~AddTimeToSecondaryAttackCooldownLeaf() override;
		};
	}
}

#endif