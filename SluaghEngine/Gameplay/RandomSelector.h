#ifndef SE_GAMEPLAY_RANDOM_SELECTOR_H
#define SE_GAMEPLAY_RANDOM_SELECTOR_H

#include "IComposite.h"

namespace SE
{
	namespace GAMEPLAY
	{
		/**
		*
		* @brief A class that selects and check its child in a random order.
		*
		* @details Inheriting from IComposite, the RandomSelector will shuffle the order the childs are ticked off.
		* If all childs return BEHAVIOUR_FAILURE, so does the RandomSelector.
		*
		* @sa IBehaviour for the basic Behaviour, IComposite for the basic composite class, and Selector for the non-random form of Selector
		*
		**/
		class RandomSelector : public IComposite
		{
			
			private:
				RandomSelector() = delete;
			protected:
				Behaviours::iterator currentChild;
				virtual void OnInitialization() override;
				virtual Status Update() override;

			public:
				RandomSelector(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
				~RandomSelector();

		};
	}
}


#endif
