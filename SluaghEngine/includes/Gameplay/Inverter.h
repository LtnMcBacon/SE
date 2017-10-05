#ifndef SE_GAMEPLAY_INVERTER_H
#define SE_GAMEPLAY_INVERTER_H
#include "IDecorator.h"

namespace SE
{
	namespace Gameplay
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

			/**
			* @brief	Update the child and reverse the result
			*
			* @details	If the child returns BEHAVIOUR_FAILURE, the inverter will return
			* BEHAVIOUR_SUCCESS, and vice versa. If another value is returned from the child,
			* Update will return the same value.
			*
			*
			* @retval BEHAVIOUR_FAILURE The behaviour failed to complete.
			* @retval BEHAVIOUR_RUNNING The behaviour is executing, and may take multiple frames to finish.
			* @retval BEHAVIOUR_SUCCESS The behaviour is finished and succeeded in its execution.
			* @retval BEHAVIOUR_SUSPENDED The behaviour is suspended by another node, and should be checked in on later.
			* @retval BEHAVIOUR_INVALID Something is wrong, since this should only be a "default" state.
			*
			*/
			Status Update() override;
		public:
			Inverter(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child);
			~Inverter() override;

			IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new Inverter(enemyBlackboard, gameBlackboard,
					myChild->CopyBehaviour(gameBlackboard, enemyBlackboard));
			}
		};

		
	}
}


#endif
