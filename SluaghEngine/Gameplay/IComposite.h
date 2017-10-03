#ifndef SE_GAMEPLAY_COMPOSITE_H
#define SE_GAMEPLAY_COMPOSITE_H
#include "IBehaviour.h"
#include <vector>

namespace SE
{
	namespace GAMEPLAY
	{
		/**
		*
		* @brief The IComposite class is the interface class that all decorators inherit from.
		*
		* @details Inheritance from the IBehaviour class, the IComposite class is the base class for the Decorator pattern.
		*
		* The decorator keeps a single child behaviour and acts upon it, see derived classes for details.
		*
		* @sa IBehaviour for the basic Behaviour, IDecorator for when a single child should be manipulated.
		*
		**/
		class IComposite : public IBehaviour
		{
		private:
			IComposite() = delete;
		protected:
			typedef std::vector<IBehaviour*> Behaviours;
			Behaviours myChildren;


			Status Update() override = 0;
		public:
			IComposite(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard);
			void AddChild(IBehaviour* childToAdd);
			~IComposite() override;
		};
	}
}


#endif


