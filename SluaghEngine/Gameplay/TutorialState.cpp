#include <TutorialState.h>
#include "CoreInit.h"
#include <Room.h>
#include <PlayerUnit.h>
#include <ProjectileData.h>

using namespace DirectX;

static SE::Core::IEngine::ManagerWrapper managers;
static SE::Core::IEngine::Subsystems subSystems;
#include <Items.h>
SE::Gameplay::TutorialState::TutorialState()
{
	managers = CoreInit::managers;
	subSystems = CoreInit::subSystems;

	room = new Room("TutorialRoom.room");
	room->Load();
	room->RenderRoom(true);

	Skill skills[3];

	skills[0].skillDamage = 10;
	skills[0].atkType = DamageSources::DAMAGE_SOURCE_MELEE;
	skills[0].cooldown = 5;
	skills[0].currentCooldown = 0;
	skills[0].damageType = DamageType::PHYSICAL;
	skills[0].projectileFileGUID = "playerMeleeProjectiles.SEP";

	skills[1].skillDamage = 10;
	skills[1].atkType = DamageSources::DAMAGE_SOURCE_MELEE;
	skills[1].cooldown = 5;
	skills[1].currentCooldown = 0;
	skills[1].damageType = DamageType::PHYSICAL;
	skills[1].projectileFileGUID = "playerMeleeProjectiles.SEP";

	char map[25][25];
	room->GetMap(map);



#pragma region SkillHUD


	skillIndicators[0].maxCooldown = skills[0].cooldown;
	skillIndicators[1].maxCooldown = skills[1].cooldown;

	skillIndicators[0].frame = CoreInit::managers.entityManager->Create();
	skillIndicators[1].frame = CoreInit::managers.entityManager->Create();
	skillIndicators[0].Image = CoreInit::managers.entityManager->Create();
	skillIndicators[1].Image = CoreInit::managers.entityManager->Create();

	SE::Core::IGUIManager::CreateInfo indicatorInfo;
	indicatorInfo.texture = "Q.png";
	indicatorInfo.textureInfo.posX = 1000;
	indicatorInfo.textureInfo.posY = 600;
	indicatorInfo.textureInfo.height = 100;
	indicatorInfo.textureInfo.width = 100;
	indicatorInfo.textureInfo.layerDepth = 0.001;
	indicatorInfo.textureInfo.anchor = { 0.0f, 0.0f };

	SE::Core::ITextManager::CreateInfo textInfo;
	textInfo.font = "CloisterBlack.spritefont";
	textInfo.info.colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	textInfo.info.layerDepth = indicatorInfo.textureInfo.layerDepth / 2;
	textInfo.info.anchor = { 0.0f, 0.0f };
	textInfo.info.height = indicatorInfo.textureInfo.height;
	textInfo.info.width = indicatorInfo.textureInfo.width;
	textInfo.info.posX = indicatorInfo.textureInfo.posX - 15;
	textInfo.info.posY = indicatorInfo.textureInfo.posY;
	textInfo.info.text = L"";

	CoreInit::managers.guiManager->Create(skillIndicators[0].frame, indicatorInfo);
	CoreInit::managers.guiManager->ToggleRenderableTexture(skillIndicators[0].frame, true);
	CoreInit::managers.textManager->Create(skillIndicators[0].Image, textInfo);
	CoreInit::managers.textManager->ToggleRenderableText(skillIndicators[0].Image, true);

	indicatorInfo.texture = "E.png";
	indicatorInfo.textureInfo.posX = 1150;
	indicatorInfo.textureInfo.posY = 600;

	textInfo.info.text = L"";
	textInfo.info.posX = indicatorInfo.textureInfo.posX - 15;
	textInfo.info.posY = indicatorInfo.textureInfo.posY;

	CoreInit::managers.guiManager->Create(skillIndicators[1].frame, indicatorInfo);
	CoreInit::managers.guiManager->ToggleRenderableTexture(skillIndicators[1].frame, true);
	CoreInit::managers.textManager->Create(skillIndicators[1].Image, textInfo);
	CoreInit::managers.textManager->ToggleRenderableText(skillIndicators[1].Image, true);

#pragma endregion


	player = new PlayerUnit(skills, nullptr,5, 10, map);
	player->SetZPosition(0.9f);
	player->PositionEntity(5.5f, 10.5f);


#pragma region Camera stuff
	Core::Entity cam;
	Core::ICameraManager::CreateInfo cInfo;
	cInfo.aspectRatio = CoreInit::subSystems.optionsHandler->GetOptionDouble("Camera", "aspectRatio", (1280.0f / 720.0f));
	cam = CoreInit::managers.cameraManager->GetActive();
	CoreInit::managers.cameraManager->UpdateCamera(cam, cInfo);

	float cameraRotationX = DirectX::XM_PI / 3;
	float cameraRotationY = DirectX::XM_PI / 3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	player->UpdatePlayerRotation(cameraRotationX, cameraRotationY);
	CoreInit::managers.transformManager->BindChild(player->GetEntity(), cam, false, true);
	CoreInit::managers.transformManager->Move(cam, -5 * cameraTranslation);
	CoreInit::managers.transformManager->SetRotation(cam, cameraRotationX, cameraRotationY, 0);

	//Create a default light

	Core::Entity dummy = CoreInit::managers.entityManager->Create();
	CoreInit::managers.transformManager->Create(dummy, { 0.0f, 1.95f, 0.0f });
	CoreInit::managers.transformManager->BindChild(player->GetEntity(), dummy, false, true);
	//CoreInit::managers.renderableManager->CreateRenderableObject(dummy, { "Placeholder_Block.mesh" });
	//CoreInit::managers.renderableManager->ToggleRenderableObject(dummy, true);

	SE::Core::ILightManager::CreateInfo lightInfo;
	lightInfo.pos = { 0.0f, 0.0f, 0.0f };
	lightInfo.color = { 0.74f, 0.92f, 0.95f };
	lightInfo.radius = 10.5f;
	lightInfo.intensity = 1.3f;
	CoreInit::managers.lightManager->Create(dummy, lightInfo);
	CoreInit::managers.lightManager->ToggleLight(dummy, true);
	CoreInit::managers.lightManager->SetShadowCaster(dummy);


#pragma endregion

	


	auto weapon = Item::Weapon::Create(WeaponType::SWORD);
	player->AddItem(weapon, 0);
}


