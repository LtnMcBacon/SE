#ifndef SE_GAMEPLAY_REPEATER_H
#define SE_GAMEPLAY_REPEATER_H

#include "IDecorator.h"
#include <cstdint>

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The Repeater tick of its child a set number of times.
		* 
		* @warning If the value -1 is passed as repeatLimit, this will be used as a "keep repeating" class.
		*
		* @sa IBehaviour for the basic Behaviour, IDecorator for the class Repeater derives from,
		* RepeatUntilFail for the Repeater that goes on until a fail is encountered.
		*
		**/
		class Repeater : public IDecorator
		{
		private:
			Repeater() = delete;
		protected:

			/**
			* @brief	Initialize the Selector for updating.
			*
			* @details	Reset repeatCounter
			*
			* @retval void No return value
			*
			*
			*/
			void OnInitialization() override;

			/**
			* @brief	Goes through the childs a set ammount of time.
			*
			* @details	Update its child, and continues to do so until a set ammount of ticks has been done.
			* If the child returns BEHAVIOUR_RUNNING, BEHAVIOUR_FAILURE, BEHAVIOUR_INVALID, so will Update.
			* Otherwise, the behaviour will loop until it has looped around to the pre-defined limit.
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
			uint_fast8_t repeatLimit;
			uint_fast8_t repeatCounter;
		public:
			Repeater(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child, uint_fast8_t repeatLimit);
			~Repeater();

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new Repeater(enemyBlackboard, gameBlackboard,
					myChild->CopyBehaviour(gameBlackboard, enemyBlackboard), repeatLimit);
			}
		};
	}
}




#endif
