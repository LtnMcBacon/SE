#ifndef SE_GAMEPLAY_REPEATER_H
#define SE_GAMEPLAY_REPEATER_H

#include "IDecorator.h"
#include <cstdint>

namespace SE
{
	namespace GAMEPLAY
	{
		/**
		*
		* @brief The Repeater tick of its child a set number of times.
		*
		* @sa IBehaviour for the basic Behaviour, IDecorator for the class Repeater derives from,
		* RepeatUntilFail for the Repeater that goes on until a fail is encountered.
		*
		**/
		class Repeater : public IDecorator
		{
		private:
			Repeater() = delete;
		protected:
			void OnInitialization() override;
			Status Update() override;
			uint_fast8_t repeatLimit;
			uint_fast8_t repeatCounter;
		public:
			Repeater(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child, uint_fast8_t repeatLimit);
			~Repeater();
		};
	}
}




#endif
