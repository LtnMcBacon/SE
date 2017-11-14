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
		enum class WeaponType
		{
			MELEE,
			RANGED,
			MAGIC
		};
		enum class ItemType
		{
			WEAPON,
			CONSUMABLE
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
				static WeaponType GetRandWeaponType();
				static std::wstring GetWString(WeaponType type);
			};
			
			static void Drop(Core::Entity ent, DirectX::XMFLOAT3 pos);
			static void Pickup(Core::Entity ent);
			static void ToggleRenderPickupInfo(Core::Entity ent);

			static int GetRandStr();
			static int GetRandAgi();
			static int GetRandWil();
			static int GetRandHealth();
			static int GetRandDamage();

			
		
			static std::wstring GetWString(Element ele);

			static DirectX::XMFLOAT4 GetElementColor(Element ele);
			static Element GetRandElement();
		};
	}
}
#endif