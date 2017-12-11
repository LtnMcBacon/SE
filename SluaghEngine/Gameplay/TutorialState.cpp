#include <TutorialState.h>
#include "CoreInit.h"
#include <Room.h>
#include <PlayerUnit.h>
#include <ProjectileData.h>
#include <Items.h>

using namespace DirectX;

static SE::Core::IEngine::ManagerWrapper managers;
static SE::Core::IEngine::Subsystems subSystems;

SE::Gameplay::TutorialState::TutorialState()
{
	managers = CoreInit::managers;
	subSystems = CoreInit::subSystems;

	room = new Room("TutorialRoom.room");
	room->Load();
	room->RenderRoom(true);

	subSystems.devConsole->AddCommand([this](DevConsole::IConsole* backend, int argc, char** argv) {
	
		scriptToRun = &TutorialState::SpawnaGlastigScript;
	
	}, "skip", "Skips the tutorial");

	Skill skills[3];

	skills[0].skillDamage = 10;
	skills[0].atkType = DamageSources::DAMAGE_SOURCE_RANGED;
	skills[0].cooldown = 5;
	skills[0].currentCooldown = 0;
	skills[0].damageType = DamageType::PHYSICAL;
	skills[0].projectileFileGUID = "DaggerRain.SEP";

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

	skillIndicators[0].frame = managers.entityManager->Create();
	skillIndicators[1].frame = managers.entityManager->Create();
	skillIndicators[0].Image = managers.entityManager->Create();
	skillIndicators[1].Image = managers.entityManager->Create();

	SE::Core::IGUIManager::CreateInfo indicatorInfo;
	indicatorInfo.texture = "Q.png";
	indicatorInfo.textureInfo.posX = 1000;
	indicatorInfo.textureInfo.posY = 600;
	indicatorInfo.textureInfo.height = 100;
	indicatorInfo.textureInfo.width = 100;
	indicatorInfo.textureInfo.layerDepth = 0.9;
	indicatorInfo.textureInfo.anchor = { 0.0f, 0.0f };

	SE::Core::ITextManager::CreateInfo textInfo;
	textInfo.font = "CloisterBlack.spritefont";
	textInfo.info.colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	textInfo.info.layerDepth = 0.9;
	textInfo.info.anchor = { 0.0f, 0.0f };
	textInfo.info.height = indicatorInfo.textureInfo.height;
	textInfo.info.width = indicatorInfo.textureInfo.width;
	textInfo.info.posX = indicatorInfo.textureInfo.posX - 15;
	textInfo.info.posY = indicatorInfo.textureInfo.posY;
	textInfo.info.text = L"";

	managers.guiManager->Create(skillIndicators[0].frame, indicatorInfo);
	managers.guiManager->ToggleRenderableTexture(skillIndicators[0].frame, true);
	managers.textManager->Create(skillIndicators[0].Image, textInfo);
	managers.textManager->ToggleRenderableText(skillIndicators[0].Image, true);

	indicatorInfo.texture = "E.png";
	indicatorInfo.textureInfo.posX = 1150;
	indicatorInfo.textureInfo.posY = 600;

	textInfo.info.text = L"";
	textInfo.info.posX = indicatorInfo.textureInfo.posX - 15;
	textInfo.info.posY = indicatorInfo.textureInfo.posY;

	managers.guiManager->Create(skillIndicators[1].frame, indicatorInfo);
	managers.guiManager->ToggleRenderableTexture(skillIndicators[1].frame, true);
	managers.textManager->Create(skillIndicators[1].Image, textInfo);
	managers.textManager->ToggleRenderableText(skillIndicators[1].Image, true);

#pragma endregion


	player = new PlayerUnit(skills, nullptr,nullptr,5, 10, map);
	player->SetZPosition(0.9f);
	player->PositionEntity(5.5f, 10.5f);


#pragma region Camera stuff
	Core::Entity cam;
	Core::ICameraManager::CreateInfo cInfo;
	cInfo.aspectRatio = CoreInit::subSystems.optionsHandler->GetOptionDouble("Camera", "aspectRatio", (1280.0f / 720.0f));
	cam = managers.entityManager->Create();
	managers.cameraManager->Create(cam, cInfo);
	managers.cameraManager->SetActive(cam);
	

	float cameraRotationX = DirectX::XM_PI / 3;
	float cameraRotationY = DirectX::XM_PI / 3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	player->UpdatePlayerRotation(cameraRotationX, cameraRotationY);
	managers.transformManager->BindChild(player->GetEntity(), cam, false, true);
	managers.transformManager->Move(cam, -5 * cameraTranslation);
	managers.transformManager->SetRotation(cam, cameraRotationX, cameraRotationY, 0);

	//Create a default light

	Core::Entity dummy = managers.entityManager->Create();
	managers.transformManager->Create(dummy, { 0.0f, 1.95f, 0.0f });
	managers.transformManager->BindChild(player->GetEntity(), dummy, false, true);
	//managers.renderableManager->CreateRenderableObject(dummy, { "Placeholder_Block.mesh" });
	//managers.renderableManager->ToggleRenderableObject(dummy, true);

	SE::Core::ILightManager::CreateInfo lightInfo;
	lightInfo.pos = { 0.0f, 0.0f, 0.0f };
	lightInfo.color = { 0.74f, 0.92f, 0.95f };
	lightInfo.radius = 10.5f;
	lightInfo.intensity = 1.3f;
	managers.lightManager->Create(dummy, lightInfo);
	managers.lightManager->ToggleLight(dummy, true);
	managers.lightManager->SetShadowCaster(dummy);


#pragma endregion


#pragma region Sounds
	sounds = managers.entityManager->Create();
	managers.audioManager->Create(sounds, { "Anv‰ndMusen.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "Anv‰ndwasd.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "Sluagh.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "Bodach.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "BraJobbat.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "BraJobbatForts‰tt.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "DetFinns.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "DuKan.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "DuKanOcksÂ.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "FˆremÂl.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "GÂFramTill.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "Glaisdig.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "Nuklavie.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "Och.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "TestaNu.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "TryckNu.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "Utm‰rkt.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "Utm‰rktGeNu.wav", SE::Audio::VoiceSound });
	managers.audioManager->Create(sounds, { "V‰lkommen.wav", SE::Audio::VoiceSound });
#pragma endregion

	BehaviourPointers temp;
	temp.currentRoom = &room;
	temp.player = player;

	projectileManager = new ProjectileManager(temp);



	/*auto weapon = Item::Weapon::Create(WeaponType::SWORD);
	player->AddItem(weapon, 0);*/

	subSystems.window->UpdateTime();
	scriptToRun = &TutorialState::GreetingScript;

//scriptToRun = &TutorialState::SpawnaGlastigScript;

}


SE::Gameplay::TutorialState::~TutorialState()
{
	delete projectileManager;
	delete player;
	delete room;
}

using namespace SE;
using namespace Gameplay;


static const auto GetBasicMovement(PlayerUnit::MovementInput& movement, PlayerUnit::ActionInput& action)
{

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
	managers.cameraManager->WorldSpaceRayFromScreenPos(mX, mY, width, height, rayO, rayD);

	float distance = DirectX::XMVectorGetY(rayO) / -XMVectorGetY(rayD);

	auto clickPos = rayO + rayD*distance;

	movement.mousePosX = DirectX::XMVectorGetX(clickPos);
	movement.mousePosY = DirectX::XMVectorGetZ(clickPos);
}


SE::Gameplay::IGameState::State SE::Gameplay::TutorialState::Update(void *& pass)
{
	float dt = subSystems.window->GetDelta();

	(*this.*scriptToRun)(dt);

	return scriptToRun == &TutorialState::EndTutorialScript ? State::MAIN_MENU_STATE : State::TUTORIAL_STATE;
}

void SE::Gameplay::TutorialState::NothingScript(float dt)
{
}

void SE::Gameplay::TutorialState::NoneScript(float dt)
{
	PlayerUnit::MovementInput movement(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput action;
	std::vector<ProjectileData> newProjectiles;

	GetBasicMovement(movement, action);

	projectileManager->AddProjectiles(newProjectiles);

	projectileManager->UpdateProjectilePositions(subSystems.window->GetDelta());
	room->CheckProjectileCollision(projectileManager->GetAllProjectiles());
	projectileManager->UpdateProjectileActions(subSystems.window->GetDelta());

	room->Update(dt, player->GetXPosition(), player->GetYPosition());


	player->Update(dt, movement, newProjectiles, action);
}

void SE::Gameplay::TutorialState::GreetingScript(float dt)
{
	auto greetingText = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Ancient.spritefont";
	gti.info.text = L"V‰lkommen till Sluagh";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.layerDepth = 0.9;
	//gti.info.scale = { 0.25f ,0.25f };
	managers.textManager->Create(greetingText, gti);
	managers.textManager->ToggleRenderableText(greetingText, true);
	managers.eventManager->SetLifetime(greetingText, 4.5f);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "V‰lkommen.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	
	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		scriptToRun = &TutorialState::WASDScript;
	});

	managers.eventManager->RegisterEntitytoEvent(greetingText, "OnDeath");

	scriptToRun = &TutorialState::NoneScript;

}

void SE::Gameplay::TutorialState::WASDScript(float dt)
{
	auto wasdText = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"ANVƒND w,a,s,d F÷R ATT F÷RFLYTTA DIG I VƒRLDEN";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(wasdText, gti);
	managers.textManager->ToggleRenderableText(wasdText, true);
	managers.eventManager->SetLifetime(wasdText, 8.0f);
	scriptToRun = &TutorialState::NoneScript;
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Anv‰ndwasd.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		scriptToRun = &TutorialState::MouseScript;
	});

	managers.eventManager->RegisterEntitytoEvent(wasdText, "OnDeath");

}

