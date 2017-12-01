#ifndef SE_GAMEPLAY_ADD_TIME_TO_ATTACK_COOLDOWN_LEAF_H
#define SE_GAMPELAY_ADD_TIME_TO_ATTACK_COOLDOWN_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class AddTimeToAttackCooldownLeaf : public IBehaviour
		{
		private:
			AddTimeToAttackCooldownLeaf() = delete;
			float timeToAdd = 0.f;
		protected:
			Status Update() override;
		public:
			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new AddTimeToAttackCooldownLeaf(gameBlackboard, enemyBlackboard, timeToAdd);
			};

			AddTimeToAttackCooldownLeaf(GameBlackboard* gameBlackBoard, EnemyBlackboard* enemyBlackBoard, float time);
			~AddTimeToAttackCooldownLeaf() override;

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}

#endif