#ifndef SE_GAMEPLAY_SUCCEEDER_H
#define SE_GAMEPLAY_SUCCEEDER_H

#include "IDecorator.h"

namespace SE
{
	namespace GAMEPLAY
	{
		class Succeeder : public IDecorator
		{
		private:
			Succeeder() = delete;
		protected:
			Status Update() override;

		public:
			Succeeder(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child);
			~Succeeder();
		};

	}
}



#endif