void SE::Gameplay::TutorialState::MouseScript(float dt)
{
	auto mouseText = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"ANVƒND MUSEN F÷R ATT SE DIG OMKRING";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(mouseText, gti);
	managers.textManager->ToggleRenderableText(mouseText, true);
	managers.eventManager->SetLifetime(mouseText, 4.5f);
	scriptToRun = &TutorialState::NoneScript;
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Anv‰ndMusen.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		w = false;
		a = false;
		s = false;
		d = false;
		scriptToRun = &TutorialState::WaitForMovementInputScript;
	});
	managers.eventManager->RegisterEntitytoEvent(mouseText, "OnDeath");
}

void SE::Gameplay::TutorialState::BraJobbatMovementScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"BRA JOBBAT!";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.eventManager->SetLifetime(ent, 4.5f);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "BraJobbat.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		scriptToRun = &TutorialState::GlimmerPickupWeaponScript;
	});
	scriptToRun = &TutorialState::NoneScript;

	managers.eventManager->RegisterEntitytoEvent(ent, "OnDeath");
}

void SE::Gameplay::TutorialState::GlimmerPickupWeaponScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"F÷RM≈L P≈ MARKEN MARKERAS MED ETT GLIMMANDE SKIMMER!";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.eventManager->SetLifetime(ent, 7.0f);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "FˆremÂl.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		scriptToRun = &TutorialState::SpawnPickupWeaponScript;
	});
	scriptToRun = &TutorialState::NoneScript;
	managers.eventManager->RegisterEntitytoEvent(ent, "OnDeath");
}

