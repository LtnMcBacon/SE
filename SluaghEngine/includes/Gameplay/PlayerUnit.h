#ifndef _SE_GAMEPLAY_PLAYER_UNIT_H_
#define _SE_GAMEPLAY_PLAYER_UNIT_H_

#include "GameUnit.h"
#include <Gameplay\SkillFactory.h>
#include <Utilz\GUID.h>
#include <Gameplay\Skill.h>
#include <map>
#include "Core/IAnimationManager.h"

namespace SE
{
	namespace Gameplay
	{
		struct ProjectileData;

		/**
		*
		* @brief The player class
		*
		* @details This class contains the update functions and logic for the player in Sluagh.
		* It inherits from GameUnit.
		*
		* @warning Only a simple version at the moment
		*
		* @sa GameUnit
		*
		**/
		class PlayerUnit : public GameUnit
		{
		private:

			enum AvailableAnimations
			{
				PLAYER_IDLE_ANIMATION,
				PLAYER_MOVE_ANIMATION,
				PLAYER_ATTACK_ANIMATION,
				PLAYER_HIT_ANIMATION,
				PLAYER_DEATH_ANIMATION
			};
			AvailableAnimations currentAnimation = PLAYER_IDLE_ANIMATION;
			std::map<AvailableAnimations, Core::IAnimationManager::AnimationPlayInfo> animationPlayInfos;

			void InitializeAnimationInfo();

			void AnimationUpdate(AvailableAnimations animationToRun);
			void BlendToAnimation(AvailableAnimations animationToRun);

			/**
			* @brief	Resolve the events that has been added this frame.
			*
			* @details	Go through all the events that has been added to the
			* PlayerUnit this frame.
			*
			* @retval void No return value
			*
			* @warning Not implemented at all yet! Will be called from the "Update" function
			*
			*/
			void ResolveEvents();

			/**
			* @brief	Hinder collision during movement
			*
			* @details	This function checks if the units new position after a move will cause a collision.
			* If a move will cause a collision in x- or y-direction, then that direction-component will be nulled
			* before further computation is done.
			*
			* @param [in] dt Delta time for this frame
			* @param [in/out] xMov The x-component of the direction
			* @param [in/out] yMov The y-component of the direction
			*
			* @retval True Collision occoured
			* @retval False Collision didn't occour
			*
			*/
			bool CorrectCollision(float dt, float &xMov, float &yMov);

		public:

			struct MovementInput
			{
				bool upButton;
				bool leftButton;
				bool downButton;
				bool rightButton;
				
				bool mouseRightDown;
				float mousePosX;
				float mousePosY;

				MovementInput()
				{
					
				}

				MovementInput(bool up, bool left, bool down, bool right, bool mouseRD, float mouseX, float mouseY)
				{
					upButton = up;
					leftButton = left;
					downButton = down;
					rightButton = right;

					mouseRightDown = mouseRD;
					mousePosX = mouseX;
					mousePosY = mouseY;
				}
			};

			/**
			* @brief	Update the players movement
			*
			* @details	This function updates the position of the player and checks so that it is a legal position,
			* if not it tries to retain as much of the movement as possible
			*
			* @param [in] dt Delta time for this frame
			* @param [in] MovementInput input data
			*
			* @retval void No value
			*
			*/
			void UpdateMovement(float dt, const MovementInput& inputs);

			struct ActionInput
			{
				bool actionButton;
				bool skill1Button;
				bool skill2Button;

				ActionInput()
				{

				}

				ActionInput(bool skill1 = false, bool skill2 = false, bool action = false)
				{
					skill1Button = skill1;
					skill2Button = skill2;
					actionButton = action;
				}
			};

			/**
			* @brief	Update the players actions
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
			void UpdateActions(float dt, std::vector<ProjectileData>& newProjectiles, const ActionInput& input);

			/**
			* @brief To be documented
			*/
			void AddForce(float force[2]);

			/**
			* @brief	Update the map
			*
			* @param [in] dt Delta time for this frame
			*
			* @retval void No value
			*
			*/
			void UpdateMap(const char** mapForRoom);

			/**
			* @brief Updates the players rotation with given rotation
			* @param [in] The new x angle in radians
			* @param [in] The new y angle in radians
			**/
			void UpdatePlayerRotation(float camAngleX, float camAngleY);

		private:
			PlayerUnit() {};
			PlayerUnit(const PlayerUnit& other) = delete;
			PlayerUnit(const PlayerUnit&& other) = delete;
			PlayerUnit& operator=(const PlayerUnit& rhs) = delete;

			char map[25][25] = { {} };
			float forcesToApply[2] = {};
			float rotMov[2] = {};

		private:
			struct Stats
			{
				//std::string characterName;
				int str = 5;
				int agi = 5;
				int whi = 5;

				//str
				float health			 = 100.f;
				float damage			 = 1.f;
				float meleeMultiplier	 = 1.f;
				float physicalResistance = 1.f;

				//agi
				float rangedDamage		= 1.f;
				float rangedMultiplier  = 1.f;
				float movementSpeed		= 5.f;
				//float healBonus			= 1.f;
				//float attackSpeed		= 1.f;

				//whi
				float magicDamage		= 1.f;
				float magicMultiplier	= 1.f;
				float magicResistance	= 1.f;
				float natureResistance	= 1.f;
				float fireResistance	= 1.f;
				float waterResistance	= 1.f;
				
				int armorCap			= 3;

				ArmourType armour		= ArmourType::ARMOUR_TYPE_NONE;
				DamageSources weapon	= DamageSources::DAMAGE_SOURCE_MELEE;
				DamageTypes element		= DamageTypes::DAMAGE_TYPE_PHYSICAL;

				

			};
			Stats baseStat;
			Stats newStat;

			/**
			* @brief	Used to calculate the new strength stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcStrChanges();
			/**
			* @brief	Used to calculate the new agility stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcAgiChanges();
			/**
			* @brief	Used to calculate the new whisdom stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcWhiChanges();
			/**
			* @brief	  Changes the equipped armor type.
			* @param [in] The new given armor type.
			**/
			void changeArmorType(ArmourType armoUr);
			/**
			* @brief	  Changes the equipped weapon type.
			* @param [in] The new given weapon type.
			**/
			void changeWeaponType(DamageSources weapon);
			/**
			* @brief	  Changes the equipped element type.
			* @param [in] The new given element type.
			**/
			void changeElementType(DamageTypes element);
		
		public:


		private:		
			std::vector<Skill> skills;
			
			/**
			* @brief		Removes all the skills from the list.
			*
			* @param[ín]	skills is the list that will be emptied.
			**/
			void flushSkills(std::vector<Skill> skills);
			//void addPlayerSkills();
			//void movePlayerSkillsToAI();

			SkillFactory SF;

			//void changeElementType(Gameplay::DamageTypes element);
			
			float attackSpeed = 1.0f;
			float attackCooldown = 0.f;
		public:
			PlayerUnit(Skill* skills, void* perks, float xPos, float yPos, char mapForRoom[25][25]);
			~PlayerUnit();
		};

	}
}
#endif
