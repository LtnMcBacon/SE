#include <Items.h>
#include "CoreInit.h"
#include <array>
struct WeaponInfo
{
	SE::Utilz::GUID icon;
	SE::Utilz::GUID iconP;
	SE::Utilz::GUID backTex;
	SE::Utilz::GUID mesh;
	SE::Utilz::GUID mat;
	SE::Utilz::GUID shader;
	SE::Utilz::GUID projectile;
	SE::Utilz::GUID attAnim;
	SE::Utilz::GUID equipJoint;
	int slotIndex;
	DirectX::XMFLOAT3 equipPos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 equipRot;
};

static const std::array<WeaponInfo, 3> weaponInfo = { {
	{"Sword_black.png", "Sword_black.png", "Physical.png", "Sword.mesh", "MCModell.mat", "SimpleLightPS.hlsl", "playerMeleeProjectiles.SEP", "TopSwordAttackAnim_MCModell.anim", "LHand", 0,{ 0.07f,0.15f,0.5f }, {1,1,1},{ -0.25f, 0.2f, 1.5f } },
	{"Crossbow_black.png", "Sword_black.png", "Range.png", "Crossbow_fbx.mesh", "Crossbow_fbx.mat", "SimpleLightPS.hlsl", "CrossbowAttack.SEP", "TopCrossbowAttackAnim_MCModell.anim" , "LHand", 0,{0, -0.02f, 0.25f },{ 0.15f,0.15f,0.15f },{0.0f,-1.2f,0.195f } },
	{"Wand_black.png","Sword_black.png", "Magic.png", "WandPivotEnd.mesh", "WandPivotEnd.mat", "SimpleLightPS.hlsl", "WandAttack.SEP", "TopWandAttackAnim_MCModell.anim", "LHand", 0,{ 0.049f, 0.024f,0.084f },{ 1,1,1 },{ -0.307f, 0.922f, 0.022f } }
} };


struct ConsumableInfo
{
	SE::Utilz::GUID icon;
	SE::Utilz::GUID backTex;
	SE::Utilz::GUID mesh;
	SE::Utilz::GUID mat;
	SE::Utilz::GUID shader;
};
static const std::array<ConsumableInfo, 1> consInfo = { {
	{"Water.png","Water.png", "Pot.mesh", "default.mat", "SimpleLightPS.hlsl" }

} };

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


SE::Gameplay::DamageType SE::Gameplay::Item::GetRandDamageType()
{
	return DamageType(CoreInit::subSystems.window->GetRand() % 4);
}


SE::Core::Entity SE::Gameplay::Item::Weapon::Create()
{
	auto wep = CoreInit::managers.entityManager->Create();
	auto type = ItemType::WEAPON;// GetRandType();
	auto wType = WeaponType(std::rand() % weaponInfo.size());
	auto ele = GetRandDamageType();
	CoreInit::managers.transformManager->Create(wep);
	CoreInit::managers.transformManager->SetScale(wep, weaponInfo[size_t(wType)].scale);
	CoreInit::managers.materialManager->Create(wep, { weaponInfo[size_t(wType)].shader, weaponInfo[size_t(wType)].mat });
	CoreInit::managers.renderableManager->CreateRenderableObject(wep, { weaponInfo[size_t(wType)].mesh , false, false, true});
	Core::IGUIManager::CreateInfo icon;
	icon.texture = weaponInfo[size_t(wType)].icon;
	icon.textureInfo.width = 50;
	icon.textureInfo.height = 50;
	icon.textureInfo.anchor = {0.5f,0.5f };
	icon.textureInfo.screenAnchor = { 0, 1 };
	icon.textureInfo.posX = 5;
	icon.textureInfo.posY = -60;
	icon.textureInfo.layerDepth = 0;

	CoreInit::managers.guiManager->Create(wep, icon);

	CoreInit::managers.dataManager->SetValue(wep, "Item", int32_t(type));
	CoreInit::managers.dataManager->SetValue(wep, "Health", Item::GetRandHealth());
	CoreInit::managers.dataManager->SetValue(wep, "Str", Item::GetRandStr());
	CoreInit::managers.dataManager->SetValue(wep, "Agi", Item::GetRandAgi());
	CoreInit::managers.dataManager->SetValue(wep, "Wis", Item::GetRandWil());
	CoreInit::managers.dataManager->SetValue(wep, "Damage", Item::GetRandDamage());
	CoreInit::managers.dataManager->SetValue(wep, "Type", int32_t(wType));
	CoreInit::managers.dataManager->SetValue(wep, "Element", int32_t(ele));
	CoreInit::managers.dataManager->SetValue(wep, "AttAnim", weaponInfo[size_t(wType)].attAnim.id);
	CoreInit::managers.dataManager->SetValue(wep, "AttProj", weaponInfo[size_t(wType)].projectile.id);

	
	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "StartRenderWIC");
	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "WeaponPickUp");
	return wep;
}