void SE::Gameplay::TutorialState::SpawnPickupWeaponScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"G≈ FRAM TILL F÷REM≈LET OCH H≈LL IN SHIFT KNAPPEN!";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "GÂFramTill.wav";
	managers.audioManager->PlaySound(sounds, currentSound);

	scriptToRun = &TutorialState::NoneScript;



	auto pe = player->GetEntity();

	Core::IEventManager::EntityEventCallbacks renderItemInfoEC;
	renderItemInfoEC.triggerCheck = [pe](const Core::Entity ent)
	{
		if (!CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO))
			return false;

		if (auto visible = std::get<bool>(CoreInit::managers.dataManager->GetValue(pe, "InfoVisible", false)); visible)
			return false;

		return CoreInit::managers.collisionManager->CheckCollision(ent, pe);
	};

	renderItemInfoEC.triggerCallback = [pe, this,ent](const Core::Entity ent2)
	{
		CoreInit::managers.eventManager->TriggerEvent("StopRenderItemInfo", true);
		CoreInit::subSystems.devConsole->PrintChannel("Debug", "Render");
		Item::RenderItemInfo(ent2, player->GetItemToCompareWith());
		CoreInit::managers.dataManager->SetValue(pe, "InfoVisible", true);
		scriptToRun = &TutorialState::PickupWeaponScript;
		managers.entityManager->Destroy(ent);
	};

	CoreInit::managers.eventManager->RegisterEntityEvent("RenderItemInfo", renderItemInfoEC);
	CoreInit::managers.eventManager->RegisterTriggerEvent("StopRenderItemInfo", [pe](const Core::Entity ent) {
		CoreInit::subSystems.devConsole->PrintChannel("Debug", "StopRender");
		CoreInit::managers.entityManager->Destroy(ent);
		CoreInit::managers.dataManager->SetValue(pe, "InfoVisible", false);
	});

	Core::IEventManager::EntityEventCallbacks pickUpEvent;
	pickUpEvent.triggerCallback = [this](const Core::Entity ent) {


	};



	pickUpEvent.triggerCheck = [pe](const Core::Entity ent) {
		if (CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO))
			if (CoreInit::subSystems.window->ButtonDouble(GameInput::PICKUP))
			{
				return managers.collisionManager->CheckCollision(ent, pe);
			}
		return false;
	};


	managers.eventManager->RegisterEntityEvent("ItemPickup", pickUpEvent);


	auto wep = Item::Weapon::Create(Item::Weapon::Type::SWORD);
	Item::Drop(wep, { 5.5f,0.0f, 10.5f });
}

