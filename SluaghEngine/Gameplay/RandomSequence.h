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
			virtual void OnInitialization() override;
			virtual Status Update() override;

		public:
			RandomSequence(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			~RandomSequence();

		};
	}
}

#endif