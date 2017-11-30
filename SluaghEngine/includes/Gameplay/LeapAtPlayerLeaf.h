#ifndef SE_GAMEPLAY_LEAP_AT_PLAYER_LEAF_H
#define SE_GAMPELAY_LEAP_AT_PLAYER_LEAF_H
#include "IBehaviour.h"
#include "Utilz/GUID.h"

namespace SE
{

	namespace Gameplay
	{
		class LeapAtPlayerLeaf : public IBehaviour
		{
		private:
			LeapAtPlayerLeaf() = delete;
			/**
			* @brief	Hinder collision during movement
			*
			* @details	This function checks if the units new position after a move will cause a collision.
			* If a move will cause a collision in x- or y-direction, then that direction-component will be nulled
			* before further computation is done.
			*
			* @param [in/out] xMov The x-component of the direction
			* @param [in/out] yMov The y-component of the direction
			*
			* @retval True Collision occoured
			* @retval False Collision didn't occour
			*
			*/
			bool CorrectCollision(float &xMov, float &yMov);

			float xMov, yMov;
			float xTarget, yTarget;
			float moveEachFrame = 0.f;

		public:
			LeapAtPlayerLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~LeapAtPlayerLeaf()
			{

			};
			static const Utilz::GUID leapAtPlayerAttackFileGUID;
			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new LeapAtPlayerLeaf(enemyBlackboard, gameBlackboard);
			};
		};
	}
}


#endif
