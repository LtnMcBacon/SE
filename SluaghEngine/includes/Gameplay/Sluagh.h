#ifndef SE_GAMEPLAY_SLUAGH_H
#define SE_GAMEPLAY_SLUAGH_H
#include "PlayerUnit.h"

namespace SE
{
	namespace Gameplay
	{
		class SluaghRoom;
		class Sluagh
		{
		private:
			PlayerUnit* thePlayer;
			PlayerUnit* theSluagh;

			float commitmentTime = 0.f;
			float timeSinceMovement = 0.f;
			PlayerUnit::MovementInput previousMovement;
			PlayerUnit::ActionInput previousAction;
			SluaghRoom* room;
			float distanceToPlayer;

			Sluagh() = delete;
		public:
			Sluagh(PlayerUnit* thePlayer, SluaghRoom* room);
			~Sluagh();
			void Update(float dt, std::vector<ProjectileData>& projectilesOut);
			bool CollisionAgainstProjectile(float projectileX, float projectileY, float projectileRadius);
			inline std::weak_ptr<GameUnit*> GetSharedPtr() { return theSluagh->GetSharedPtr(); };
			inline PlayerUnit* GetSluagh() { return theSluagh; };
			void ToggleRendering(bool render);

			void InitializeSluagh();
			bool SluaghMovedPreviousFrame();

		private:
			enum class UtilityMapEnum
			{
				SKILL1,
				SKILL2,
				WEAPON_SLOT_1,
				WEAPON_SLOT_2,
				WEAPON_SLOT_3,
				WEAPON_SLOT_4,
				WEAPON_SLOT_5,
				MOVE_UP,
				MOVE_DOWN,
				MOVE_LEFT,
				MOVE_RIGHT

			};

			std::unordered_map<UtilityMapEnum, float> utilityMap;

			enum class MovementDirection
			{
				UP,
				DOWN,
				LEFT,
				RIGHT
			};

			void DecideActions(float dt, PlayerUnit::MovementInput& movement, PlayerUnit::ActionInput& action);
			float UtilityForUsingACertainSkill(float dt, Skill& skillToCheck);
			float UtilityForChangingWeapon(float dt, int& weaponToSwapFor);
			float UtilityForUsingItem(float dt, int& item);
			float UtilityForMoveInDirection(float dt, MovementDirection dir);
			void ClearMap();

			void CalculateSkillUtilities(float dt);
			void CalculateWeaponChangeUtilities(float dt);
			void CalculateUseItemUtilities(float dt);
			void CalculateMovementUtilities(float dt);
			void DecideActionInput(float dt, PlayerUnit::ActionInput& action);
			void DecideMovementInput(float dt, PlayerUnit::MovementInput& movement, PlayerUnit::ActionInput& action);
		};
	}
}

#endif
