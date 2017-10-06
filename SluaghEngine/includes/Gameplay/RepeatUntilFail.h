#ifndef SE_GAMEPLAY_REPEAT_UNTIL_FAIL_H
#define SE_GAMEPLAY_REPEAT_UNTIL_FAIL_H

#include "IDecorator.h"

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The RepeatUntilFail tick of its child until a failure is returned.
		*
		* @sa IBehaviour for the basic Behaviour, IDecorator for the class RepeatUntilFail derives from,
		* Repeater for the Repeater that goes on a set number of times.
		*
		**/
		class RepeatUntilFail : public IDecorator
		{
		private:
			RepeatUntilFail() = delete;
		protected:

			/**
			* @brief	Goes through the child until fail..
			*
			* @details	Update its child, and continues to do so until a child returns BEHAVIOUR_FAILURE.
			* If a child returns something but BEHAVIOUR_FAILURE and BEHAVIOUR_SUCCESS, RepeatUntilFail
			* will return BEHAVIOUR_RUNNING
			*
			*
			* @retval BEHAVIOUR_FAILURE The behaviour failed to complete.
			* @retval BEHAVIOUR_RUNNING The behaviour is executing, and may take multiple frames to finish.
			* @retval BEHAVIOUR_SUCCESS The behaviour is finished and succeeded in its execution.
			* @retval BEHAVIOUR_SUSPENDED The behaviour is suspended by another node, and should be checked in on later.
			* @retval BEHAVIOUR_INVALID Something is wrong, since this should only be a "default" state.
			*
			*/
			Status Update() override;
		public:
			RepeatUntilFail(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child);
			~RepeatUntilFail();

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new RepeatUntilFail(enemyBlackboard, gameBlackboard,
					myChild->CopyBehaviour(gameBlackboard, enemyBlackboard));
			};
		};
	}
}






#endif
