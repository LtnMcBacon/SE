#ifndef SE_GAMEPLAY_RANDOM_SEQUENCE_H
#define SE_GAMEPLAY_RANDOM_SEQUENCE_H

#include "IComposite.h"

namespace SE
{
	namespace GAMEPLAY
	{

		/**
		*
		* @brief A class that check its child in a random order.
		*
		* @details Inheriting from IComposite, the RandomSequence will tick of the childs in a random order.
		* If all childs return BEHAVIOUR_SUCCESS, so does the Selector.
		*
		* @sa IBehaviour for the basic Behaviour, IComposite for the basic composite class, and Sequence for the non-random form of Sequence
		*
		**/
		class RandomSequence : public IComposite
		{
		private:
			RandomSequence() = delete;
		protected:
			Behaviours::iterator currentChild;

			/**
			* @brief	Initialize the Random Sequence for updating.
			*
			* @details	Shuffle the child vector, and sets the first child to "current" child.
			*
			* @retval void No return value
			*
			*/
			virtual void OnInitialization() override;

			/**
			* @brief	Goes through the childs in a random order
			*
			* @details	Goes through the childs in random order.
			* If a child returns anything but BEHAVIOUR_SUCCESS,
			* Update will return the same value. If all children returns
			* BEHAVIOUR_SUCCESS, so will Update.
			*
			*
			* @retval BEHAVIOUR_FAILURE The behaviour failed to complete.
			* @retval BEHAVIOUR_RUNNING The behaviour is executing, and may take multiple frames to finish.
			* @retval BEHAVIOUR_SUCCESS The behaviour is finished and succeeded in its execution.
			* @retval BEHAVIOUR_SUSPENDED The behaviour is suspended by another node, and should be checked in on later.
			* @retval BEHAVIOUR_INVALID Something is wrong, since this should only be a "default" state.
			*
			*/
			virtual Status Update() override;

		public:
			RandomSequence(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~RandomSequence();

		};
	}
}

#endif