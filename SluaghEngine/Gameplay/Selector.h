#ifndef SE_GAMEPLAY_SELECTOR_H
#define SE_GAMEPLAY_SELECTOR_H

#include "IComposite.h"

namespace SE
{
	namespace GAMEPLAY
	{
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
