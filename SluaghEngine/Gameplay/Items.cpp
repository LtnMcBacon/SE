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

//
//SE::Core::Entity SE::Gameplay::Item::Weapon::Create()
//{
//	auto wep = CoreInit::managers.entityManager->Create();
//	auto type = ItemType::WEAPON;// GetRandType();
//	auto wType = WeaponType(std::rand() % weaponInfo.size());
//	auto ele = Stats::GetRandomDamageType();
//	CoreInit::managers.transformManager->Create(wep);
//	CoreInit::managers.transformManager->SetScale(wep, weaponInfo[size_t(wType)].scale);
//	CoreInit::managers.materialManager->Create(wep, { weaponInfo[size_t(wType)].shader, weaponInfo[size_t(wType)].mat });
//	CoreInit::managers.renderableManager->CreateRenderableObject(wep, { weaponInfo[size_t(wType)].mesh , false, false, true});
//	Core::IGUIManager::CreateInfo icon;
//	icon.texture = weaponInfo[size_t(wType)].icon;
//	icon.textureInfo.width = 50;
//	icon.textureInfo.height = 50;
//	icon.textureInfo.anchor = {0.5f,0.5f };
//	icon.textureInfo.screenAnchor = { 0, 1 };
//	icon.textureInfo.posX = 10;
//	icon.textureInfo.posY = -60;
//	icon.textureInfo.layerDepth = 0;
//
//	CoreInit::managers.guiManager->Create(wep, icon);
//
//	CoreInit::managers.dataManager->SetValue(wep, "Item", int32_t(type));
//	CoreInit::managers.dataManager->SetValue(wep, "Health", Stats::GetRandHealth());
//	CoreInit::managers.dataManager->SetValue(wep, "Str", Stats::GetRandStr());
//	CoreInit::managers.dataManager->SetValue(wep, "Agi", Stats::GetRandAgi());
//	CoreInit::managers.dataManager->SetValue(wep, "Wis", Stats::GetRandWil());
//	CoreInit::managers.dataManager->SetValue(wep, "Damage", Stats::GetRandDamage());
//	CoreInit::managers.dataManager->SetValue(wep, "Type", int32_t(wType));
//	CoreInit::managers.dataManager->SetValue(wep, "Element", int32_t(ele));
//	CoreInit::managers.dataManager->SetValue(wep, "AttAnim", weaponInfo[size_t(wType)].attAnim.id);
//	CoreInit::managers.dataManager->SetValue(wep, "AttProj", weaponInfo[size_t(wType)].projectile.id);
//
//	
//	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "StartRenderWIC");
//	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "WeaponPickUp");
//	return wep;
//}
//
//
//SE::Core::Entity SE::Gameplay::Item::Weapon::Create(WeaponType type)
//{
//	auto wep = CoreInit::managers.entityManager->Create();
//	auto itype = ItemType::WEAPON;// GetRandType();
//	auto wType = type;
//	auto ele = Stats::GetRandomDamageType();
//	CoreInit::managers.transformManager->Create(wep);
//	CoreInit::managers.transformManager->SetScale(wep, weaponInfo[size_t(wType)].scale);
//	CoreInit::managers.materialManager->Create(wep, { weaponInfo[size_t(wType)].shader, weaponInfo[size_t(wType)].mat });
//	CoreInit::managers.renderableManager->CreateRenderableObject(wep, { weaponInfo[size_t(wType)].mesh , false, false, true });
//	Core::IGUIManager::CreateInfo icon;
//	icon.texture = weaponInfo[size_t(wType)].icon;
//	icon.textureInfo.width = 50;
//	icon.textureInfo.height = 50;
//	icon.textureInfo.anchor = { 0.5f,0.5f };
//	icon.textureInfo.screenAnchor = { 0, 1 };
//	icon.textureInfo.posX = 10;
//	icon.textureInfo.posY = -60;
//	icon.textureInfo.layerDepth = 0;
//
//	CoreInit::managers.guiManager->Create(wep, icon);
//
//	CoreInit::managers.dataManager->SetValue(wep, "Item", int32_t(itype));
//	CoreInit::managers.dataManager->SetValue(wep, "Health", Stats::GetRandHealth());
//	CoreInit::managers.dataManager->SetValue(wep, "Str", Stats::GetRandStr());
//	CoreInit::managers.dataManager->SetValue(wep, "Agi", Stats::GetRandAgi());
//	CoreInit::managers.dataManager->SetValue(wep, "Wis", Stats::GetRandWil());
//	CoreInit::managers.dataManager->SetValue(wep, "Damage", 3);
//	CoreInit::managers.dataManager->SetValue(wep, "Type", int32_t(wType));
//	CoreInit::managers.dataManager->SetValue(wep, "Element", int32_t(ele));
//	CoreInit::managers.dataManager->SetValue(wep, "AttAnim", weaponInfo[size_t(wType)].attAnim.id);
//	CoreInit::managers.dataManager->SetValue(wep, "AttProj", weaponInfo[size_t(wType)].projectile.id);
//
//
//	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "StartRenderWIC");
//	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "WeaponPickUp");
//	return wep;
//}
//
//void SE::Gameplay::Item::Weapon::CreateMeta(Core::Entity ent)
//{
//
//	auto itype = ItemType::WEAPON;
//	auto wType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1));
//	//auto ele = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(from, "Element", -1));
//	CoreInit::managers.transformManager->Create(ent);
//	CoreInit::managers.transformManager->SetScale(ent, weaponInfo[size_t(wType)].scale);
//	CoreInit::managers.materialManager->Create(ent, { weaponInfo[size_t(wType)].shader, weaponInfo[size_t(wType)].mat });
//	CoreInit::managers.renderableManager->CreateRenderableObject(ent, { weaponInfo[size_t(wType)].mesh , false, false, true });
//	Core::IGUIManager::CreateInfo icon;
//	icon.texture = weaponInfo[size_t(wType)].icon;
//	icon.textureInfo.width = 50;
//	icon.textureInfo.height = 50;
//	icon.textureInfo.anchor = { 0.5f,0.5f };
//	icon.textureInfo.screenAnchor = { 0, 1 };
//	icon.textureInfo.posX = 10;
//	icon.textureInfo.posY = -60;
//	icon.textureInfo.layerDepth = 0;
//
//	CoreInit::managers.guiManager->Create(ent, icon);
//
//	//CoreInit::managers.dataManager->SetValue(to, "Health", std::get<int32_t>(CoreInit::managers.dataManager->GetValue(from, "Health", 0)));
//	//CoreInit::managers.dataManager->SetValue(to, "Str", std::get<int32_t>(CoreInit::managers.dataManager->GetValue(from, "Str", 0)));
//	//CoreInit::managers.dataManager->SetValue(to, "Agi", std::get<int32_t>(CoreInit::managers.dataManager->GetValue(from, "Agi", 0)));
//	//CoreInit::managers.dataManager->SetValue(to, "Wis", std::get<int32_t>(CoreInit::managers.dataManager->GetValue(from, "Wis", 0)));
//	//CoreInit::managers.dataManager->SetValue(to, "Damage", std::get<int32_t>(CoreInit::managers.dataManager->GetValue(from, "Damage", 0)));
//	//CoreInit::managers.dataManager->SetValue(to, "Type", int32_t(wType));
//	//CoreInit::managers.dataManager->SetValue(to, "Element", std::get<int32_t>(CoreInit::managers.dataManager->GetValue(from, "Element", -1)));
//	//CoreInit::managers.dataManager->SetValue(to, "AttAnim", weaponInfo[size_t(wType)].attAnim.id);
//	//CoreInit::managers.dataManager->SetValue(to, "AttProj", weaponInfo[size_t(wType)].projectile.id);
//
//
//
//	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "StartRenderWIC");
//	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "WeaponPickUp");
//}
//
//using namespace SE;
//static const auto wepInfo = [](SE::Core::Entity ent, Core::Entity parent, long posX,bool stop)
//{
//	long offset = -125;
//	long he = 35;
//	Core::ITextManager::CreateInfo ciStr;
//	auto s = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Str", 0));
//	ciStr.font = "CloisterBlack.spritefont";
//	ciStr.info.posX = posX;
//	ciStr.info.posY = offset;
//	ciStr.info.screenAnchor = { 0.5f, 0.5f };
//	ciStr.info.anchor = { 1,0.0f };
//	ciStr.info.scale = { 0.4f, 1.0f };
//	ciStr.info.height = he;
//	ciStr.info.text = L"Str: " + std::to_wstring(s);
//	auto wStr = CoreInit::managers.entityManager->Create();
//	/*CoreInit::managers.textManager->Create(wStr, ciStr);
//
//	if (stop)
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderItemInfo");
//	}
//	else
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderWIC");
//		CoreInit::managers.dataManager->SetValue(wStr, "Parent", parent);
//	}
//	
//	CoreInit::managers.textManager->ToggleRenderableText(wStr, true);
//
//	offset += he + 3;
//*/
//	s = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Agi", 0));
//	ciStr.font = "CloisterBlack.spritefont";
//	ciStr.info.posY = offset;
//	ciStr.info.text = L"Agi: " + std::to_wstring(s);
//	/*wStr = CoreInit::managers.entityManager->Create();
//	CoreInit::managers.textManager->Create(wStr, ciStr);
//
//	if (stop)
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderItemInfo");
//	}
//	else
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderWIC");
//		CoreInit::managers.dataManager->SetValue(wStr, "Parent", parent);
//	}
//	CoreInit::managers.textManager->ToggleRenderableText(wStr, true);
//
//	offset += he + 3;
//*/
//	s = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Wis", 0));
//	ciStr.font = "CloisterBlack.spritefont";
//	ciStr.info.posY = offset;
//	ciStr.info.text = L"Wis: " + std::to_wstring(s);
//	/*wStr = CoreInit::managers.entityManager->Create();
//	CoreInit::managers.textManager->Create(wStr, ciStr);
//	if (stop)
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderItemInfo");
//	}
//	else
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderWIC");
//		CoreInit::managers.dataManager->SetValue(wStr, "Parent", parent);
//	}
//	CoreInit::managers.textManager->ToggleRenderableText(wStr, true);
//
//	offset += he + 3;
//*/
//	Core::ITextManager::CreateInfo cielement;
//	cielement.font = "CloisterBlack.spritefont";
//	cielement.info.posX = posX;
//	cielement.info.posY = offset;
//	cielement.info.screenAnchor = { 0.5f, 0.5f };
//	cielement.info.anchor = { 1,0.0f };
//	cielement.info.scale = { 0.4f, 1.0f };
//	cielement.info.height = he;
//	auto element = DamageType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Element", -1)));
//	cielement.info.colour = elt[size_t(element)].color;
//	cielement.info.text = elt[size_t(element)].str;
//	/*auto weaponElement = CoreInit::managers.entityManager->Create();
//	CoreInit::managers.textManager->Create(weaponElement, cielement);
//
//	if (stop)
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponElement, "StopRenderItemInfo");
//	}
//	else
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponElement, "StopRenderWIC");
//		CoreInit::managers.dataManager->SetValue(weaponElement, "Parent", parent);
//	}
//	CoreInit::managers.textManager->ToggleRenderableText(weaponElement, true);
//
//	offset += he + 3;
//*/
//
//	Core::ITextManager::CreateInfo cidamage;
//	cidamage.font = "CloisterBlack.spritefont";
//	cidamage.info.posX = posX;
//	cidamage.info.posY = offset;
//	cidamage.info.screenAnchor = { 0.5f, 0.5f };
//	cidamage.info.anchor = { 1,0.0f };
//	cidamage.info.scale = { 0.4f, 1.0f };
//	cidamage.info.height = he;
//	auto damage = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Damage", -1));
//	cidamage.info.text = L"Damage: " + std::to_wstring(damage);
//	auto weaponDamage = CoreInit::managers.entityManager->Create();
//	CoreInit::managers.textManager->Create(weaponDamage, cidamage);
//	if (stop)
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponDamage, "StopRenderItemInfo");
//	}
//	else
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponDamage, "StopRenderWIC");
//		CoreInit::managers.dataManager->SetValue(weaponDamage, "Parent", parent);
//	}
//	CoreInit::managers.textManager->ToggleRenderableText(weaponDamage, true);
//	offset += he + 3;
//
//	cidamage.font = "CloisterBlack.spritefont";
//	cidamage.info.posX = posX;
//	cidamage.info.posY = offset;
//	cidamage.info.screenAnchor = { 0.5f, 0.5f };
//	cidamage.info.anchor = { 1,0.0f };
//	cidamage.info.scale = { 0.4f, 1.0f };
//	cidamage.info.height = he;
//	auto hp = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Health", -1));
//	cidamage.info.text = L"Health: " + std::to_wstring(hp);
//	auto weaponHealth = CoreInit::managers.entityManager->Create();
//	CoreInit::managers.textManager->Create(weaponHealth, cidamage);
//	if (stop)
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponHealth, "StopRenderItemInfo");
//	}
//	else
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponHealth, "StopRenderWIC");
//		CoreInit::managers.dataManager->SetValue(weaponHealth, "Parent", parent);
//	}
//	CoreInit::managers.textManager->ToggleRenderableText(weaponHealth, true);
//	offset += he + 3;
//
//
//	Core::IGUIManager::CreateInfo ciback;
//
//	auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1));
//
//	ciback.texture = weaponInfo[type].backTex;
//	ciback.textureInfo.width = 200;
//	ciback.textureInfo.height = offset + 130;
//	ciback.textureInfo.posX = posX+5;
//	ciback.textureInfo.posY = -130;
//	ciback.textureInfo.screenAnchor = { 0.5f, 0.5f };
//	ciback.textureInfo.anchor = { 1.0, 0.0f };
//	auto weaponBack = CoreInit::managers.entityManager->Create();
//	CoreInit::managers.guiManager->Create(weaponBack, ciback);
//	if (stop)
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponBack, "StopRenderItemInfo");
//	}
//	else
//	{
//		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponBack, "StopRenderWIC");
//		CoreInit::managers.dataManager->SetValue(weaponBack, "Parent", parent);
//	}
//	CoreInit::managers.guiManager->ToggleRenderableTexture(weaponBack, true);
//};
//
//void SE::Gameplay::Item::Weapon::ToggleRenderPickupInfo(Core::Entity ent)
//{
//	wepInfo(ent, ent, -40, false);
//}
//
//void SE::Gameplay::Item::Weapon::ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent)
//{
//
//	wepInfo(ent, parent, -245, true);
//
//
//}

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
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "StartRenderWIC");
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
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "StartRenderWIC");
}

