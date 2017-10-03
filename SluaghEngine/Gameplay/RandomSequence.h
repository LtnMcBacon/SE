#ifndef SE_GAMEPLAY_RANDOM_SEQUENCE_H
#define SE_GAMEPLAY_RANDOM_SEQUENCE_H

#include "IComposite.h"

namespace SE
{
	namespace GAMEPLAY
	{
		class RandomSequence : public IComposite
		{
		private:
			RandomSequence() = delete;
		protected:
			Behaviours::iterator currentChild;
			virtual void OnInitialization() override;
			virtual Status Update() override;

		public:
			RandomSequence(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~RandomSequence();

		};
	}
}

#endif