void SE::Gameplay::TutorialState::PickupWeaponScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"BRA JOBBAT!\nFORTSƒTT H≈LLA IN SHIFT\nOCH DUBBEL KLICKA P≈ 1 F÷R ATT PLOCKA UPP F÷REM≈LET";
	gti.info.screenAnchor = { 0.5f,0.7f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "BraJobbatForts‰tt.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	scriptToRun = &TutorialState::NoneScript;


	auto pe = player->GetEntity();

	Core::IEventManager::EntityEventCallbacks renderItemInfoEC;
	renderItemInfoEC.triggerCheck = [pe](const Core::Entity ent)
	{
		if (!CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO))
			return false;

		if (auto visible = std::get<bool>(CoreInit::managers.dataManager->GetValue(pe, "InfoVisible", false)); visible)
			return false;

		return CoreInit::managers.collisionManager->CheckCollision(ent, pe);
	};
	renderItemInfoEC.triggerCallback = [pe, this](const Core::Entity ent)
	{
		CoreInit::managers.eventManager->TriggerEvent("StopRenderItemInfo", true);
		CoreInit::subSystems.devConsole->PrintChannel("Debug", "Render");
		Item::RenderItemInfo(ent, player->GetItemToCompareWith());
		CoreInit::managers.dataManager->SetValue(pe, "InfoVisible", true);
	};

	CoreInit::managers.eventManager->RegisterEntityEvent("RenderItemInfo", renderItemInfoEC);
	CoreInit::managers.eventManager->RegisterTriggerEvent("StopRenderItemInfo", [pe](const Core::Entity ent) {
		CoreInit::subSystems.devConsole->PrintChannel("Debug", "StopRender");
		CoreInit::managers.entityManager->Destroy(ent);
		CoreInit::managers.dataManager->SetValue(pe, "InfoVisible", false);
	});

	Core::IEventManager::EntityEventCallbacks pickUpEvent;
	pickUpEvent.triggerCallback = [this, ent](const Core::Entity ent2) {

		CoreInit::subSystems.devConsole->Print("Picked up item %s.", std::get<std::string>(managers.dataManager->GetValue(ent2, "Name", "NaN"s)).c_str());

		if (CoreInit::subSystems.window->ButtonDouble(GameInput::ONE))
		{
			player->AddItem(ent2, 0);
			managers.dataManager->SetValue(ent2, "Pickup", true);
			scriptToRun = &TutorialState::Utm‰rktPickupWeaponScript;
			managers.entityManager->Destroy(ent);
			CoreInit::managers.eventManager->TriggerEvent("StopRenderItemInfo", false);
		}


		
	};



	pickUpEvent.triggerCheck = [pe](const Core::Entity ent) {
		if (CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO))
			if (CoreInit::subSystems.window->ButtonDouble(GameInput::PICKUP))
			{
				return managers.collisionManager->CheckCollision(ent, pe);
			}
		return false;
	};


	managers.eventManager->RegisterEntityEvent("ItemPickup", pickUpEvent);

}

void SE::Gameplay::TutorialState::Utm‰rktPickupWeaponScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"UTMƒRKT! HAR DU GJORT DETTA TIDIGARE?\n(PRO TIP: DU KAN OCKS≈ TRYCKA P≈ KNAPPARNA 2,3,4 ELLER 5)";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.eventManager->SetLifetime(ent, 15.0f);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Utm‰rkt.wav";
	managers.audioManager->PlaySound(sounds, currentSound);

	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		scriptToRun = &TutorialState::VisaTaPÂSigVapenScript;
	});

	scriptToRun = &TutorialState::NoneScript;
	managers.eventManager->RegisterEntitytoEvent(ent, "OnDeath");
}

void SE::Gameplay::TutorialState::VisaTaPÂSigVapenScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"TRYCK NU P≈ 1 F÷R ATT GREPPA DITT SVƒRD";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "TryckNu.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	managers.eventManager->RegisterTriggerEvent("DelVisaVapenText", [this](Core::Entity ent) {
		managers.entityManager->Destroy(ent);
	});
	managers.eventManager->RegisterEntitytoEvent(ent, "DelVisaVapenText");
	scriptToRun = &TutorialState::TaPÂSigVapenScript;
}

void SE::Gameplay::TutorialState::TaPÂSigVapenScript(float dt)
{


	PlayerUnit::MovementInput movement(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput action;
	std::vector<ProjectileData> newProjectiles;

	GetBasicMovement(movement, action);
	if (subSystems.window->ButtonPressed(GameInput::ONE))
	{
		action.one = true;
	}



	player->Update(dt, movement, newProjectiles, action);

	if (action.one)
	{
		managers.eventManager->TriggerEvent("DelVisaVapenText", true);
		scriptToRun = &TutorialState::BytaVapenAddInitScript;
	}
}

void SE::Gameplay::TutorialState::BytaVapenAddInitScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"DU KAN BYTA VAPEN GENOM ATT TRYCKA P≈ KNAPPEN 1,2,3,4 ELLER 5\nGREPPA NU DITT TROLLSP÷";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "DuKan.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	managers.eventManager->RegisterTriggerEvent("DelBytVapenText", [this](Core::Entity ent) {
		managers.entityManager->Destroy(ent);
	});
	managers.eventManager->RegisterEntitytoEvent(ent, "DelBytVapenText");

	player->AddItem(Item::Weapon::Create(Item::Weapon::Type::WAND), 1);

	scriptToRun = &TutorialState::BytaVapenAddScript;

}

