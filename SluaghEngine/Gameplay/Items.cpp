#include <Items.h>
#include "CoreInit.h"

struct TypeInfo
{
	SE::Utilz::GUID icon;
	SE::Utilz::GUID mesh;
	SE::Utilz::GUID mat;
	SE::Utilz::GUID shader;
};

static const TypeInfo typeInfo[3] = {
	{"venomblades.jpg", "Sword.mesh", "Sword.mat", "SimpleLightPS.hlsl"},
	{"venomblades.jpg", "default.mesh", "default.mat", "SimplePS.hlsl" },
	{"venomblades.jpg", "default.mesh", "default.mat", "SimplePS.hlsl" }
};


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

int SE::Gameplay::Item::GetRandStr()
{
	return (CoreInit::subSystems.window->GetRand() % 201) - 100;
}

int SE::Gameplay::Item::GetRandAgi()
{
	return (CoreInit::subSystems.window->GetRand() % 201) - 100;
}

int SE::Gameplay::Item::GetRandWil()
{
	return (CoreInit::subSystems.window->GetRand() % 201) - 100;
}

int SE::Gameplay::Item::GetRandHealth()
{
	return (CoreInit::subSystems.window->GetRand() % 401) - 200;
}

int SE::Gameplay::Item::GetRandDamage()
{
	return (CoreInit::subSystems.window->GetRand() % 100);
}


SE::Gameplay::Element SE::Gameplay::Item::GetRandElement()
{
	return Element(CoreInit::subSystems.window->GetRand() % 4);
}


SE::Core::Entity SE::Gameplay::Item::Weapon::Create()
{
	auto wep = CoreInit::managers.entityManager->Create();
	auto type = ItemType::WEAPON;// GetRandType();
	auto ele = GetRandElement();
	CoreInit::managers.transformManager->Create(wep);
	CoreInit::managers.materialManager->Create(wep, { typeInfo[size_t(type)].shader, typeInfo[size_t(type)].mat });
	CoreInit::managers.renderableManager->CreateRenderableObject(wep, { typeInfo[size_t(type)].mesh });
	Core::IGUIManager::CreateInfo icon;
	icon.texture = typeInfo[size_t(type)].icon;
	icon.textureInfo.width = 50;
	icon.textureInfo.height = 50;
	icon.textureInfo.anchor = {0.5f,0.5f };
	icon.textureInfo.screenAnchor = { 0, 1 };
	icon.textureInfo.posX = 5;
	icon.textureInfo.posY = -60;
	icon.textureInfo.layerDepth = 0;

	CoreInit::managers.guiManager->Create(wep, icon);

	CoreInit::managers.dataManager->SetValue(wep, "Item", true);
	CoreInit::managers.dataManager->SetValue(wep, "Health", Item::GetRandHealth());
	CoreInit::managers.dataManager->SetValue(wep, "Str", Item::GetRandStr());
	CoreInit::managers.dataManager->SetValue(wep, "Agi", Item::GetRandAgi());
	CoreInit::managers.dataManager->SetValue(wep, "Wis", Item::GetRandWil());
	CoreInit::managers.dataManager->SetValue(wep, "Damage", Item::GetRandDamage());
	CoreInit::managers.dataManager->SetValue(wep, "Type", int32_t(type));
	CoreInit::managers.dataManager->SetValue(wep, "Element", int32_t(ele));

	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "StartRenderWIC");
	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "WeaponPickUp");
	return wep;
}

