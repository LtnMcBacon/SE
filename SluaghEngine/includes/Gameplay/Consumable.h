#ifndef SE_GAMEPLAY_CONSUMABLE_H_
#define SE_GAMEPLAY_CONSUMABLE_H_
#include <Core\Entity.h>

namespace SE::Gameplay::Item::Consumable
{
	enum class Type
	{
		HP,
		NONE
	};

	Core::Entity Create();
	Core::Entity Create(Type type);
	void CreateMeta(Core::Entity ent);
	void ToggleRenderPickupInfo(Core::Entity ent);
	void ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent);

	void RenderItemInfo(Core::Entity item, Core::Entity compareWith);
}

#endif //SE_GAMEPLAY_CONSUMABLE_H_