void SE::Gameplay::TutorialState::BytaVapenAddScript(float dt)
{
	PlayerUnit::MovementInput movement(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput action;
	std::vector<ProjectileData> newProjectiles;

	GetBasicMovement(movement, action);
	if (subSystems.window->ButtonPressed(GameInput::TWO))
	{
		action.two = true;
	}



	player->Update(dt, movement, newProjectiles, action);

	if (action.two)
	{
		managers.eventManager->TriggerEvent("DelBytVapenText", true);
		scriptToRun = &TutorialState::SlÂMedVapenInitScript;
	}
}

void SE::Gameplay::TutorialState::SlÂMedVapenInitScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"UTMƒRKT!\nGE NU DINA VAPEN N≈GRA BRA HUGG GENOM ATT TRYCKA P≈ VƒNSTER MUSKNAPP";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.25f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	hugg = 0;
	managers.eventManager->RegisterTriggerEvent("DelSlÂMedVapenScript", [this](Core::Entity ent) {
		managers.entityManager->Destroy(ent);
		scriptToRun = &TutorialState::NoneScript;
	});
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Utm‰rktGeNu.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	scriptToRun = &TutorialState::SlÂMedVapenScript;
	managers.eventManager->RegisterEntitytoEvent(ent, "DelSlÂMedVapenScript");
}

void SE::Gameplay::TutorialState::SlÂMedVapenScript(float dt)
{
	PlayerUnit::MovementInput movement(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput action;
	std::vector<ProjectileData> newProjectiles;

	GetBasicMovement(movement, action);
	if (subSystems.window->ButtonPressed(GameInput::ACTION))
	{
		action.actionButton = true;
		hugg++;
	}
	if (subSystems.window->ButtonPressed(GameInput::ONE))
	{
		action.one = true;
	}
	else if (subSystems.window->ButtonPressed(GameInput::TWO))
	{
		action.two = true;
	}
	player->Update(dt, movement, newProjectiles, action);


	projectileManager->AddProjectiles(newProjectiles);

	projectileManager->UpdateProjectilePositions(subSystems.window->GetDelta());
	room->CheckProjectileCollision(projectileManager->GetAllProjectiles());
	projectileManager->UpdateProjectileActions(subSystems.window->GetDelta());

	room->Update(dt, player->GetXPosition(), player->GetYPosition());


	if (hugg >= 10)
	{
		managers.eventManager->TriggerEvent("DelSlÂMedVapenScript", true);
		scriptToRun = &TutorialState::TestaSpellInitScript;
	}
}

void SE::Gameplay::TutorialState::TestaSpellInitScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"DU KAN OCKS≈ ANVƒNDA SPECIELLA F÷RM≈GOR\nTESTA ATT TRYCKA P≈ q ELLER e";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "DuKanOcksÂ.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	managers.eventManager->RegisterTriggerEvent("TestaSkillDelText", [this](Core::Entity ent) {
		managers.entityManager->Destroy(ent);
	});
	managers.eventManager->RegisterEntitytoEvent(ent, "TestaSkillDelText");

	scriptToRun = &TutorialState::TestaSpellScript;

}

void SE::Gameplay::TutorialState::TestaSpellScript(float dt)
{
	PlayerUnit::MovementInput movement(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput action;
	std::vector<ProjectileData> newProjectiles;

	GetBasicMovement(movement, action);
	if (subSystems.window->ButtonPressed(GameInput::SKILL1))
	{
		action.skill1Button = true;
	}
	else if (subSystems.window->ButtonPressed(GameInput::SKILL2))
	{
		action.skill2Button = true;
	}

	player->Update(dt, movement, newProjectiles, action);


	projectileManager->AddProjectiles(newProjectiles);

	projectileManager->UpdateProjectilePositions(subSystems.window->GetDelta());
	room->CheckProjectileCollision(projectileManager->GetAllProjectiles());
	projectileManager->UpdateProjectileActions(subSystems.window->GetDelta());

	room->Update(dt, player->GetXPosition(), player->GetYPosition());


	if (action.skill1Button || action.skill2Button)
	{
		managers.eventManager->TriggerEvent("TestaSkillDelText", true);
		scriptToRun = &TutorialState::SpawnaFiendeScript;
	}
}

void SE::Gameplay::TutorialState::SpawnaTr‰ningsDockaScript(float dt)
{
	player->SetGodMode(true);

	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"TESTA NU DINA VAPEN";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);
	managers.eventManager->SetLifetime(ent, 4.5f);

	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		scriptToRun = &TutorialState::SpawnaFiendeScript;
	});
	scriptToRun = &TutorialState::NoneScript;
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "TestaNu.wav";
	managers.audioManager->PlaySound(sounds, currentSound);
	managers.eventManager->RegisterEntitytoEvent(ent, "OnDeath");
}

