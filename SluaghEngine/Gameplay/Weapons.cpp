#include <Weapons.h>
#include "CoreInit.h"

struct WeT
{
	std::wstring str;
	SE::Utilz::GUID mesh;
	SE::Utilz::GUID mat;
	SE::Utilz::GUID shader;
};

static const WeT wet[3] = {
	{L"Melee", "default.mesh", "default.mat", "SimplePS.hlsl"},
	{L"Ranged", "default.mesh", "default.mat", "SimplePS.hlsl" },
	{L"Magic", "default.mesh", "default.mat", "SimplePS.hlsl" }
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

int SE::Gameplay::Weapon::GetRandStr()
{
	return (CoreInit::subSystems.window->GetRand() % 201) - 100;
}

int SE::Gameplay::Weapon::GetRandAgi()
{
	return (CoreInit::subSystems.window->GetRand() % 201) - 100;
}

int SE::Gameplay::Weapon::GetRandWil()
{
	return (CoreInit::subSystems.window->GetRand() % 201) - 100;
}

int SE::Gameplay::Weapon::GetRandHealth()
{
	return (CoreInit::subSystems.window->GetRand() % 401) - 200;
}

int SE::Gameplay::Weapon::GetRandDamage()
{
	return (CoreInit::subSystems.window->GetRand() % 100);
}

SE::Gameplay::WeaponType SE::Gameplay::Weapon::GetRandType()
{
	return WeaponType(CoreInit::subSystems.window->GetRand() % 3);
}

std::wstring SE::Gameplay::Weapon::GetWString(WeaponType type)
{
	return wet[size_t(type)].str;
}

std::wstring SE::Gameplay::Weapon::GetWString(Element ele)
{
	return elt[size_t(ele)].str;
}

DirectX::XMFLOAT4 SE::Gameplay::Weapon::GetElementColor(Element ele)
{
	return elt[size_t(ele)].color;
}

SE::Gameplay::Element SE::Gameplay::Weapon::GetRandElement()
{
	return Element(CoreInit::subSystems.window->GetRand() % 4);
}


SE::Core::Entity SE::Gameplay::Weapon::CreateWeapon(DirectX::XMFLOAT3 pos)
{
	auto wep = CoreInit::managers.entityManager->Create();
	auto type = GetRandType();
	auto ele = GetRandElement();
	CoreInit::managers.transformManager->Create(wep, pos, {}, { 0.2f,0.2f,0.2f });
	CoreInit::managers.materialManager->Create(wep, { wet[size_t(type)].shader, wet[size_t(type)].mat });
	CoreInit::managers.renderableManager->CreateRenderableObject(wep, { wet[size_t(type)].mesh });
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(wep, wet[size_t(type)].mesh);
	CoreInit::managers.renderableManager->ToggleRenderableObject(wep, true);


	CoreInit::managers.dataManager->SetValue(wep, "Weapon", true);
	CoreInit::managers.dataManager->SetValue(wep, "Health", Weapon::GetRandHealth());
	CoreInit::managers.dataManager->SetValue(wep, "Str", Weapon::GetRandStr());
	CoreInit::managers.dataManager->SetValue(wep, "Agi", Weapon::GetRandAgi());
	CoreInit::managers.dataManager->SetValue(wep, "Wil", Weapon::GetRandWil());
	CoreInit::managers.dataManager->SetValue(wep, "Damage", Weapon::GetRandDamage());
	CoreInit::managers.dataManager->SetValue(wep, "Type", int(type));
	CoreInit::managers.dataManager->SetValue(wep, "Element", int(ele));
	CoreInit::managers.dataManager->SetValue(wep, "Name", "xXx_Killer_Slasher_Naruto_Killer_xXx"s);

	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "StartRenderWIC");
	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "WeaponPickUp");
	return wep;
}

