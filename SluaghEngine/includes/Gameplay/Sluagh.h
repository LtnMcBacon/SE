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
			Sluagh(PlayerUnit* thePlayer);
			~Sluagh();
			void Update(float dt, std::vector<ProjectileData> &projectilesOut);
			bool CollisionAgainstProjectile(float projectileX, float projectileY, float projectileRadius);
			inline std::weak_ptr<GameUnit*> GetSharedPtr() { return theSluagh->GetSharedPtr(); };

		};
	}
}

#endif