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

			Sluagh() = delete;
		public:
			Sluagh(PlayerUnit* thePlayer, char map[25][25]);
			~Sluagh();
			void Update(float dt, std::vector<ProjectileData> &projectilesOut);
			bool CollisionAgainstProjectile(float projectileX, float projectileY, float projectileRadius);
			inline std::weak_ptr<GameUnit*> GetSharedPtr() { return theSluagh->GetSharedPtr(); };
			inline PlayerUnit* GetSluagh() { return theSluagh; };
			void ToggleRendering(bool render);

		};
	}
}

#endif