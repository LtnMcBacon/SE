#ifndef SE_GAMEPLAY_SLUAGH_ROOM_H
#define SE_GAMEPLAY_SLUAGH_ROOM_H
#include "Room.h"
#include "Sluagh.h"
#include "ProjectileManager.h"

namespace SE
{
	namespace Gameplay
	{
		class SluaghRoom : public Room
		{
		public:
			explicit SluaghRoom(const Utilz::GUID& fileName, PlayerUnit* thePlayer, ProjectileManager* projectileManagers);
			~SluaghRoom() override;
			void Update(float dt, float playerX, float playerY) override;
		private:
			bool ProjectileAgainstEnemies(Projectile& projectile) override;
			ProjectileManager* projectileManagers;
			Sluagh* theSluagh;
		};
	}
}




#endif