void SE::Gameplay::TutorialState::SpawnaFiendeScript(float dt)
{
	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"DET FINNS FYRA OLIKA MONSTER I SLOTTET";
	gti.info.screenAnchor = { 0.5f,0.5f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);	
	
	scriptToRun = &TutorialState::NoneScript;

	managers.eventManager->SetLifetime(ent, 8.0f);

	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		scriptToRun = &TutorialState::SpawnaGlastigScript;
	});
	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "DetFinns.wav";
	managers.audioManager->PlaySound(sounds, currentSound);

	managers.eventManager->RegisterEntitytoEvent(ent, "OnDeath");
}

void SE::Gameplay::TutorialState::SpawnaGlastigScript(float dt)
{
	Core::Entity glastig;
	Core::Entity glastigLight;
	glastig = managers.entityManager->Create();
	managers.transformManager->Create(glastig, { 16.5f, 0.5f, 15.5f }, {0, -XM_PIDIV2,0});
	Core::IAnimationManager::CreateInfo aci;
	aci.mesh = "Glaistig.mesh";
	aci.skeleton = "Glaistig.skel";
	aci.animationCount = 0;
	managers.animationManager->CreateAnimatedObject(glastig, aci);
	managers.animationManager->ToggleVisible(glastig, true);
	Core::IMaterialManager::CreateInfo mci;
	mci.materialFile = "Glaistig.mat";
	mci.shader = "SimpleLightPS.hlsl";
	managers.materialManager->Create(glastig, mci);


	glastigLight = managers.entityManager->Create();
	Core::ILightManager::CreateInfo pl;
	pl.pos = { 15.5f, 2.5f, 15.5f };
	pl.color = { 1.0f, 0.85f, 0.85f };
	pl.intensity = 2;
	pl.radius = 3;
	managers.lightManager->Create(glastigLight, pl);
	managers.lightManager->ToggleLight(glastigLight, true);

	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"GLAISTIG";
	gti.info.screenAnchor = { 0.5f,0.25f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(glastig, gti);
	managers.textManager->ToggleRenderableText(glastig, true);

	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Glaisdig.wav";
	managers.audioManager->PlaySound(sounds, currentSound);

	Utilz::GUID anims[] = { "AttackAnim_Glaistig.anim" };
	managers.animationManager->Start(glastig, anims, 1, 2, Core::AnimationFlags::LOOP | Core::AnimationFlags::BLENDTO);
	managers.eventManager->SetLifetime(glastig, 12.0f);

	managers.eventManager->RegisterTriggerEvent("OnDeath", [this, glastigLight](Core::Entity ent) {
		scriptToRun = &TutorialState::SpawnaBodachScript;
		managers.entityManager->Destroy(ent);
		managers.entityManager->Destroy(glastigLight);
	});

	managers.eventManager->RegisterEntitytoEvent(glastig, "OnDeath");

	scriptToRun = &TutorialState::NoneScript;
}

void SE::Gameplay::TutorialState::SpawnaBodachScript(float dt)
{
	Core::Entity bodach;
	Core::Entity bodachLight;
	bodach = managers.entityManager->Create();
	managers.transformManager->Create(bodach, { 16.5f, 0.0f, 12.5f }, { 0, -XM_PIDIV2,0 });
	Core::IAnimationManager::CreateInfo aci;
	aci.mesh = "Bodach.mesh";
	aci.skeleton = "Bodach.skel";
	aci.animationCount = 0;
	managers.animationManager->CreateAnimatedObject(bodach, aci);
	managers.animationManager->ToggleVisible(bodach, true);
	Core::IMaterialManager::CreateInfo mci;
	mci.materialFile = "Bodach.mat";
	mci.shader = "SimpleLightPS.hlsl";
	managers.materialManager->Create(bodach, mci);


	bodachLight = managers.entityManager->Create();
	Core::ILightManager::CreateInfo pl;
	pl.pos = { 15.5f, 1.75f, 12.5f };
	pl.color = { 1.0f, 0.85f, 0.85f };
	pl.intensity = 2;
	pl.radius = 3;
	managers.lightManager->Create(bodachLight, pl);
	managers.lightManager->ToggleLight(bodachLight, true);

	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"BODACH";
	gti.info.screenAnchor = { 0.5f,0.25f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(bodach, gti);
	managers.textManager->ToggleRenderableText(bodach, true);

	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Bodach.wav";
	managers.audioManager->PlaySound(sounds, currentSound);

	Utilz::GUID anims[] = { "IdleAnim_Bodach.anim" };
	managers.animationManager->Start(bodach, anims, 1, 7, Core::AnimationFlags::LOOP | Core::AnimationFlags::BLENDTO);
	managers.eventManager->SetLifetime(bodach, 12.0f);

	managers.eventManager->RegisterTriggerEvent("OnDeath", [this, bodachLight](Core::Entity ent) {
		scriptToRun = &TutorialState::SpawnaNuckelaveeScript;
		managers.entityManager->Destroy(ent);
		managers.entityManager->Destroy(bodachLight);
	});

	managers.eventManager->RegisterEntitytoEvent(bodach, "OnDeath");

	scriptToRun = &TutorialState::NoneScript;
}

void SE::Gameplay::TutorialState::SpawnaNuckelaveeScript(float dt)
{
	Core::Entity nuck;
	Core::Entity nuckLight;
	nuck = managers.entityManager->Create();
	managers.transformManager->Create(nuck, { 16.5f, 0.9f, 10.5f }, { 0, -XM_PIDIV2,0 });
	Core::IAnimationManager::CreateInfo aci;
	aci.mesh = "Nuckelavee.mesh";
	aci.skeleton = "Nuckelavee.skel";
	aci.animationCount = 0;
	managers.animationManager->CreateAnimatedObject(nuck, aci);
	managers.animationManager->ToggleVisible(nuck, true);
	Core::IMaterialManager::CreateInfo mci;
	mci.materialFile = "Nuckelavee.mat";
	mci.shader = "SimpleLightPS.hlsl";
	managers.materialManager->Create(nuck, mci);


	nuckLight = managers.entityManager->Create();
	Core::ILightManager::CreateInfo pl;
	pl.pos = { 15.5f, 1.75f, 10.5f };
	pl.color = { 1.0f, 0.85f, 0.85f };
	pl.intensity = 2;
	pl.radius = 3;
	managers.lightManager->Create(nuckLight, pl);
	managers.lightManager->ToggleLight(nuckLight, true);

	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"NUCKLAVEE";
	gti.info.screenAnchor = { 0.5f,0.25f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(nuck, gti);
	managers.textManager->ToggleRenderableText(nuck, true);

	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Nuklavie.wav";
	managers.audioManager->PlaySound(sounds, currentSound);

	Utilz::GUID anims[] = { "BottomIdleAnim_MCModell.anim","TopIdleAnim_MCModell.anim" };
	managers.animationManager->Start(nuck, anims, 2, 7, Core::AnimationFlags::LOOP | Core::AnimationFlags::BLENDTO);
	managers.eventManager->SetLifetime(nuck, 12.0f);

	managers.eventManager->RegisterTriggerEvent("OnDeath", [this, nuckLight](Core::Entity ent) {
		scriptToRun = &TutorialState::SpawnAndScript;
		managers.entityManager->Destroy(ent);
		managers.entityManager->Destroy(nuckLight);
	});

	managers.eventManager->RegisterEntitytoEvent(nuck, "OnDeath");

	scriptToRun = &TutorialState::NoneScript;
}

void SE::Gameplay::TutorialState::SpawnAndScript(float dt)
{
	delete projectileManager;
	projectileManager = nullptr;
	delete player;
	player = nullptr;
	room->Unload();
	delete room;
	room = nullptr;

	managers.entityManager->Destroy(skillIndicators[0].frame);
	managers.entityManager->Destroy(skillIndicators[1].frame);
	managers.entityManager->Destroy(skillIndicators[0].Image);
	managers.entityManager->Destroy(skillIndicators[1].Image);



	auto ent = managers.entityManager->Create();
	Core::ITextManager::CreateInfo gti;
	gti.font = "Knights.spritefont";
	gti.info.text = L"OCH...";
	gti.info.screenAnchor = { 0.5f,0.25f };
	gti.info.anchor = { 0.5f,0.5f };
	gti.info.scale = { 0.35f ,0.35f };
	gti.info.layerDepth = 0.9;
	managers.textManager->Create(ent, gti);
	managers.textManager->ToggleRenderableText(ent, true);

	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Och.wav";
	managers.audioManager->PlaySound(sounds, currentSound);

	managers.eventManager->SetLifetime(ent, 7.0f);
	subSystems.window->MapActionButton(8731479874, Window::KeySpace);
	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		if(subSystems.window->ButtonDown(8731479874))
			scriptToRun = &TutorialState::NonSuspiciousScript;
		else
			scriptToRun = &TutorialState::GÂTillSluaghSvartScript;
		managers.entityManager->Destroy(ent);
	});

	managers.eventManager->RegisterEntitytoEvent(ent, "OnDeath");
	scriptToRun = &TutorialState::NothingScript;
}

void SE::Gameplay::TutorialState::GÂTillSluaghSvartScript(float dt)
{
	


	Core::ICameraManager::CreateInfo cci;
	cci.aspectRatio = subSystems.optionsHandler->GetOptionDouble("Camera", "aspectRatio", subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 1280)/(double)subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 720));
	cci.fov = 1.570796f;
	auto newCam = managers.entityManager->Create();
	managers.cameraManager->Create(newCam, cci);
	managers.cameraManager->SetActive(newCam);

	auto sluagh = managers.entityManager->Create();

	Core::IMaterialManager::CreateInfo mci;
	mci.materialFile = "Slaughplane.mat";
	mci.shader = "SimpleTexPS.hlsl";
	managers.materialManager->Create(sluagh, mci);

	Core::IAnimationManager::CreateInfo aci;
	aci.mesh = "MCModell.mesh";
	aci.skeleton = "MCModell.skel";
	aci.animationCount = 0;
	
	managers.transformManager->Create(sluagh, { 0.0f,-0.4f, 1.0f }, {0, -XM_PI,0});
	managers.animationManager->CreateAnimatedObject(sluagh, aci);
	managers.animationManager->ToggleVisible(sluagh, true);
	Utilz::GUID anims[] = { "TopSwordAttackAnim_MCModell.anim","BottomIdleAnim_MCModell.anim" };
	managers.animationManager->Start(sluagh, anims, 2, 2, Core::AnimationFlags::IMMEDIATE);

	auto sword = Item::Weapon::Create(Item::Weapon::Type::SWORD);
	Item::Equip(sword, sluagh);

	managers.audioManager->StopSound(sounds, currentSound);
	currentSound = "Sluagh.wav";
	managers.audioManager->PlaySound(sounds, currentSound);

	auto& l = managers.entityManager->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 10.0f, 0.0f };
	d.color = { 1, 1, 1 };
	managers.lightManager->Create(l, d);
	managers.lightManager->ToggleLight(l, true);

	managers.eventManager->SetLifetime(sluagh, 12.0f);


	managers.eventManager->RegisterTriggerEvent("OnDeath", [this, sword, l](Core::Entity ent) {
		scriptToRun = &TutorialState::EndTutorialScript;
		managers.entityManager->Destroy(ent);
		managers.entityManager->Destroy(sword);
		managers.entityManager->Destroy(l);
	});

	managers.eventManager->RegisterEntitytoEvent(sluagh, "OnDeath");


	scriptToRun = &TutorialState::NothingScript;


	
}

