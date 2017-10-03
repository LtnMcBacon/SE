#ifndef SE_GAMEPLAY_REPEATER_H
#define SE_GAMEPLAY_REPEATER_H

#include "IDecorator.h"

namespace SE
{
	namespace GAMEPLAY
	{
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
