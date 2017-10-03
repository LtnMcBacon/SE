#ifndef SE_GAMEPLAY_RANDOM_SELECTOR_H
#define SE_GAMEPLAY_RANDOM_SELECTOR_H

#include "IComposite.h"

namespace SE
{
	namespace GAMEPLAY
	{
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
