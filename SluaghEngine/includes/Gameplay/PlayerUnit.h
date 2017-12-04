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
				PLAYER_RUN_ANIMATION,
				PLAYER_ATTACK_ANIMATION,
				PLAYER_ON_HIT_ANIMATION,
				PLAYER_ON_DEATH_ANIMATION
			};
			std::map<AvailableAnimations, std::vector<Utilz::GUID>> animationPlayInfos;

			void InitializeAnimationInfo();

			bool AnimationUpdate(AvailableAnimations animationToRun, Core::AnimationFlags animationFlags, float playSpeed = 1.f);


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
			void ResolveEvents(float dt);

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

			static const uint8_t MAX_ITEMS = 5;
			Core::Entity items[MAX_ITEMS];
			uint8_t currentItem = 0;
			uint8_t showingItem = 0;
			Core::Entity itemSelectedEntity;
			Stats weaponStats;
			bool displaying = false;
			bool hideP = false;
			bool godMode = false;
			void SetCurrentWeaponStats();
		public:
			//Cheats
			void SetGodMode(bool on);
			void SetSpeed(float speed);
			void Suicide();

			std::vector<DamageEvent>& GetDamageEvents();
			std::vector<HealingEvent>& GetHealingEvents();
			std::vector<ConditionEvent>& GetConditionEvents();
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


			struct ActionInput
			{
				bool actionButton = false;
				bool skill1Button = false;
				bool skill2Button = false;

				bool one = false;
				bool two = false;
				bool three = false;
				bool four = false;
				bool five = false;

				bool showInfo = false;

				ActionInput(bool skill1 = false, bool skill2 = false, bool action = false)
				{
					skill1Button = skill1;
					skill2Button = skill2;
					actionButton = action;

				}
			};

			/**
			* @brief	Update the map
			*
			* @param [in] dt Delta time for this frame
			*
			* @retval void No value
			*
			*/
			void UpdateMap(char** mapForRoom);

			/**
			* @brief Updates the players rotation with given rotation
			* @param [in] The new x angle in radians
			* @param [in] The new y angle in radians
			**/
			void UpdatePlayerRotation(float camAngleX, float camAngleY);

			void Update(float dt, const MovementInput& mInputs, std::vector<ProjectileData>& newProjectiles, const ActionInput& aInput);

			float GetMaxHealth()
			{
				return newStat.health;
			}


			void AddItem(Core::Entity item, uint8_t slot);
			inline Core::Entity GetCurrentItem()const
			{
				return items[currentItem];
			}
			inline Core::Entity GetItemToCompareWith()const
			{
				return items[showingItem];
			}
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

			void GetRotation(float &rotX, float &rotY) { rotX = rotMov[0]; rotY = rotMov[1]; };

			Core::Entity* GetAllItems() { return items; };
		private:

			PlayerUnit() {};
			PlayerUnit(const PlayerUnit& other) = delete;
			PlayerUnit(const PlayerUnit&& other) = delete;
			PlayerUnit& operator=(const PlayerUnit& rhs) = delete;

			char map[25][25] = { {} };
			float rotMov[2] = {};

			/* Sound */
			uint8_t nrAggroSounds = 6;
			SE::Utilz::GUID playerAggroSounds[6];
			uint8_t nrHealingSounds = 3;
			SE::Utilz::GUID playerHealingSounds[3];
			uint8_t nrAggroColdSounds = 3;
			SE::Utilz::GUID playerAggroColdSounds[3];
			uint8_t nrHealingColdSounds = 1;
			SE::Utilz::GUID playerHealingColdSounds[1];
			SE::Utilz::GUID currentSound;

			/**
			* @brief	Sets the sounds for the player
			*
			*/
			void PlayerSounds();





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

		private:
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
			void changeElementType(DamageType element);

		public:
			int getSkillVectorSize();

			//Skill Getters
			inline std::string GetSkillName(int skillNumber) { return skills.at(skillNumber).skillName; };
			inline DamageSources GetAttackType(int skillNumber) { return skills.at(skillNumber).atkType; };
			inline DamageType GetDamageType(int skillNumber) { return skills.at(skillNumber).damageType; };
			inline Boons GetBoon(int skillNumber) { return skills.at(skillNumber).boon; };
			inline Banes GetBanes(int skillNumber) { return skills.at(skillNumber).bane; };
			inline unsigned short int GetAnimation(int skillNumber){ return (unsigned short int)skills.at(skillNumber).animation; };
			inline unsigned short int GetParticle(int skillNumber) { return (unsigned short int)skills.at(skillNumber).particle; };

			inline Utilz::GUID GetProjectileReference(int skillNumber) { return skills.at(skillNumber).projectileFileGUID; };
			inline float GetSkillDamage(int skillNumber) { return skills.at(skillNumber).skillDamage; };
			inline float GetBoonEffectValue(int skillNumber) { return skills.at(skillNumber).boonEffectValue; };
			inline float GetBoonRange(int skillNumber) { return skills.at(skillNumber).boonRange; };
			inline float GetBoonDuration(int skillNumber) { return skills.at(skillNumber).boonDuration; };
			inline float GetBaneEffetValue(int skillNumber) { return skills.at(skillNumber).baneEffectValue; };
			inline float GetBaneRange(int skillNumber) { return skills.at(skillNumber).baneRange; };
			inline float GetBaneDuration(int skillNumber) { return skills.at(skillNumber).baneDuration; };
			inline float GetCooldown(int skillNumber) { return skills.at(skillNumber).cooldown; };
			inline float GetCurrentCooldown(int skillNumber) { return skills.at(skillNumber).currentCooldown; };
			
			//Skill Setters
			inline void SetSkillName(int skillNumber, std::string skillName) { this->skills.at(skillNumber).skillName = skillName; };
			inline void SetAttackType(int skillNumber, unsigned short int atkType) { this->skills.at(skillNumber).atkType = static_cast<DamageSources>(atkType); };
			inline void SetDamageType(int skillNumber, unsigned short int damageType) { this->skills.at(skillNumber).damageType = static_cast<DamageType>(damageType); };
			inline void SetBoon(int skillNumber, unsigned short int boon) { this->skills.at(skillNumber).boon = static_cast<Boons>(boon); };
			inline void SetBanes(int skillNumber, unsigned short int bane) { this->skills.at(skillNumber).bane = static_cast<Banes>(bane); };
			inline void SetAnimation(int skillNumber, unsigned short int animation) { this->skills.at(skillNumber).animation = animation; };
			inline void SetParticle(int skillNumber, unsigned short int particle) { this->skills.at(skillNumber).particle = particle; };
			
			inline void  SetProjectileReference(int skillNumber, std::string projectileName) { this->skills.at(skillNumber).projectileFileGUID = projectileName; };
			inline void  SetSkillDamage(int skillNumber, float skillDamage) { this->skills.at(skillNumber).skillDamage = skillDamage; };
			inline void  SetBoonEffectValue(int skillNumber, float effectValue) { this->skills.at(skillNumber).boonEffectValue = effectValue; };
			inline void  SetBoonRange(int skillNumber, float range) { this->skills.at(skillNumber).boonRange = range; };
			inline void  SetBoonDuration(int skillNumber, float duration) { this->skills.at(skillNumber).boonDuration = duration; };
			inline void  SetBaneEffectValue(int skillNumber, float effectValue) { this->skills.at(skillNumber).baneEffectValue = effectValue; };
			inline void  SetBaneRange(int skillNumber, float range) { this->skills.at(skillNumber).baneRange = range; };
			inline void  SetBaneDuration(int skillNumber, float duration) { this->skills.at(skillNumber).baneDuration = duration; };
			inline void  SetCooldown(int skillNumber, float cooldown) { this->skills.at(skillNumber).cooldown = cooldown; };
			inline void  SetCurrentCooldown(int skillNumber, float currentCooldown) { this->skills.at(skillNumber).currentCooldown = currentCooldown; };
			
			//Skill adders
			inline void  AddSkillDamage(int skillNumber, float skillDamage) { this->skills.at(skillNumber).skillDamage += skillDamage; };
			inline void  AddBoonEffectValue(int skillNumber, float effectValue) { this->skills.at(skillNumber).boonEffectValue += effectValue; };
			inline void  AddBoonRange(int skillNumber, float range) { this->skills.at(skillNumber).boonRange += range; };
			inline void  AddBoonDuration(int skillNumber, float duration) { this->skills.at(skillNumber).boonDuration += duration; };
			inline void  AddBaneEffectValue(int skillNumber, float effectValue) { this->skills.at(skillNumber).baneEffectValue += effectValue; };
			inline void  AddBaneRange(int skillNumber, float range) { this->skills.at(skillNumber).baneRange += range; };
			inline void  AddBaneDuration(int skillNumber, float duration) { this->skills.at(skillNumber).baneDuration += duration; };
			inline void  AddCooldown(int skillNumber, float cooldown) { this->skills.at(skillNumber).cooldown += cooldown; };
			inline void  AddCurrentCooldown(int skillNumber, float currentCooldown) { this->skills.at(skillNumber).currentCooldown += currentCooldown; };
			
			//Skillremovers
			inline void  RemoveSkillDamage(int skillNumber, float skillDamage) { this->skills.at(skillNumber).skillDamage -= skillDamage; };
			inline void  RemoveBoonEffectValue(int skillNumber, float effectValue) { this->skills.at(skillNumber).boonEffectValue -= effectValue; };
			inline void  RemoveBoonRange(int skillNumber, float range) { this->skills.at(skillNumber).boonRange -= range; };
			inline void  RemoveBoonDuration(int skillNumber, float duration) { this->skills.at(skillNumber).boonDuration -= duration; };
			inline void  RemoveBaneEffectValue(int skillNumber, float effectValue) { this->skills.at(skillNumber).baneEffectValue -= effectValue; };
			inline void  RemoveBaneDuration(int skillNumber, float duration) { this->skills.at(skillNumber).baneDuration -= duration; };
			inline void  RemoveCooldown(int skillNumber, float cooldown) { this->skills.at(skillNumber).cooldown -= cooldown; };
			inline void  RemoveCurrentCooldown(int skillNumber, float currentCooldown) { this->skills.at(skillNumber).currentCooldown -= currentCooldown; };
			
			inline std::vector<Skill> &GetAllSkills() { return skills; };

			void ToggleAsSluagh(bool sluagh);




		private:
			std::vector<Skill> skills;

			/**
			* @brief		Removes all the skills from the list.
			*
			* @param[ín]	skills is the list that will be emptied.
			**/
			inline void flushSkills(std::vector<Skill> skills) { skills.clear(); };
			//void addPlayerSkills();
			//void movePlayerSkillsToAI();

			SkillFactory SF;

			//void changeElementType(Gameplay::DamageTypes element);

			bool isStunned = false;
			bool attacking = false;
			float attackSpeed = 5.f;
			float attackCooldown = 0.f;
			bool isSluagh = false;
		public:
			PlayerUnit(Skill* skills, void* perks, float xPos, float yPos, char mapForRoom[25][25]);
			PlayerUnit(std::ifstream &input, float xPos, float yPos, char mapForRoom[25][25]);
			void SavePlayerToFile(std::ofstream &toSave);
			~PlayerUnit();
		};

	}
}

#endif
