#ifndef _SE_GAMEPLAY_PROJECTILE_H_
#define _SE_GAMEPLAY_PROJECTILE_H_

#include "GameUnit.h"


namespace SE
{
	namespace Gameplay
	{

		struct ProjectileData;
		/**
		*
		* @brief The projectile class
		*
		* @details This class contains the update functions and logic for the projectiles in Sluagh.
		* It inherits from GameUnit.
		*
		* @sa GameUnit
		*
		**/

		enum class ValidTarget
		{
			EVERYONE,
			ENEMIES,
			PLAYER
		};

		enum class RotationStyle
		{
			NONE,
			SELF,
			PLAYER
		};

		struct Rotation
		{
			RotationStyle style = RotationStyle::NONE;
			float force;
		};

		//stefan approves
		struct BoundingRect
		{
			float upperLeftX;
			float upperLeftY;

			float upperRightX;
			float upperRightY;

			float lowerLeftX;
			float lowerLeftY;

			float lowerRightX;
			float lowerRightY;
		};

		class Projectile : public GameUnit
		{
		public:

			/**
			* @brief	Update the players movement
			*
			* @details	This function updates the position of the player and checks so that it is a legal position,
			* if not it tries to retain as much of the movement as possible
			*
			* @param [in] dt Delta time for this frame
			*
			* @retval void No value
			*
			*/
			void UpdateMovement(float dt);

			/**
			* @brief	Update the players movement
			*
			* @details	This function updates the players actions and adds new projectiles to the game, skills should be checked for use in here
			* and perks then check new events that are created before they are deployed
			*
			* @param [in] dt Delta time for this frame
			* @param [out] newProjectiles Data to create new projectiles from
			*
			* @retval void No value
			*
			*/
			void UpdateActions(float dt);

			inline void SetCollisionVector(float x, float y)
			{
				collisionVecX = x;
				collisionVecY = y;
			};

			inline bool GetAlive()
			{
				return alive;
			}
			
			inline BoundingRect GetBoundingRect()
			{
				return rect;
			}

		private:
			//Projectile(const Projectile& other) = delete;
			//Projectile(const Projectile&& other) = delete;
			Projectile& operator=(const Projectile& rhs) = delete;

			std::vector<std::function<bool(Projectile* projectile, float dt)>> functionsToRun;

			std::vector<std::function<bool(Projectile* projectile, float dt)>> onCollision;
			std::vector<std::function<bool(Projectile* projectile, float dt)>> onDeath;

			float extentX;
			float extentY;

			Rotation rotData;
			float rotation;

			float speed;
			float lifeTime;
			bool alive = true;

			ValidTarget target; // what type of unit the enemy can hit

			DamageEvent eventDamage; // event to be transfered on hit
			HealingEvent eventHealing; // event to be transfered on hit
			ConditionEvent eventCondition; // event to be transfered on hit

			float collisionVecX = 0.0f;
			float collisionVecY = 0.0f;

			BoundingRect rect;

			/**
			* @brief	Helper function for rotating corners of the bounding data
			*/
			void UpdateBounding();

			/**
			* @brief	Helper function for rotating corners of the bounding data
			*/
			void RotatePoint(float &xCoord, float &yCoord);

		public:
			Projectile();
			Projectile(ProjectileData data, Rotation rot, float projectileSpeed, float projectileLifeTime, ValidTarget projectileTarget, DamageEvent eventD, HealingEvent eventH, ConditionEvent eventC);
			~Projectile();

			void AddContinuousFunction(std::function<bool(Projectile* projectile, float dt)> func);
			void AddCollisionFunction(std::function<bool(Projectile* projectile, float dt)> func);
			void AddDeathFunction(std::function<bool(Projectile* projectile, float dt)> func);

			inline void SetRotation(float projectileRotation)
			{
				rotation = projectileRotation;
			};

			inline void SetRotationStyle(Rotation rotationStyle)
			{
				rotData = rotationStyle;
			};

			inline void SetSpeed(float projectileSpeed)
			{
				speed = projectileSpeed;
			};

			inline void SetValidTarget(ValidTarget projectileTarget)
			{
				target = projectileTarget;
			};

			inline void SetDamageEvent(DamageEvent eventD)
			{
				eventDamage = eventD;
			};

			inline void SetHealingEvent(HealingEvent eventH)
			{
				eventHealing = eventH;
			};

			inline void SetConditionEvent(ConditionEvent eventC)
			{
				eventCondition = eventC;
			};

		};

	}
}




#endif
