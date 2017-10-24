#ifndef _SE_GAMEPLAY_PROJECTILE_H_
#define _SE_GAMEPLAY_PROJECTILE_H_

#include "GameUnit.h"
#include <functional>

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

		enum class CollisionType
		{
			NONE,
			OBJECT,
			ENEMY
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
			float force = 0;
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

			float radius;
		};

		struct CollisionData
		{
			float xVec = 0.0f;
			float yVec = 0.0f;
			CollisionType type = CollisionType::NONE;
		};

		struct BehaviourData
		{
			union
			{
				float f;
				int i;
			};
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

			inline void SetCollisionData(CollisionData data)
			{
				collisionData = data;
			};

			inline BoundingRect GetBoundingRect()
			{
				return rect;
			}

			inline float GetExtentX()
			{
				return extentX;
			}

			inline float GetExtentY()
			{
				return extentY;
			}

			inline DamageEvent GetProjectileDamageEvent()
			{
				return eventDamage;
			}

			inline HealingEvent GetProjectileHealingEvent()
			{
				return eventHealing;
			}

			inline ConditionEvent GetProjectileConditionEvent()
			{
				return eventCondition;
			}

		private:

			std::vector<std::function<bool(Projectile* projectile, float dt)>> functionsToRun;
			std::vector<std::function<bool(Projectile* projectile, float dt)>> tempStorage;

			std::vector<std::function<bool(Projectile* projectile, float dt)>> onCollision;
			std::vector<std::function<bool(Projectile* projectile, float dt)>> onDeath;

			std::vector<BehaviourData> behaviourData;

			float extentX;
			float extentY;

			Rotation rotData;
			float rotation;

			float speed;
			float lifeTime;
			bool active = true;

			ValidTarget target; // what type of unit the enemy can hit

			DamageEvent eventDamage; // event to be transfered on hit
			HealingEvent eventHealing; // event to be transfered on hit
			ConditionEvent eventCondition; // event to be transfered on hit

			CollisionData collisionData;

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
			Projectile(const Projectile& other);
			Projectile& operator=(const Projectile& other);
			Projectile(Projectile&& other);
			~Projectile();

			void AddContinuousFunction(const std::function<bool(Projectile* projectile, float dt)>& func);
			void AddCollisionFunction(const std::function<bool(Projectile* projectile, float dt)>& func);
			void AddDeathFunction(const std::function<bool(Projectile* projectile, float dt)>& func);

			int AddBehaviourData(BehaviourData data);
			BehaviourData& GetBehaviourData(int index);

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

			inline void SetActive(bool value)
			{
				active = value;
			}

			inline void SetLifeTime(float value)
			{
				lifeTime = value;
			}



			inline bool GetActive()
			{
				return active;
			};

			inline float GetCollisionVectorX()
			{
				return collisionData.xVec;
			}

			inline float GetCollisionVectorY()
			{
				return collisionData.yVec;
			}

			inline CollisionType GetCollisionType()
			{
				return collisionData.type;
			}

			inline Rotation GetRotationStyle()
			{
				return rotData;
			}

			inline float GetLifeTime()
			{
				return lifeTime;
			}

			inline float GetSpeed()
			{
				return speed;
			}

		};

	}
}




#endif
