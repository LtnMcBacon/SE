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

		struct Weapon
		{
			static Core::Entity CreateWeapon(DirectX::XMFLOAT3 pos);


			static int GetRandStr();
			static int GetRandAgi();
			static int GetRandWil();
			static int GetRandHealth();
			static int GetRandDamage();

			static WeaponType GetRandType();
			static std::wstring GetWString(WeaponType type);
			static std::wstring GetWString(Element ele);

			static DirectX::XMFLOAT4 GetElementColor(Element ele);
			static Element GetRandElement();
		};
	}
}
#endif