using namespace SE;
static const auto wepInfo = [](SE::Core::Entity ent, Core::Entity parent, long posX,bool stop)
{
	long offset = -125;
	long he = 35;
	Core::ITextManager::CreateInfo ciStr;
	auto s = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Str", 0));
	ciStr.font = "CloisterBlack.spritefont";
	ciStr.info.posX = posX;
	ciStr.info.posY = offset;
	ciStr.info.screenAnchor = { 0.5f, 0.5f };
	ciStr.info.anchor = { 1,0.0f };
	ciStr.info.scale = { 0.4f, 1.0f };
	ciStr.info.height = he;
	ciStr.info.text = L"Str: " + std::to_wstring(s);
	auto wStr = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(wStr, ciStr);

	if (stop)
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderItemInfo");
	}
	else
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderWIC");
		CoreInit::managers.dataManager->SetValue(wStr, "Parent", parent);
	}
	
	CoreInit::managers.textManager->ToggleRenderableText(wStr, true);

	offset += he + 3;

	s = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Agi", 0));
	ciStr.font = "CloisterBlack.spritefont";
	ciStr.info.posY = offset;
	ciStr.info.text = L"Agi: " + std::to_wstring(s);
	wStr = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(wStr, ciStr);

	if (stop)
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderItemInfo");
	}
	else
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderWIC");
		CoreInit::managers.dataManager->SetValue(wStr, "Parent", parent);
	}
	CoreInit::managers.textManager->ToggleRenderableText(wStr, true);

	offset += he + 3;

	s = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Wis", 0));
	ciStr.font = "CloisterBlack.spritefont";
	ciStr.info.posY = offset;
	ciStr.info.text = L"Wis: " + std::to_wstring(s);
	wStr = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(wStr, ciStr);
	if (stop)
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderItemInfo");
	}
	else
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(wStr, "StopRenderWIC");
		CoreInit::managers.dataManager->SetValue(wStr, "Parent", parent);
	}
	CoreInit::managers.textManager->ToggleRenderableText(wStr, true);

	offset += he + 3;

	Core::ITextManager::CreateInfo cielement;
	cielement.font = "CloisterBlack.spritefont";
	cielement.info.posX = posX;
	cielement.info.posY = offset;
	cielement.info.screenAnchor = { 0.5f, 0.5f };
	cielement.info.anchor = { 1,0.0f };
	cielement.info.scale = { 0.4f, 1.0f };
	cielement.info.height = he;
	auto element = Element(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Element", -1)));
	cielement.info.colour = elt[size_t(element)].color;
	cielement.info.text = elt[size_t(element)].str;
	auto weaponElement = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponElement, cielement);

	if (stop)
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponElement, "StopRenderItemInfo");
	}
	else
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponElement, "StopRenderWIC");
		CoreInit::managers.dataManager->SetValue(weaponElement, "Parent", parent);
	}
	CoreInit::managers.textManager->ToggleRenderableText(weaponElement, true);

	offset += he + 3;


	Core::ITextManager::CreateInfo cidamage;
	cidamage.font = "CloisterBlack.spritefont";
	cidamage.info.posX = posX;
	cidamage.info.posY = offset;
	cidamage.info.screenAnchor = { 0.5f, 0.5f };
	cidamage.info.anchor = { 1,0.0f };
	cidamage.info.scale = { 0.4f, 1.0f };
	cidamage.info.height = he;
	auto damage = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Damage", -1));
	cidamage.info.text = L"Damage: " + std::to_wstring(damage);
	auto weaponDamage = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponDamage, cidamage);
	if (stop)
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponDamage, "StopRenderItemInfo");
	}
	else
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponDamage, "StopRenderWIC");
		CoreInit::managers.dataManager->SetValue(weaponDamage, "Parent", parent);
	}
	CoreInit::managers.textManager->ToggleRenderableText(weaponDamage, true);
	offset += he + 3;

	cidamage.font = "CloisterBlack.spritefont";
	cidamage.info.posX = posX;
	cidamage.info.posY = offset;
	cidamage.info.screenAnchor = { 0.5f, 0.5f };
	cidamage.info.anchor = { 1,0.0f };
	cidamage.info.scale = { 0.4f, 1.0f };
	cidamage.info.height = he;
	auto hp = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Health", -1));
	cidamage.info.text = L"Health: " + std::to_wstring(hp);
	auto weaponHealth = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponHealth, cidamage);
	if (stop)
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponHealth, "StopRenderItemInfo");
	}
	else
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponHealth, "StopRenderWIC");
		CoreInit::managers.dataManager->SetValue(weaponHealth, "Parent", parent);
	}
	CoreInit::managers.textManager->ToggleRenderableText(weaponHealth, true);
	offset += he + 3;


	Core::IGUIManager::CreateInfo ciback;
	ciback.texture = "parchment.jpg";
	ciback.textureInfo.width = 200;
	ciback.textureInfo.height = offset + 130;
	ciback.textureInfo.posX = posX+5;
	ciback.textureInfo.posY = -130;
	ciback.textureInfo.screenAnchor = { 0.5f, 0.5f };
	ciback.textureInfo.anchor = { 1.0, 0.0f };
	auto weaponBack = CoreInit::managers.entityManager->Create();
	CoreInit::managers.guiManager->Create(weaponBack, ciback);
	if (stop)
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponBack, "StopRenderItemInfo");
	}
	else
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponBack, "StopRenderWIC");
		CoreInit::managers.dataManager->SetValue(weaponBack, "Parent", parent);
	}
	CoreInit::managers.guiManager->ToggleRenderableTexture(weaponBack, true);
};

void SE::Gameplay::Item::Weapon::ToggleRenderPickupInfo(Core::Entity ent)
{
	wepInfo(ent, ent, -40, false);
}

void SE::Gameplay::Item::Weapon::ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent)
{

	wepInfo(ent, parent, -245, true);


}

SE::Core::Entity SE::Gameplay::Item::Create()
{
	return Core::Entity();
}

void SE::Gameplay::Item::Drop(Core::Entity ent, DirectX::XMFLOAT3 pos)
{
	CoreInit::managers.transformManager->SetPosition(ent, pos);
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(ent, 0.2);
	CoreInit::managers.guiManager->ToggleRenderableTexture(ent, false);
	CoreInit::managers.renderableManager->ToggleRenderableObject(ent, true);
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "WeaponPickUp");
	CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "StartRenderWIC");
}

void SE::Gameplay::Item::Pickup(Core::Entity ent)
{
	CoreInit::managers.collisionManager->Destroy(ent);
	CoreInit::managers.renderableManager->ToggleRenderableObject(ent, false);
	CoreInit::managers.guiManager->ToggleRenderableTexture(ent, true);
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "WeaponPickUp");
	CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "StartRenderWIC");
}

void SE::Gameplay::Item::ToggleRenderPickupInfo(Core::Entity ent)
{
	Weapon::ToggleRenderPickupInfo(ent);
}

void SE::Gameplay::Item::ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent)
{
	if (auto item = std::get<bool>(CoreInit::managers.dataManager->GetValue(ent, "Item", false)))
	{
		if (item)
			Weapon::ToggleRenderEquiuppedInfo(ent, parent);
	}
}

SE::Core::Entity SE::Gameplay::Item::Consumable::Create()
{
	return Core::Entity();
}
