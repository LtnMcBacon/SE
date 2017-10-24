#ifndef _SE_GAMEPLAY_PROJECTILE_MANAGER_H_
#define _SE_GAMEPLAY_PROJECTILE_MANAGER_H_

#include "GameUnit.h"
#include <Gameplay\Projectile.h>
#include <Gameplay\ProjectileFactory.h>

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

		class ProjectileManager
		{
		public:

			/**
			* @brief	Update the projectiles movement
			*
			* @param [in] dt Delta time for this frame
			*
			* @retval void No value
			*
			*/
			void UpdateProjectilePositions(float dt);

			/**
			* @brief	Update the projectiles actions
			*
			* @param [in] dt Delta time for this frame
			*
			* @retval void No value
			*
			*/
			void UpdateProjectileActions(float dt);

			/**
			* @brief	Adds new projectiles to the manager
			*
			* @param [in] newProjectiles Vector containing information needed to create new projectiles
			*
			* @retval void No value
			*
			*/
			void AddProjectiles(std::vector<ProjectileData>& newProjectiles);

			/**
			 * @brief Check collision between the target and all projectiles. If a collision occours,
			 * the projectile's data will be copied to the collision object, before the projectile get's an 
			 * "On collision" signal. unitTarget is whether the unit is a "Player", "Enemy", or something else.
			 */
			void CheckCollisionBetweenUnitAndProjectiles(GameUnit* unit, ValidTarget unitTarget);

		private:
			ProjectileManager(const ProjectileManager& other) = delete;
			ProjectileManager(const ProjectileManager&& other) = delete;
			ProjectileManager& operator=(const ProjectileManager& rhs) = delete;

			std::vector<Projectile> projectiles;

			ProjectileFactory factory;
			int nrOfProjectiles = 0;

			/**
			 * @brief HelperFunction for collision checks
			 */
			bool CheckCollisionHelperFunction(GameUnit* unit, BoundingRect projectileRect);

		public:

			ProjectileManager(BehaviourPointers bPtrs);
			~ProjectileManager();

			inline std::vector<Projectile>& GetAllProjectiles()
			{
				return projectiles;
			};
		};

	}
}




#endif
