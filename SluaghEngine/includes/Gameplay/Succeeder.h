#ifndef SE_GAMEPLAY_SUCCEEDER_H
#define SE_GAMEPLAY_SUCCEEDER_H

#include "IDecorator.h"

namespace SE
{
	namespace Gameplay
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

			/**
			* @brief	Tick of the child and return BEHAVIOUR_SUCCESS
			*
			* @retval BEHAVIOUR_SUCCESS The behaviour is finished and succeeded in its execution.
			*/
			Status Update() override;

		public:
			Succeeder(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child);
			~Succeeder();

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{

				return new Succeeder(enemyBlackboard, gameBlackboard, 
					myChild->CopyBehaviour(gameBlackboard,	enemyBlackboard));
			}

			size_t SizeOfBehaviour() const override;
		};

	}
}



#endif