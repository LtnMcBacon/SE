#ifndef _SE_GAMEPLAY_PROJECTILE_FACTORY_H_
#define _SE_GAMEPLAY_PROJECTILE_FACTORY_H_

#include "GameUnit.h"
#include <Gameplay\Projectile.h>
#include <Gameplay\ProjectileData.h>

namespace SE
{
	namespace Gameplay
	{

		/**
		*
		* @brief The class that holds and manages projectiles
		*
		* @details This class holds and updates all the projectiles in the game
		*
		**/

		class ProjectileFactory
		{
		public:

			/**
			* @brief	Creates a new projectile based on the projectile data and data read from a file created by an editor
			*
			* @param [in] data Data created by the unit from which the need for the projectile originates
			*
			* @retval Projectile The newly created projectile
			*
			*/
			SE::Gameplay::Projectile CreateNewProjectile(ProjectileData data);


		private:
			ProjectileFactory(const ProjectileFactory& other) = delete;
			ProjectileFactory(const ProjectileFactory&& other) = delete;
			ProjectileFactory& operator=(const ProjectileFactory& rhs) = delete;


		public:

			ProjectileFactory();
			~ProjectileFactory();

		};

	}
}




#endif
