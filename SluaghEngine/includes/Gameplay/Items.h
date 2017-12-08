#ifndef SE_GAMEPLAY_ITEMS_H_
#define SE_GAMEPLAY_ITEMS_H_
#include "Stats.h"
#include "Weapon.h"
#include "Consumable.h"
#include <Utilz\GUID.h>
#include <Core\Entity.h>
#include <DirectXMath.h>
#include <fstream>

namespace SE
{
	namespace Gameplay
	{
		enum class ItemType
		{
			WEAPON,
			CONSUMABLE,
			NUM_TYPES,
			NONE
		};
		

		namespace Item
		{
			Core::Entity Create();

			/*struct Consumable
			{
				static Core::Entity Create();
				static Core::Entity Create(ConsumableType type);
				static void CreateMeta(Core::Entity ent);
				static void ToggleRenderPickupInfo(Core::Entity ent);
				static void ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent);
			};*/
			//struct Weapon
			//{
			//	static Core::Entity Create();
			//	static Core::Entity Create(WeaponType type);
			//	static void CreateMeta(Core::Entity ent);
			//	static void ToggleRenderPickupInfo(Core::Entity ent);
			//	static void ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent);
			//};
			
			void Drop(Core::Entity ent, DirectX::XMFLOAT3 pos);
			void Drop(Core::Entity ent);
			void Pickup(Core::Entity ent);
			void GodPickup(Core::Entity ent);
			void Equip(Core::Entity item, Core::Entity to);
			void Unequip(Core::Entity item, Core::Entity from);

			void RenderItemInfo(Core::Entity item, Core::Entity compareWith);

			Core::Entity Copy(Core::Entity toCopy);
			void WriteToFile(Core::Entity ent,const std::string& filename);
			void WriteToFile(Core::Entity ent, std::ofstream& file);

			Core::Entity Create(const std::string& filename);
			Core::Entity Create(std::ifstream& file);
		};
	}
}
#endif