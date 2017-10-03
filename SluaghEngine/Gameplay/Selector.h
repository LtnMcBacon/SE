#ifndef SE_GAMEPLAY_SELECTOR_H
#define SE_GAMEPLAY_SELECTOR_H

#include "IComposite.h"

namespace SE
{
	namespace GAMEPLAY
	{

		/**
		*
		* @brief A class that selects and check its child in order.
		*
		* @details Inheriting from IComposite, the Selector will tick of the childs in order.
		* If all childs return BEHAVIOUR_FAILURE, so does the Selector.
		*
		* @sa IBehaviour for the basic Behaviour, IComposite for the basic composite class, and RandomSelector for the random form of Selector
		*
		**/
		class Selector : public IComposite
		{
		private:
			Selector() = delete;
		protected:
			Behaviours::iterator currentChild;

			/**
			* @brief	Initialize the Selector for updating.
			*
			* @details	Sets the first child to "current" child.
			*
			* @retval void No return value
			*
			*
			*/
			virtual void OnInitialization() override;

			/**
			* @brief	Goes through the childs in order
			*
			* @details	Goes through the childs in order.
			* If a child returns anything but BEHAVIOUR_FAILURE,
			* Update will return the same value.
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
			Selector(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~Selector();
			
		};
	}
}



#endif