void SE::Gameplay::Item::Pickup(Core::Entity ent)
{

	CoreInit::managers.collisionManager->Destroy(ent);
	CoreInit::managers.renderableManager->ToggleRenderableObject(ent, false);
	CoreInit::managers.guiManager->ToggleRenderableTexture(ent, true);
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "ItemPickup");
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "StartRenderWIC");
	CoreInit::managers.particleSystemManager->Destroy(ent);
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "RoomChange");
}

void SE::Gameplay::Item::GodPickup(Core::Entity ent)
{
	CoreInit::managers.collisionManager->Destroy(ent);
	CoreInit::managers.renderableManager->ToggleRenderableObject(ent, false);
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "ItemPickup");
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "StartRenderWIC");
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

void SE::Gameplay::Item::ToggleRenderPickupInfo(Core::Entity ent)
{
	auto item = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Item", -1)));
	if (item == ItemType::WEAPON)
		Weapon::ToggleRenderPickupInfo(ent);
	else if (item == ItemType::CONSUMABLE)
		Consumable::ToggleRenderPickupInfo(ent);
}

void SE::Gameplay::Item::ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent)
{
	auto item = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Item", -1)));
	if (item == ItemType::WEAPON)
		Weapon::ToggleRenderEquiuppedInfo(ent, parent);
	else if (item == ItemType::CONSUMABLE)
		Consumable::ToggleRenderEquiuppedInfo(ent, parent);
}
