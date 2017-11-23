#ifndef SE_GAMEPLAY_SLUAGH_H
#define SE_GAMEPLAY_SLUAGH_H
#include "PlayerUnit.h"

namespace SE
{
	namespace Gameplay
	{
		class Sluagh
		{
		private:
			PlayerUnit* thePlayer;
			PlayerUnit* theSluagh;

			PlayerUnit::MovementInput previousMovement;
			PlayerUnit::ActionInput previousAction;

			Sluagh() = delete;
		public:
			Sluagh(PlayerUnit* thePlayer, char map[25][25]);
			~Sluagh();
			void Update(float dt, std::vector<ProjectileData>& projectilesOut);
			bool CollisionAgainstProjectile(float projectileX, float projectileY, float projectileRadius);
			inline std::weak_ptr<GameUnit*> GetSharedPtr() { return theSluagh->GetSharedPtr(); };
			inline PlayerUnit* GetSluagh() { return theSluagh; };
			void ToggleRendering(bool render);

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
		};
	}
}

#endif
