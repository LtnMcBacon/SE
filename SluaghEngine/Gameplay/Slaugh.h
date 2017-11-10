#pragma once
#include "PlayerUnit.h"

namespace SE
{
	namespace Gameplay
	{
		class Slaugh
		{
		private:
			PlayerUnit* theSlaugh;

			float CalculateSkillUseUtility();
			float CalculateAttackUtility();
			float CalculateMoveUtility();
			float CalculateChangeWeaponUtility();

		public:
			Slaugh(std::string filePath, int startX, int startY);

			void Update(float dt, PlayerUnit* thePlayer, std::vector<ProjectileData> &createdProjectiles);
			bool IsAlive();



		};
	}
}