void SE::Gameplay::TutorialState::EndTutorialScript(float dt)
{
}

void SE::Gameplay::TutorialState::WaitForMovementInputScript(float dt)
{
	PlayerUnit::MovementInput movement(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput action;
	std::vector<ProjectileData> newProjectiles;

	GetBasicMovement(movement, action);
	if (movement.leftButton)
		a = true;
	if (movement.rightButton)
		d = true;
	if (movement.upButton)
		w = true;
	if (movement.downButton)
		s = true;

	player->Update(dt, movement, newProjectiles, action);

	if (w&&a&&s&&d)
	{
		scriptToRun = &TutorialState::BraJobbatMovementScript;
	}
}

void SE::Gameplay::TutorialState::NonSuspiciousScript(float dt)
{
	auto greeting = managers.entityManager->Create();
	Core::IAudioManager::CreateInfo aci;
	aci.soundFile = "Greeting.wav";
	aci.soundType = Audio::SoundIndexName::BakgroundSound;
	managers.audioManager->Create(greeting, aci);
	managers.audioManager->PlaySound(greeting, "Greeting.wav");

	Core::IGUIManager::CreateInfo gci;
	gci.texture = "Greeting.sei";
	gci.textureInfo.width = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 1280);
	gci.textureInfo.height = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 720);
	managers.guiManager->Create(greeting, gci);
	managers.guiManager->ToggleRenderableTexture(greeting, true);

	managers.eventManager->SetLifetime(greeting, 8.0f);


	managers.eventManager->RegisterTriggerEvent("OnDeath", [this](Core::Entity ent) {
		scriptToRun = &TutorialState::EndTutorialScript;
		managers.entityManager->Destroy(ent);
	});

	managers.eventManager->RegisterEntitytoEvent(greeting, "OnDeath");


	scriptToRun = &TutorialState::NothingScript;
}