SE::Gameplay::TutorialState::~TutorialState()
{
	delete player;
	delete room;
}

SE::Gameplay::IGameState::State SE::Gameplay::TutorialState::Update(void *& pass)
{

	PlayerUnit::MovementInput movement(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput action(false, false);
	std::vector<ProjectileData> newProjectiles;

	if (subSystems.window->ButtonDown(uint32_t(GameInput::UP)))
	{
		movement.upButton = true;
	}
	if (subSystems.window->ButtonDown(uint32_t(GameInput::DOWN)))
	{
		movement.downButton = true;
	}
	if (subSystems.window->ButtonDown(uint32_t(GameInput::LEFT)))
	{
		movement.leftButton = true;
	}
	if (subSystems.window->ButtonDown(uint32_t(GameInput::RIGHT)))
	{
		movement.rightButton = true;
	}

	int mX, mY;
	subSystems.window->GetMousePos(mX, mY);

	DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR rayD;


	auto width = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "width", 1280);
	auto height = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "height", 720);
	CoreInit::managers.cameraManager->WorldSpaceRayFromScreenPos(mX, mY, width, height, rayO, rayD);

	float distance = DirectX::XMVectorGetY(rayO) / -XMVectorGetY(rayD);

	auto clickPos = rayO + rayD*distance;

	movement.mousePosX = DirectX::XMVectorGetX(clickPos);
	movement.mousePosY = DirectX::XMVectorGetZ(clickPos);


	if (subSystems.window->ButtonPressed(uint32_t(GameInput::SKILL1)))
	{
		action.skill1Button = true;
	}

	if (subSystems.window->ButtonPressed(uint32_t(GameInput::SKILL2)))
	{
		action.skill2Button = true;
	}

	if (subSystems.window->ButtonDown(uint32_t(GameInput::ACTION)))
	{
		action.actionButton = true;
	}

	float dt = min(1 / 30.f, subSystems.window->GetDelta());


	player->Update(dt, movement, newProjectiles, action);

	//UpdateProjectiles(newProjectiles);

	room->Update(dt, player->GetXPosition(), player->GetYPosition());

	return State::TUTORIAL_STATE;
}