void SE::Gameplay::Weapon::ToggleRenderPickupInfo(Core::Entity ent)
{
	long offset = -125;
	long he = 35;
	Core::ITextManager::CreateInfo ciname;
	auto s = std::get<std::string>(CoreInit::managers.dataManager->GetValue(ent, "Name", "NaN"s));
	ciname.font = "CloisterBlack.spritefont";
	ciname.info.posX = -35;
	ciname.info.text = L"Test";
	ciname.info.posY = offset;
	ciname.info.screenAnchor = { 0.5f, 0.5f };
	ciname.info.anchor = { 1,0.0f };
	ciname.info.scale = { 0.4f, 1.0f };
	ciname.info.height = he;
	ciname.info.text.assign(s.begin(), s.end());
	auto weaponName = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponName, ciname);

	CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponName, "StopRenderWIC");
	CoreInit::managers.dataManager->SetValue(weaponName, "Parent", ent);
	CoreInit::managers.textManager->ToggleRenderableText(weaponName, true);

	offset += he + 3;

	Core::ITextManager::CreateInfo citype;
	citype.font = "CloisterBlack.spritefont";
	citype.info.posX = -35;
	citype.info.posY = offset;
	citype.info.screenAnchor = { 0.5f, 0.5f };
	citype.info.anchor = { 1,0.0f };
	citype.info.scale = { 0.4f, 1.0f };
	citype.info.height = he;
	auto type = WeaponType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1)));
	citype.info.text = Weapon::GetWString(type);
	auto weaponType = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponType, citype);

	CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponType, "StopRenderWIC");
	CoreInit::managers.dataManager->SetValue(weaponType, "Parent", ent);
	CoreInit::managers.textManager->ToggleRenderableText(weaponType, true);

	offset += he + 3;

	Core::ITextManager::CreateInfo cielement;
	cielement.font = "CloisterBlack.spritefont";
	cielement.info.posX = -35;
	cielement.info.posY = offset;
	cielement.info.screenAnchor = { 0.5f, 0.5f };
	cielement.info.anchor = { 1,0.0f };
	cielement.info.scale = { 0.4f, 1.0f };
	cielement.info.height = he;
	auto element = Element(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Element", -1)));
	cielement.info.colour = Weapon::GetElementColor(element);
	cielement.info.text = Weapon::GetWString(element);
	auto weaponElement = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponElement, cielement);

	CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponElement, "StopRenderWIC");
	CoreInit::managers.dataManager->SetValue(weaponElement, "Parent", ent);
	CoreInit::managers.textManager->ToggleRenderableText(weaponElement, true);

	offset += he + 3;


	Core::ITextManager::CreateInfo cidamage;
	cidamage.font = "CloisterBlack.spritefont";
	cidamage.info.posX = -35;
	cidamage.info.posY = offset;
	cidamage.info.screenAnchor = { 0.5f, 0.5f };
	cidamage.info.anchor = { 1,0.0f };
	cidamage.info.scale = { 0.4f, 1.0f };
	cidamage.info.height = he;
	auto damage = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Damage", -1));
	cidamage.info.text = L"Damage: " + std::to_wstring(damage);
	auto weaponDamage = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponDamage, cidamage);
	CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponDamage, "StopRenderWIC");
	CoreInit::managers.dataManager->SetValue(weaponDamage, "Parent", ent);
	CoreInit::managers.textManager->ToggleRenderableText(weaponDamage, true);
	offset += he + 3;

	cidamage.font = "CloisterBlack.spritefont";
	cidamage.info.posX = -35;
	cidamage.info.posY = offset;
	cidamage.info.screenAnchor = { 0.5f, 0.5f };
	cidamage.info.anchor = { 1,0.0f };
	cidamage.info.scale = { 0.4f, 1.0f };
	cidamage.info.height = he;
	auto hp = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Health", -1));
	cidamage.info.text = L"Health: " + std::to_wstring(hp);
	auto weaponHealth = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponHealth, cidamage);
	CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponHealth, "StopRenderWIC");
	CoreInit::managers.dataManager->SetValue(weaponHealth, "Parent", ent);
	CoreInit::managers.textManager->ToggleRenderableText(weaponHealth, true);
	offset += he + 3;


	Core::IGUIManager::CreateInfo ciback;
	ciback.texture = "Crossbow_texture_wood.jpg";
	ciback.textureInfo.width = 200;
	ciback.textureInfo.height = 200;
	ciback.textureInfo.posX = -30;
	ciback.textureInfo.posY = -130;
	ciback.textureInfo.screenAnchor = { 0.5f, 0.5f };
	ciback.textureInfo.anchor = { 1.0, 0.0f };
	auto weaponBack = CoreInit::managers.entityManager->Create();
	CoreInit::managers.guiManager->Create(weaponBack, ciback);
	CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponBack, "StopRenderWIC");
	CoreInit::managers.dataManager->SetValue(weaponBack, "Parent", ent);
	CoreInit::managers.guiManager->ToggleRenderableTexture(weaponBack, true);
}
