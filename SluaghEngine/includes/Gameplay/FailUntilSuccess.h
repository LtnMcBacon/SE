#ifndef SE_GAMEPLAY_FAIL_UNTIL_SUCCESS_H
#define SE_GAMEPLAY_FAIL_UNTIL_SUCCESS_H

#include "IDecorator.h"

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief Returns failure until it's child node returns anything but running. Will not reset it's child before running has not been returned.
		*
		* @sa IBehaviour for the basic Behaviour, IDecorator for the class FailUntilSuccess derives from.
		*
		**/
		class FailUntilSuccess : public IDecorator
		{
		private:
			FailUntilSuccess() = delete;
		protected:

			/**
			* @brief	Tick of the child and return BEHAVIOUR_SUCCESS
			*
			* @retval BEHAVIOUR_SUCCESS The behaviour is finished and succeeded in its execution.
			*/
			Status Update() override;

		public:
			FailUntilSuccess(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child);
			~FailUntilSuccess();

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{

				return new FailUntilSuccess(enemyBlackboard, gameBlackboard,
					myChild->CopyBehaviour(gameBlackboard, enemyBlackboard));
			}
		};

	}
}



#endif