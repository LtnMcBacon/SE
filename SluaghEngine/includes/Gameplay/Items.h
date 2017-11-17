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
			CONSUMABLE,
			NUM_TYPES
		};
		enum class ConsumableType
		{
			HP
		};
		enum class WeaponType
		{
			SWORD,
			CROSSBOW,
			WAND
		};
		struct Item
		{
			static Core::Entity Create();

			struct Consumable
			{
				static Core::Entity Create();
				static Core::Entity Create(ConsumableType type);
				static void ToggleRenderPickupInfo(Core::Entity ent);
				static void ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent);
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
			static void Equip(Core::Entity to, Core::Entity ent);
			static void Unequip(Core::Entity from, Core::Entity ent);
			static void ToggleRenderPickupInfo(Core::Entity ent);
			static void ToggleRenderEquiuppedInfo(Core::Entity item, Core::Entity parent);

			static int GetRandStr();
			static int GetRandAgi();
			static int GetRandWil();
			static int GetRandHealth();
			static int GetRandDamage();

			static DamageType GetRandDamageType();
		};
	}
}
#endif