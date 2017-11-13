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

	CoreInit::managers.eventManager->RegisterEntitytoEvent(wep, "WeaponPickUp");
	return wep;
}
