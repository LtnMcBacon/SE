#ifndef SE_GAMEPLAY_SEQUENCE_H
#define SE_GAMEPLAY_SEQUENCE_H

#include "IComposite.h"

namespace SE
{
	namespace GAMEPLAY
	{
		class Sequence : public IComposite
		{
		private:
			Sequence();
		protected:
			Behaviours::iterator currentChild;
			virtual void OnInitialization() override;
			virtual Status Update() override;

		public:
			Sequence(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~Sequence();

		};
	}
}




#endif