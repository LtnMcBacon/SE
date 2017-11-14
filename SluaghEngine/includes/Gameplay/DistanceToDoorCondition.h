#ifndef SE_GAMEPLAY_DISTANCE_TO_DOOR_CONDITION_LEAF_H
#define SE_GAMEPLAY_DISTANCE_TO_DOOR_CONDITION_LEAF_H
#include "IBehaviour.h"
namespace SE
{
	namespace Gameplay
	{
		class DistanceToDoorCondition : public IBehaviour
		{
		private:
			float maxDistance;
			DistanceToDoorCondition() = delete;
		public:
			DistanceToDoorCondition(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, float maxDistance) :
				IBehaviour(enemyBlackboard, gameBlackboard), maxDistance(maxDistance)
			{
				myStatus = Status::BEHAVIOUR_SUCCESS;
			};
			~DistanceToDoorCondition()
			{

			};

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new DistanceToDoorCondition(enemyBlackboard, gameBlackboard, maxDistance);
			};
		};
	}
}


#endif

