#include <Items.h>
#include "CoreInit.h"
#include <array>



struct ElT
{
	std::wstring str;
	DirectX::XMFLOAT4 color;
};


static const ElT elt[4] = {
	{L"Physical", {0.7f, 0.7f, 0.7f, 1.0f}},
	{ L"Fire",{ 0.8f, 0.3f, 0.2f, 1.0f } },
	{ L"Water",{ 0.1f, 0.1f, 0.8f, 1.0f } },
	{ L"Nature",{ 0.1f, 0.7f, 0.1f, 1.0f } }
};

using namespace SE::Gameplay;

SE::Core::Entity SE::Gameplay::Item::Copy(Core::Entity toCopy)
{
	if (auto item = std::get<int32_t>( CoreInit::managers.dataManager->GetValue(toCopy, "Item", -1)); item != -1)
	{
		auto newItem = CoreInit::managers.entityManager->Create();
		CoreInit::managers.dataManager->Copy(newItem, toCopy);

		switch (ItemType(item))
		{
		case ItemType::WEAPON:
			Weapon::CreateMeta(newItem);
			break;
		case ItemType::CONSUMABLE:
			Consumable::CreateMeta(newItem);
			break;
		}
		return newItem;
	}

	return -1;
}

void SE::Gameplay::Item::WriteToFile(Core::Entity ent, const std::string & filename)
{
	std::ofstream file(filename, std::ios::binary);

	if (file.is_open())
	{
		CoreInit::managers.dataManager->WriteToFile(ent, file);

		file.close();
	}
	else
	{
		CoreInit::subSystems.devConsole->PrintChannel("Item", "Could not open file for writing item, %s", filename.c_str());
	}
}

void SE::Gameplay::Item::WriteToFile(Core::Entity ent, std::ofstream & file)
{
	CoreInit::managers.dataManager->WriteToFile(ent, file);
}

SE::Core::Entity SE::Gameplay::Item::Create(const std::string & filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (file.is_open())
	{
		auto ent = Create(file);
		file.close();
		return ent;
	}
	else
	{
		CoreInit::subSystems.devConsole->PrintChannel("Item", "Could not open file for creating item, %s", filename.c_str());
	}
	return -1;
}

SE::Core::Entity SE::Gameplay::Item::Create(std::ifstream & file)
{
	auto ent = CoreInit::managers.entityManager->Create();
	CoreInit::managers.dataManager->CreateFromFile(ent, file);

	if (auto item = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Item", -1)); item != -1)
	{
		switch (ItemType(item))
		{
		case ItemType::WEAPON:
			Weapon::CreateMeta(ent);
			break;
		case ItemType::CONSUMABLE:
			Consumable::CreateMeta(ent);
			break;
		}
	}

	return ent;
}

SE::Core::Entity SE::Gameplay::Item::Create()
{
	auto rand = std::rand() % 100;
	if(rand < 80)
		return Weapon::Create();
	else
		return Consumable::Create();
	return Core::Entity();
}

void SE::Gameplay::Item::Drop(Core::Entity ent, DirectX::XMFLOAT3 pos)
{
	CoreInit::managers.transformManager->SetPosition(ent, pos);
	CoreInit::managers.transformManager->SetRotation(ent, 0,0,0);
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(ent, 0.2);
	CoreInit::managers.guiManager->ToggleRenderableTexture(ent, false);
	CoreInit::managers.particleSystemManager->CreateSystem(ent, { "lootParticle.pts" });
	CoreInit::managers.particleSystemManager->ToggleVisible(ent, true);
	CoreInit::managers.renderableManager->ToggleRenderableObject(ent, true);
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "ItemPickup");
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "RenderItemInfo");
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "RoomChange");
}

void SE::Gameplay::Item::Drop(Core::Entity ent)
{
	CoreInit::managers.transformManager->SetRotation(ent, 0, 0, 0);
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(ent, 0.2);
	CoreInit::managers.guiManager->ToggleRenderableTexture(ent, false);
	CoreInit::managers.particleSystemManager->CreateSystem(ent, { "lootParticle.pts" });
	CoreInit::managers.particleSystemManager->ToggleVisible(ent, true);
	CoreInit::managers.renderableManager->ToggleRenderableObject(ent, true);
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "ItemPickup");
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "RenderItemInfo");
}

void SE::Gameplay::Item::Pickup(Core::Entity ent)
{

	CoreInit::managers.collisionManager->Destroy(ent);
	CoreInit::managers.renderableManager->ToggleRenderableObject(ent, false);
	CoreInit::managers.guiManager->ToggleRenderableTexture(ent, true);
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "ItemPickup");
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "RenderItemInfo");
	CoreInit::managers.particleSystemManager->Destroy(ent);
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "RoomChange");
}

void SE::Gameplay::Item::GodPickup(Core::Entity ent)
{
	CoreInit::managers.collisionManager->Destroy(ent);
	CoreInit::managers.renderableManager->ToggleRenderableObject(ent, false);
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "ItemPickup");
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "RenderItemInfo");
	CoreInit::managers.particleSystemManager->Destroy(ent);
}

void SE::Gameplay::Item::Equip(Core::Entity item, Core::Entity to)
{
	auto itemType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(item, "Item", -1));
	if (ItemType(itemType) == ItemType::WEAPON)
	{
		Weapon::Equip(item, to);
	}

}

void SE::Gameplay::Item::Unequip(Core::Entity item, Core::Entity from)
{
	auto itemType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(item, "Item", -1));
	if (ItemType(itemType) == ItemType::WEAPON)
	{
		Weapon::UnEquip(item, from);
	}
}


void SE::Gameplay::Item::RenderItemInfo(Core::Entity item, Core::Entity compareWith)
{
	auto itemType = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(item, "Item", -1)));
	if (itemType == ItemType::WEAPON)
	{
		if(auto itemTypeCW = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(compareWith, "Item", -1))); itemTypeCW == ItemType::WEAPON)
			Weapon::RenderItemInfo(item, compareWith);
		else
			Weapon::RenderItemInfo(item, item);
	}

	else if (itemType == ItemType::CONSUMABLE)
	{
		if (auto itemTypeCW = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(compareWith, "Item", -1))); itemTypeCW == ItemType::CONSUMABLE)
			Consumable::RenderItemInfo(item, compareWith);
		else
			Consumable::RenderItemInfo(item, item);
	}
}
