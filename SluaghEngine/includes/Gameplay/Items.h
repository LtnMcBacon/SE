#ifndef SE_GAMEPLAY_WEAPON_H_
#define SE_GAMEPLAY_WEAPON_H_
#include "Stats.h"
#include <Utilz\GUID.h>
#include <Core\Entity.h>
#include <DirectXMath.h>
namespace SE
{
	namespace Gameplay
	{
		enum class ItemType
		{
			WEAPON,
			CONSUMABLE
		};
		enum class WeaponType
		{
			SWORD,
			CROSSBOW,
			STAFF
		};
		struct Item
		{
			static Core::Entity Create();

			struct Consumable
			{
				static Core::Entity Create();
			};
			struct Weapon
			{
				static Core::Entity Create();
				static Core::Entity Create(WeaponType type);
				static void ToggleRenderPickupInfo(Core::Entity ent);
				static void ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent);
			};
			
			static void Drop(Core::Entity ent, DirectX::XMFLOAT3 pos);
			static void Pickup(Core::Entity ent);
			static void ToggleRenderPickupInfo(Core::Entity ent);
			static void ToggleRenderEquiuppedInfo(Core::Entity item, Core::Entity parent);

			static int GetRandStr();
			static int GetRandAgi();
			static int GetRandWil();
			static int GetRandHealth();
			static int GetRandDamage();

			static Element GetRandElement();
		};
	}
}
#endif