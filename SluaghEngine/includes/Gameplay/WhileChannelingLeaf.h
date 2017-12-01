#ifndef SE_GAMEPLAY_WHILE_CHANNELING_LEAF_H
#define SE_GAMPELAY_WHILE_CHANNELING_LEAF_H
#include "IBehaviour.h"
namespace SE
{
	namespace Gameplay
	{
		class WhileChannelingLeaf : public IBehaviour
		{
		private:
			WhileChannelingLeaf() = delete;
		public:
			WhileChannelingLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				IBehaviour(enemyBlackboard, gameBlackboard)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~WhileChannelingLeaf()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new WhileChannelingLeaf(enemyBlackboard, gameBlackboard);
			};

			size_t SizeOfBehaviour() const override { return sizeof(*this); };
		};

		
	}
}


#endif

