#ifndef SE_GAMEPLAY_MAKE_INVULNERABLE_LEAF_H
#define SE_GAMPELAY_MAKE_INVULNERABLE_LEAF_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		class MakeInvulnerableLeaf : public IBehaviour
		{
		private:
			MakeInvulnerableLeaf() = delete;
		public:
			MakeInvulnerableLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~MakeInvulnerableLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new MakeInvulnerableLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};
	}
}


#endif

