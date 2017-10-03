#ifndef SE_GAMEPLAY_INVERTER_H
#define SE_GAMEPLAY_INVERTER_H
#include "IDecorator.h"

namespace SE
{
	namespace GAMEPLAY
	{
		class Inverter : public IDecorator
		{
		private:
			Inverter() = delete;
		protected:
			Status Update() override;
		public:
			Inverter(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child);
			~Inverter() override;
		};

		
	}
}


#endif
