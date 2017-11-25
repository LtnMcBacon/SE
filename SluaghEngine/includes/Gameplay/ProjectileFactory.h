#ifndef _SE_GAMEPLAY_PROJECTILE_FACTORY_H_
#define _SE_GAMEPLAY_PROJECTILE_FACTORY_H_

#include "GameUnit.h"
#include <Gameplay\Projectile.h>
#include <Gameplay\ProjectileData.h>
#include <ResourceHandler\IResourceHandler.h>
#include "Utilz/GUID.h"

#include <variant>
#include <unordered_map>
#include <map>
#include <string>

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

		class Room;
		class PlayerUnit;
		class ProjectileManager;

		struct BehaviourPointers
		{
			Room** currentRoom;
			PlayerUnit* player;
			ProjectileManager* pManager;
		};

		class ProjectileFactory
		{
		private:

			/**
			* @brief	Wrapper for a parameter of a behaviour, uses variant to be able to hold whatever type might be needed
			*/
			struct BehaviourParameter
			{
				std::variant<float, int, bool, std::string, std::vector<std::function<bool(Projectile* projectile, float dt)>>, Projectile*, std::weak_ptr<GameUnit*>> data;
			};

			struct LoadedProjectile
			{
				float projectileWidth, projectileHeight, rotationAroundUnit, distanceFromUnit, projectileRotation, rotationPerSec, projectileSpeed, timeToLive;
				bool boundToOwner;
				Utilz::GUID meshName;
				float meshScale;
				Utilz::GUID particleEffect;
				Utilz::GUID materialName;
				Utilz::GUID soundName;
				int nrOfBehaviours;
				std::vector<std::string> behaviours;
			};

			BehaviourPointers ptrs;

			uint64_t projectileGeneration;

			std::vector<std::function<std::function<bool(Projectile* projectile, float dt)>(std::vector<BehaviourParameter> parameter)>> behaviourFunctions;

			std::vector<Projectile> newProjectiles;

			std::map<Utilz::GUID, std::vector<LoadedProjectile>, Utilz::GUID::Compare> loadedProjectiles;

		public:

			/**
			* @brief	Creates a new projectile based on the projectile data and data read from a file created by an editor
			*
			* @param [in] data Data created by the unit from which the need for the projectile originates
			*
			* @retval Projectile The newly created projectile
			*
			*/
			void CreateNewProjectile(const ProjectileData& data);

			inline void GetNewProjectiles(std::vector<Projectile>& vectorToAddTo)
			{
				vectorToAddTo.insert(vectorToAddTo.end(), newProjectiles.begin(), newProjectiles.end());
				newProjectiles.clear();
			}


		private:
			ProjectileFactory(const ProjectileFactory& other) = delete;
			ProjectileFactory(const ProjectileFactory&& other) = delete;
			ProjectileFactory& operator=(const ProjectileFactory& rhs) = delete;

			void GetLine(const std::string& file, std::string& line, int& pos);
			void LoadNewProjectiles(const ProjectileData& data);
			std::function<bool(Projectile* projectile, float dt)> ParseBehaviour(Projectile& p, std::weak_ptr<GameUnit*> ownerUnit, const char* fileData);
			void ParseValue(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter>& parameters, const char* valueData);

			
			/**
			* @brief Acts as an if case
			*/
			std::function<bool(Projectile* projectile, float dt)> IFCaseBehaviour(std::vector<BehaviourParameter> parameters/*vector Conditions, vector ifTrue, vector ifFalse*/);

			/**
			* @brief Acts as an AND checker to be used with IfCaseBehaviour
			*/
			std::function<bool(Projectile* projectile, float dt)> ANDConditionBehaviour(std::vector<BehaviourParameter> parameters/*vector arguments*/);

			/**
			* @brief Acts as an OR checker to be used with IfCaseBehaviour
			*/
			std::function<bool(Projectile* projectile, float dt)> ORConditionBehaviour(std::vector<BehaviourParameter> parameters/*vector arguments*/);
			
			/**
			* @brief	Adds bounce behaviour to the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> BounceBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds speed changing behaviour to the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> SpeedAddDynamicBehaviour(std::vector<BehaviourParameter> parameters/*float speedModifier*/);

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
			std::function<bool(Projectile* projectile, float dt)> LifeTimeAddStaticBehaviour(std::vector<BehaviourParameter> parameters/*float timeToIncrease*/);

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
			* @brief	Adds time condition behaviour to the projectile so that other behaviours are run once after the time is up
			*/
			std::function<bool(Projectile* projectile, float dt)> TimeConditionBehaviour(std::vector<BehaviourParameter> parameters/*float delay, int timesToRepeat, Projectile* projectile*/);
			
			/**
			 * @brief	Adds a behaviour for projectiles to follow the player
			 */
			std::function<bool(Projectile* projectile, float dt)> TargetPlayerBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters);

			/**
			 * @brief	Adds a condition to check line of sight between owner and the projectile
			 */
			std::function<bool(Projectile* projectile, float dt)> LineOfSightConditionBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds a condition to check line of sight between owner and the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> LockToPlayerBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds a condition to check line of sight between owner and the projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> KillSelfBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds collide condition behaviour to the projectile so that other behaviours are run once the projectile collides with something
			*/
			std::function<bool(Projectile* projectile, float dt)> CollidedConditionBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds alive condition behaviour to the projectile so that other behaviours are run if the projectile is alive
			*/
			std::function<bool(Projectile* projectile, float dt)> IsAliveConditionBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds distance to closest enemy condition behaviour to the projectile so that other behaviours are run once the projectile is close enough to an enemy
			*/
			std::function<bool(Projectile* projectile, float dt)> CloseToEnemyConditionBehaviour(std::vector<BehaviourParameter> parameters/*float minDistance*/);

			/**
			* @brief	Adds distance to player condition behaviour to the projectile so that other behaviours are run once the projectile is close enough to the player
			*/
			std::function<bool(Projectile* projectile, float dt)> CloseToPlayerConditionBehaviour(std::vector<BehaviourParameter> parameters/*float minDistance*/);

			/**
			* @brief	Adds projectile creation behaviour to the projectile so that the projectile creates new projectiles
			*/
			std::function<bool(Projectile* projectile, float dt)> CreateProjectilesBehaviour(std::vector<BehaviourParameter> parameters/*string fileName*/);

			/**
			* @brief	Adds invert condition behaviour to the projectile that inverts the given behaviour
			*/
			std::function<bool(Projectile* projectile, float dt)> InverterBehaviour(std::vector<BehaviourParameter> parameters/*vector arguments*/);

			/**
			* @brief	Adds a behaviour that kills all projectiles of a certain generation
			*/
			std::function<bool(Projectile* projectile, float dt)> KillGenerationBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds a behaviour to set a projectile as alive
			*/
			std::function<bool(Projectile* projectile, float dt)> SetAliveBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds a condition to see if a projectile collided with an enemy
			*/
			std::function<bool(Projectile* projectile, float dt)> CollidedWithEnemyConditionBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds a condition to see if a projectile collided with an object
			*/
			std::function<bool(Projectile* projectile, float dt)> CollidedWithObjectConditionBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds a condition to see if a projectile collided with the player
			*/
			std::function<bool(Projectile* projectile, float dt)> CollidedWithPlayerConditionBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds a condition to see if a projectile collided with the player
			*/
			std::function<bool(Projectile* projectile, float dt)> SetDamageBasedOnDTBehaviour(std::vector<BehaviourParameter> parameters);

			/**
			* @brief	Adds a condition to see if a projectiles owner's health is above a certain amount
			*/
			std::function<bool(Projectile* projectile, float dt)> UserHealthAboveConditionBehaviour(std::vector<BehaviourParameter> parameters); // f, o

			/**
			* @brief	Adds a behaviour to set the actual damage of a projectile
			*/
			std::function<bool(Projectile* projectile, float dt)> SetActualDamageBehaviour(std::vector<BehaviourParameter> parameters); // f

			/**
			 * @brief	Adds a behaviour to create particles between the projectile and the owner
			 */
			std::function<bool(Projectile* projectile, float dt)> CreateParticlesBetweenProjectileAndOwnerBehaviour(std::vector<BehaviourParameter> parameters); // o

			/**
			* @brief	Adds a behaviour to knockback hit enemies
			* */
			std::function<bool(Projectile* projectile, float dt)> KnockbackBehaviour(std::vector<BehaviourParameter> parameters); // f

			/**
			* @brief	Adds a condition based on the distance between the projectile and its owner
			* */
			std::function<bool(Projectile* projectile, float dt)> RangeToOwnerConditionBehaviour(std::vector<BehaviourParameter> parameters); // o, f

			/**
			* @brief	Adds a condition based on if the owner is alive or not
			**/
			std::function<bool(Projectile* projectile, float dt)> OwnerIsAliveConditionBehaviour(std::vector<BehaviourParameter> parameters); // o

			/**
			* @brief	Adds a behaviour to set the original amount of damage in a projectile
			**/
			std::function<bool(Projectile* projectile, float dt)> SetOriginalDamageBehaviour(std::vector<BehaviourParameter> parameters); // f

			/**
			* @brief	Adds a behaviour to set the original amount of healing a projectile does
			**/
			std::function<bool(Projectile* projectile, float dt)> SetOriginalHealingBehaviour(std::vector<BehaviourParameter> parameters); // f

			/**
			* @brief	Adds a behaviour to set what type of unit a projectile can hit (p = player, e = enemy, anything else will be set to everyone)
			**/
			std::function<bool(Projectile* projectile, float dt)> SetValidTargetBehaviour(std::vector<BehaviourParameter> parameters); // s

			/**
			* @brief	Adds a decal to the projectile
			**/
			std::function<bool(Projectile* projectile, float dt)> CreateDecalBehaviour(std::vector<BehaviourParameter> parameters); // s, f, p

			/**
			* @brief	Adds a behaviour to change the opacity of the decal
			**/
			std::function<bool(Projectile* projectile, float dt)> SetDecalOpacityBehaviour(std::vector<BehaviourParameter> parameters); // f

			/**
			* @brief	Adds a behaviour to change the opacity of the decal per second
			**/
			std::function<bool(Projectile* projectile, float dt)> ModifyDecalOpacityBehaviour(std::vector<BehaviourParameter> parameters); // f



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
