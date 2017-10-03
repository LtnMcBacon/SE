#ifndef SE_GAMEPLAY_DECORATOR_H
#define SE_GAMEPLAY_DECORATOR_H
#include "IBehaviour.h"

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The IDecorator class is the interface class that all decorators inherit from.
		*
		* @details Inheritance from the IBehaviour class, the IDecorator class is the base class for the Decorator pattern.
		* 
		* The decorator keeps a single child behaviour and acts upon it, see derived classes for details.
		* 
		* @sa IBehaviour for the basic Behaviour, IComposite for when more than one child is needed.
		*
		**/
		class IDecorator : public IBehaviour
		{
		private:
			IDecorator() = delete;
		protected:
			IBehaviour* myChild;


			Status Update() override = 0;
		public:
			IDecorator(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child);
			~IDecorator() override;
		};
	}
}


#endif

