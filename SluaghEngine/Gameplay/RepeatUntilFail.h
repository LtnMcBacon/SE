#ifndef SE_GAMEPLAY_REPEATER_H
#define SE_GAMEPLAY_REPEATER_H

#include "IDecorator.h"

namespace SE
{
	namespace GAMEPLAY
	{
		/**
		*
		* @brief The RepeatUntilFail tick of its child until a failure is returned.
		*
		* @sa IBehaviour for the basic Behaviour, IDecorator for the class RepeatUntilFail derives from,
		* Repeater for the Repeater that goes on a set number of times.
		*
		**/
		class RepeatUntilFail : public IDecorator
		{
		private:
			RepeatUntilFail() = delete;
		protected:
			Status Update() override;
		public:
			RepeatUntilFail(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child);
			~RepeatUntilFail();
		};
	}
}




#endif
