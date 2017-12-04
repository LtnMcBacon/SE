#ifndef SE_GAMEPLAY_WEAPON_H_
#define SE_GAMEPLAY_WEAPON_H_
#include <Core\Entity.h>
namespace SE::Gameplay::Item::Weapon
{
	enum class Type
	{
		SWORD,
		CROSSBOW,
		WAND,
		NONE
	};
	Core::Entity Create();
	Core::Entity Create(Type type, bool base = true);
	void CreateMeta(Core::Entity wep);
	void ToggleRenderPickupInfo(Core::Entity wep);
	void ToggleRenderEquiuppedInfo(Core::Entity wep, Core::Entity parent);

	void RenderItemInfo(Core::Entity item, Core::Entity compareWith);

	void Equip(Core::Entity wep, Core::Entity to);
	void UnEquip(Core::Entity wep, Core::Entity from);
}

#endif