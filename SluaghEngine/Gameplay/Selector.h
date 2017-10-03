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
			virtual void OnInitialization() override;
			virtual Status Update() override;

		public:
			Selector(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~Selector();
			
		};
	}
}



#endif
