#ifndef SE_GAMEPLAY_MAKE_VULNERABLE_LEAF_H
#define SE_GAMPELAY_MAKE_VULNERABLE_LEAF_H
#include "IBehaviour.h"
namespace SE
{
	namespace Gameplay
	{
		class MakeVulnerableLeaf : public IBehaviour
		{
		private:
			MakeVulnerableLeaf() = delete;
		public:
			MakeVulnerableLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~MakeVulnerableLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new MakeVulnerableLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif

