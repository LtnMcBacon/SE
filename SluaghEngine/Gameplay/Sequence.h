#ifndef SE_GAMEPLAY_SEQUENCE_H
#define SE_GAMEPLAY_SEQUENCE_H

#include "IComposite.h"

namespace SE
{
	namespace GAMEPLAY
	{
		/**
		*
		* @brief A class that check its child in order.
		*
		* @details Inheriting from IComposite, the Sequence will tick of the childs in order.
		* If all childs return BEHAVIOUR_SUCCESS, so does the Selector.
		*
		* @sa IBehaviour for the basic Behaviour, IComposite for the basic composite class, and RandomSequence for the random form of Sequence
		*
		**/
		class Sequence : public IComposite
		{
		private:
			Sequence() = delete;
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