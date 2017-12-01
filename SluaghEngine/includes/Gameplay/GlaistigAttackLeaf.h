#ifndef SE_GAMEPLAY_GLAISTIG_ATTACK_LEAF_H
#define SE_GAMPELAY_GLAISTIG_ATTACK_LEAF_H
#include "IBehaviour.h"
#include "Utilz/GUID.h"

namespace SE
{
	
	namespace Gameplay
	{
		class GlaistigAttackLeaf : public IBehaviour
		{
		private:
			GlaistigAttackLeaf() = delete;
		public:
			GlaistigAttackLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~GlaistigAttackLeaf()
			{

			};
			static const Utilz::GUID glaistigAttackFileGUID;
			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new GlaistigAttackLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif
