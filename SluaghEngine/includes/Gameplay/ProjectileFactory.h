#ifndef _SE_GAMEPLAY_PROJECTILE_FACTORY_H_
#define _SE_GAMEPLAY_PROJECTILE_FACTORY_H_

#include "GameUnit.h"
#include <Gameplay\Projectile.h>
#include <Gameplay\ProjectileData.h>
#include <Gameplay\Room.h>
#include <Gameplay\PlayerUnit.h>
#include <ResourceHandler\IResourceHandler.h>
#include "Utilz/GUID.h"


#include <variant>

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

			/**
			* @brief	Wrapper for a parameter of a behaviour, uses variant to be able to hold whatever type might be needed
			*/
			struct BehaviourParameter
			{
				std::variant<float, int, bool, std::vector<std::function<bool(Projectile* projectile, float dt)>>, Projectile*> data;
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

			std::vector<std::function<std::function<bool(Projectile* projectile, float dt)>(std::vector<BehaviourParameter> parameter)>> behaviourFunctions;

			std::vector<Projectile> newProjectiles;

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
			std::function<bool(Projectile* projectile, float dt)> ParseBehaviour(Projectile& p, const char* fileData);
			void ParseValue(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter>& parameters, const char* valueData);

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
			* @brief	Adds time condition behaviour to the projectile so that other behaviours are run once after the time is up
			*/
			std::function<bool(Projectile* projectile, float dt)> TimeConditionRunBehaviour(std::vector<BehaviourParameter> parameters/*float delay, std::function<bool(Projectile* projectile, float dt)> func, Projectile* projectile*/);

			/**
			* @brief	Adds time condition behaviour to the projectile so that other behaviours are run once after the time is up
			*/
			std::function<bool(Projectile* projectile, float dt)> TimeConditionAddBehaviour(std::vector<BehaviourParameter> parameters/*float delay, std::function<bool(Projectile* projectile, float dt)> func, Projectile* projectile*/);



			/**
			* @brief	Helper function for adding the behaviour to the correct function vector of the projectile
			*/
			void AddBehaviourToProjectile(Projectile& p, TypeOfFunction type, const std::function<bool(Projectile* projectile, float dt)>& func);


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
