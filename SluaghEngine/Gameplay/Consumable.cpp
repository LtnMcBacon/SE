#include <Consumable.h>
#include "CoreInit.h"
#include <Items.h>
#include <array>

struct ConsumableInfo
{
	SE::Utilz::GUID icon;
	SE::Utilz::GUID backTex;
	SE::Utilz::GUID mesh;
	SE::Utilz::GUID mat;
	SE::Utilz::GUID shader;
};
static const std::array<ConsumableInfo, 1> consInfo = { {
	{ "Water.png","Water.png", "Pot.mesh", "Nuckelavee.mat", "SimpleLightPS.hlsl" }

	} };

SE::Core::Entity SE::Gameplay::Item::Consumable::Create()
{
	return Create(Consumable::Type(std::rand() % int(Consumable::Type::NONE)));
}

SE::Core::Entity SE::Gameplay::Item::Consumable::Create(Consumable::Type ctype)
{
	auto type = std::rand() % consInfo.size();
	auto item = CoreInit::managers.entityManager->Create();
	CoreInit::managers.dataManager->SetValue(item, "Item", int32_t(ItemType::CONSUMABLE));
	CoreInit::managers.dataManager->SetValue(item, "Health", Stats::GetRandHealth() / 10 + 1);
	CoreInit::managers.dataManager->SetValue(item, "Charges", int32_t(3));
	CoreInit::managers.dataManager->SetValue(item, "Type", int32_t(type));

	CreateMeta(item);
	
	return item;
}

void SE::Gameplay::Item::Consumable::CreateMeta(Core::Entity ent)
{
	auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1));
	auto itype = ItemType::CONSUMABLE;// GetRandType();

	CoreInit::managers.transformManager->Create(ent);
	CoreInit::managers.materialManager->Create(ent, { consInfo[type].shader, consInfo[type].mat });
	CoreInit::managers.renderableManager->CreateRenderableObject(ent, { consInfo[type].mesh });
	Core::IGUIManager::CreateInfo icon;
	icon.texture = consInfo[type].icon;
	icon.textureInfo.width = 50;
	icon.textureInfo.height = 50;
	icon.textureInfo.anchor = { 0.5f,0.5f };
	icon.textureInfo.screenAnchor = { 0, 1 };
	icon.textureInfo.posX = 5;
	icon.textureInfo.posY = -60;
	icon.textureInfo.layerDepth = 0;

	CoreInit::managers.guiManager->Create(ent, icon);
}
using namespace SE;
static const auto renconsInfo = [](SE::Core::Entity ent, Core::Entity parent, long posX, bool stop)
{
	/*long offset = -125;
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
	CoreInit::managers.guiManager->ToggleRenderableTexture(weaponBack, true);*/
};

void SE::Gameplay::Item::Consumable::ToggleRenderPickupInfo(Core::Entity ent)
{
	renconsInfo(ent, ent, -40, false);
}

void SE::Gameplay::Item::Consumable::ToggleRenderEquiuppedInfo(Core::Entity ent, Core::Entity parent)
{
	renconsInfo(ent, parent, -245, true);
}