SE::Core::Entity SE::Gameplay::Item::Weapon::Create(WeaponType type)
{
	return Core::Entity();
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
	auto element = DamageType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Element", -1)));
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

	auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1));

	ciback.texture = weaponInfo[type].backTex;
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
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(ent, 0.8);
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
	//CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "RoomChange");
}

void SE::Gameplay::Item::Equip(Core::Entity to, Core::Entity ent)
{
	auto wType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1));
	if (wType != -1)
	{
		CoreInit::managers.transformManager->SetPosition(ent, weaponInfo[wType].equipPos);
		CoreInit::managers.transformManager->SetRotation(ent, weaponInfo[wType].equipRot.x, weaponInfo[wType].equipRot.y, weaponInfo[wType].equipRot.z);
		CoreInit::managers.animationManager->AttachToEntity(to, ent, weaponInfo[wType].equipJoint, weaponInfo[wType].slotIndex);
		CoreInit::managers.renderableManager->ToggleRenderableObject(ent, true);
	}

}

void SE::Gameplay::Item::Unequip(Core::Entity from ,Core::Entity ent)
{
	auto wType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1));
	if (wType != -1)
	{
		//CoreInit::managers.transformManager->SetPosition(ent, { 0.07f,0.15f,0.5f });
		//CoreInit::managers.transformManager->SetRotation(ent, -0.25f, 0.2f, 1.5f);
		CoreInit::managers.animationManager->DettachFromEntity(from, weaponInfo[wType].slotIndex);
		CoreInit::managers.renderableManager->ToggleRenderableObject(ent, false);
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

SE::Core::Entity SE::Gameplay::Item::Consumable::Create()
{
	auto type = std::rand() % consInfo.size();
	auto item = CoreInit::managers.entityManager->Create();
	auto itype = ItemType::CONSUMABLE;// GetRandType();

	CoreInit::managers.transformManager->Create(item);
	CoreInit::managers.materialManager->Create(item, { consInfo[type].shader, consInfo[type].mat});
	CoreInit::managers.renderableManager->CreateRenderableObject(item, { consInfo[type].mesh });
	Core::IGUIManager::CreateInfo icon;
	icon.texture = consInfo[type].icon;
	icon.textureInfo.width = 50;
	icon.textureInfo.height = 50;
	icon.textureInfo.anchor = { 0.5f,0.5f };
	icon.textureInfo.screenAnchor = { 0, 1 };
	icon.textureInfo.posX = 5;
	icon.textureInfo.posY = -60;
	icon.textureInfo.layerDepth = 0;

	CoreInit::managers.guiManager->Create(item , icon);

	CoreInit::managers.dataManager->SetValue(item, "Item", int32_t(itype));
	CoreInit::managers.dataManager->SetValue(item, "Health", Item::GetRandHealth());
	CoreInit::managers.dataManager->SetValue(item, "Type", int32_t(type));

	CoreInit::managers.eventManager->RegisterEntitytoEvent(item, "StartRenderWIC");
	CoreInit::managers.eventManager->RegisterEntitytoEvent(item, "WeaponPickUp");
	return item;
}

Core::Entity SE::Gameplay::Item::Consumable::Create(ConsumableType type)
{
	return Core::Entity();
}

static const auto renconsInfo = [](SE::Core::Entity ent, Core::Entity parent, long posX, bool stop)
{
	long offset = -125;
	long he = 35;
	
	Core::ITextManager::CreateInfo ci;
	ci.font = "CloisterBlack.spritefont";
	ci.info.posX = posX;
	ci.info.posY = offset;
	ci.info.screenAnchor = { 0.5f, 0.5f };
	ci.info.anchor = { 1,0.0f };
	ci.info.scale = { 0.4f, 1.0f };
	ci.info.height = he;
	auto hp = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Health", -1));
	ci.info.text = L"Health: " + std::to_wstring(hp);
	auto hpEnt = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(hpEnt, ci);
	if (stop)
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(hpEnt, "StopRenderItemInfo");
	}
	else
	{
		CoreInit::managers.eventManager->RegisterEntitytoEvent(hpEnt, "StopRenderWIC");
		CoreInit::managers.dataManager->SetValue(hpEnt, "Parent", parent);
	}
	CoreInit::managers.textManager->ToggleRenderableText(hpEnt, true);
	offset += he + 3;


	Core::IGUIManager::CreateInfo ciback;

	auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1));

	ciback.texture = consInfo[type].backTex;
	ciback.textureInfo.width = 200;
	ciback.textureInfo.height = offset + 130;
	ciback.textureInfo.posX = posX + 5;
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

void SE::Gameplay::Item::Consumable::ToggleRenderPickupInfo(Core::Entity ent)
{
	renconsInfo(ent, ent, -40, false);
}

void SE::Gameplay::Item::Consumable::ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent)
{
	renconsInfo(ent, parent, -245, true);
}
