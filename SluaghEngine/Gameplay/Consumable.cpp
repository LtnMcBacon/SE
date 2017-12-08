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
	{ "WaterPickup.png","WaterPickup.png", "Pot.mesh", "Nuckelavee.mat", "SimpleLightPS.hlsl" }

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
	CoreInit::managers.dataManager->SetValue(item, "Health", Stats::GetRandHealth() / 5 + 1);
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
	icon.textureInfo.layerDepth = 1;


	CoreInit::managers.guiManager->Create(ent, icon);
}
using namespace SE;

void SE::Gameplay::Item::Consumable::RenderItemInfo(Core::Entity item, Core::Entity compareWith)
{
	long posY = -40;
	long textHeigth = 35;
	Core::ITextManager::CreateInfo tci;
	auto health = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(item, "Health", 0));
	auto healthCW = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(compareWith, "Health", 0));
	if (health > healthCW)
		tci.info.colour = { 0, 1.0f, 0.0f, 1.0f };
	else if (health < healthCW)
		tci.info.colour = { 1.0f, 0.0f, 0.0f, 1.0f };
	else
		tci.info.colour = { 1.0f, (165 / 255.0f), 0.0f, 1.0f };
	//tci.info.colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	tci.font = "CloisterBlack.spritefont";
	tci.info.posX = -40;
	tci.info.posY = posY;
	tci.info.screenAnchor = { 0.5f, 0.5f };
	tci.info.anchor = { 1.0f,0.0f };
	tci.info.scale = { 0.4f, 1.0f };
	tci.info.height = textHeigth;
	tci.info.text = L"Hälsa";
	auto textEnt = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(textEnt, tci);
	CoreInit::managers.textManager->ToggleRenderableText(textEnt, true);
	CoreInit::managers.eventManager->RegisterEntitytoEvent(textEnt, "StopRenderItemInfo");
	posY += textHeigth + 5;


	auto charges = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(item, "Charges", 0));
	auto chargesCW = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(compareWith, "Charges", 0));
	if (charges > chargesCW)
		tci.info.colour = { 0, 1.0f, 0.0f, 1.0f };
	else if (charges < chargesCW)
		tci.info.colour = { 1.0f, 0.0f, 0.0f, 1.0f };
	else
		tci.info.colour = { 1.0f, (165 / 255.0f), 0.0f, 1.0f };
	tci.info.posY = posY;
	tci.info.text = L"Klunkar";
	textEnt = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(textEnt, tci);
	CoreInit::managers.textManager->ToggleRenderableText(textEnt, true);
	CoreInit::managers.eventManager->RegisterEntitytoEvent(textEnt, "StopRenderItemInfo");
	posY += textHeigth + 5;


	Core::IGUIManager::CreateInfo ciback;
	auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(item, "Type", -1));
	ciback.texture = consInfo[type].backTex;
	ciback.textureInfo.width = 135;
	ciback.textureInfo.height = 125;
	ciback.textureInfo.posX = -30;
	ciback.textureInfo.posY = 0;
	ciback.textureInfo.screenAnchor = { 0.5f, 0.5f };
	ciback.textureInfo.anchor = { 1.0f, 0.5f };
	auto weaponBack = CoreInit::managers.entityManager->Create();
	CoreInit::managers.guiManager->Create(weaponBack, ciback);

	CoreInit::managers.guiManager->ToggleRenderableTexture(weaponBack, true);

	CoreInit::managers.eventManager->RegisterEntitytoEvent(weaponBack, "StopRenderItemInfo");
}
