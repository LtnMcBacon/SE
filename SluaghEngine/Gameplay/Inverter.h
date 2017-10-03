#ifndef SE_GAMEPLAY_INVERTER_H
#define SE_GAMEPLAY_INVERTER_H
#include "IDecorator.h"

namespace SE
{
	namespace GAMEPLAY
	{
		/**
		*
		* @brief The Inverter tick of its child and reverse its return value. Inherits from IDecorator.
		*
		* @sa IBehaviour for the basic Behaviour, IComposite for when more than one child is needed.
		*
		**/
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
