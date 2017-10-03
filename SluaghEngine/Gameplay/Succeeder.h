#ifndef SE_GAMEPLAY_SUCCEEDER_H
#define SE_GAMEPLAY_SUCCEEDER_H

#include "IDecorator.h"

namespace SE
{
	namespace GAMEPLAY
	{
		/**
		*
		* @brief The Succeeder tick of its child and return success.
		*
		* @sa IBehaviour for the basic Behaviour, IDecorator for the class Succeeder derives from.
		*
		**/
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