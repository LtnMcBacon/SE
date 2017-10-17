#ifndef _SE_GAMEPLAY_PROJECTILE_FACTORY_H_
#define _SE_GAMEPLAY_PROJECTILE_FACTORY_H_

#include "GameUnit.h"
#include <Gameplay\Projectile.h>
#include <Gameplay\ProjectileData.h>
#include <Gameplay\Room.h>
#include <Gameplay\PlayerUnit.h>

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

		struct BehaviourPointers
		{
			Room** currentRoom;
			PlayerUnit* player;
		};

		class ProjectileFactory
		{
		private:

			struct BehaviourParameter
			{
				union BehaviourUnion
				{
					BehaviourUnion()
					{
						
					}
					~BehaviourUnion()
					{
						
					}
					float f;
					int i;
					bool b;
					//std::function<bool(Projectile* projectile, float dt)> func;
					Projectile* projectile;
					std::weak_ptr<GameUnit*>* projectileOwner;


				}behaviour;
				BehaviourParameter() {};
				~BehaviourParameter() {};
			};

			/**
			* @brief	Enum over what a function should be added to (continuous, on collision, on death, etc)
			*/
			enum class TypeOfFunction
			{
				CONTINUOUS,
				ON_COLLISION,
				ON_DEATH
			};

			BehaviourPointers ptrs;

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

			/**
			* @brief	Adds bounce behaviour to the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> BounceBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds speed changing behaviour to the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> SpeedModifierBehaviour(std::vector<BehaviourParameter> parameters/*float speedModifier*/);

			/**
			* @brief	Adds rotation changing behaviour to the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> RotationModifierBehaviour(std::vector<BehaviourParameter> parameters/*float rotationModifier*/);

			/**
			* @brief	Adds rotation invertion behaviour to the projectile (acts weird, not fully functioning)
			*/
			std::function<bool(Projectile* projectile, float dt)> RotationInvertionBehaviour(std::vector<BehaviourParameter> parameters/*float intervall, Projectile* projectile*/);

			/**
			* @brief	Adds lifetime behaviour to the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> LifeTimeBehaviour(std::vector<BehaviourParameter> parameters/*float timeToIncrease*/);

			/**
			* @brief	Adds Targeting closest enemy behaviour to the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> TargetClosestEnemyBehaviour(std::vector<BehaviourParameter> parameters/*float rotPerSecond*/);

			/**
			* @brief Add stun ownerUnit behaviour to the projectile
			*/
			std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> LifeStealBehaviour(
				std::vector<BehaviourParameter> parameters
			);

			/**
			 * @brief Add stun ownerUnit behaviour to the projectile
			 */
			std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> StunOwnerUnitBehaviour(
				std::vector<BehaviourParameter> parameters
			);

			/**
			* @brief	Adds Targeting closest enemy behaviour to the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> TimeCondition(std::vector<BehaviourParameter> parameters/*float delay, std::function<bool(Projectile* projectile, float dt)> func*/);
			
			/**
			* @brief	Helper function for adding the behaviour to the correct function vector of the projectile
			*/
			void AddBehaviourToProjectile(Projectile& p, TypeOfFunction type, std::function<bool(Projectile* projectile, float dt)> func);


		public:

			ProjectileFactory();
			~ProjectileFactory();

			inline void SetBehaviourPtrs(BehaviourPointers bPtrs)
			{
				ptrs = bPtrs;
			};

		};

	}
}




#endif
