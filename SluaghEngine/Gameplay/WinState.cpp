#include "WinState.h"
#include "CoreInit.h"
static SE::Core::IEngine::ManagerWrapper man;
static SE::Core::IEngine::Subsystems sys;
SE::Gameplay::WinState::WinState()
{
	man = CoreInit::managers;
	sys = CoreInit::subSystems;
	

	cam = man.entityManager->Create();
	Core::ICameraManager::CreateInfo camc;
	camc.aspectRatio = sys.optionsHandler->GetOptionDouble("Camera", "aspectRatio", 1280.0f / 720.0f);
	man.cameraManager->Create(cam, camc);
	man.cameraManager->SetActive(cam);
	man.transformManager->SetPosition(cam, { 0.0f, 0.0f, -5.0f });

	deadSluagh = man.entityManager->Create();
	Core::IAnimationManager::CreateInfo deadcinfo;
	deadcinfo.mesh = "MCModell.mesh";
	deadcinfo.skeleton = "MCModell.skel";
	Utilz::GUID anims[] = { "DeathAnim_MCModell.anim" };
	deadcinfo.animations = anims;
	man.animationManager->CreateAnimatedObject(deadSluagh, deadcinfo);
	man.animationManager->Start(deadSluagh, anims, 1, 1.0f, Core::AnimationFlags::IMMEDIATE);
	man.animationManager->ToggleVisible(deadSluagh, true);

	auto l = man.entityManager->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 3.0f, 0.0f };
	d.color = { 1, 1, 1 };
	man.lightManager->Create(l, d);
	man.lightManager->ToggleLight(l, true);

	victoryText = man.entityManager->Create();
	Core::ITextManager::CreateInfo textci;
	textci.font = "Knights.spritefont";
	textci.info.text = L"        DU HAR BESEGRAT DEN ONDE SLUAGHEN\nMÅ HEDER OCH ÄRA SKÖLJA ÖVER DIG I STORA MÅTT";
	textci.info.scale = { 0.5f, 0.5f };
	man.textManager->Create(victoryText, textci);
	man.textManager->SetTextPos(victoryText, sys.window->Width() / 2 - 505, sys.window->Height() / 2 + 200);
	man.textManager->ToggleRenderableText(victoryText, true);

	sys.window->MapActionButton(Window::KeyReturn, Window::KeyReturn);
}

SE::Gameplay::WinState::~WinState()
{
	
}

SE::Gameplay::IGameState::State SE::Gameplay::WinState::Update(void*&)
{

	if (sys.window->ButtonPressed(Window::KeyReturn))
		return State::CHARACTER_CREATION_STATE;
	return IGameState::State::WIN_STATE;
}
