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
	man.transformManager->SetPosition(cam, { 0.0f, 0.0f, 0.0f });

	deadSluagh = man.entityManager->Create();
	man.transformManager->Create(deadSluagh, { 0.0f, 0.0f, 2.0f }, {0.0f, 3.14f, 0.0f});
	Core::IMaterialManager::CreateInfo info;
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	auto material = Utilz::GUID("Slaughplane.mat");
	info.shader = shader;
	info.materialFile = material;
	man.materialManager->Create(deadSluagh, info);

	Core::IAnimationManager::CreateInfo deadcinfo;
	deadcinfo.mesh = "MCModell.mesh";
	deadcinfo.skeleton = "MCModell.skel";
	Utilz::GUID anims[] = { "DeathAnim_MCModell.anim" };
	deadcinfo.animations = anims;
	man.animationManager->CreateAnimatedObject(deadSluagh, deadcinfo);
	man.animationManager->Start(deadSluagh, anims, 1, 2.0f, Core::AnimationFlags::IMMEDIATE);
	man.animationManager->ToggleVisible(deadSluagh, true);

	sluaghSoul = man.entityManager->Create();
	man.transformManager->Create(sluaghSoul, { 0.0f, 0.0f, 2.0f }, { 0.0f, 3.14f, 0.0f });
	Core::IMaterialManager::CreateInfo info2;
	auto shader2 = Utilz::GUID("SimpleLightTransparentPS.hlsl");
	auto material2 = Utilz::GUID("Slaughplane.mat");
	info.shader = shader2;
	info.materialFile = material2;
	man.materialManager->Create(sluaghSoul, info);

	Core::IAnimationManager::CreateInfo deadcinfo2;
	deadcinfo2.mesh = "MCModell.mesh";
	deadcinfo2.skeleton = "MCModell.skel";
	Utilz::GUID anims2[] = { "DeathAnim_MCModell.anim","TopIdleAnim_MCModell.anim", "BottomIdleAnim_MCModell.anim" };
	deadcinfo2.animations = anims2;
	man.animationManager->CreateAnimatedObject(sluaghSoul, deadcinfo2);
	man.animationManager->Start(sluaghSoul, anims2, 1, 2.0f, Core::AnimationFlags::IMMEDIATE);
	man.animationManager->ToggleTransparency(sluaghSoul, true);
	man.animationManager->ToggleVisible(sluaghSoul, true);
	man.transformManager->SetPosition(sluaghSoul, { 300.0f, 0.0f, 0.0f });

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
	textci.info.anchor = { 0.5f, 0.5f };
	textci.info.screenAnchor = { 0.5f, 0.8f };
	man.textManager->Create(victoryText, textci);
	man.textManager->ToggleRenderableText(victoryText, true);

	infoText = man.entityManager->Create();
	textci.info.text = L"TRYCK PÅ RETUR FÖR ATT GÅ TILLBAKA TILL MENYN";
	textci.info.scale = { 0.25f, 0.25f };
	textci.info.colour = { 0.2f,0.8f,0.2f,1.0f };
	textci.info.anchor = { 0.5f, 0.5f };
	textci.info.screenAnchor = { 0.5f, 0.9f };

	man.textManager->Create(infoText, textci);
	man.textManager->ToggleRenderableText(infoText, true);

	sys.window->MapActionButton(Window::KeyReturn, Window::KeyReturn);
	stateToReturn = State::WIN_STATE;
	timer.Tick();
	totTime = 0.0f;
	hack = true;
}

SE::Gameplay::WinState::~WinState()
{
	
}

SE::Gameplay::IGameState::State SE::Gameplay::WinState::Update(void*&)
{
	timer.Tick();
	float dt = timer.GetDelta<std::ratio<1,1>>();
	totTime += dt;
	
	if(totTime > 2.0f)
	{
			
		if(hack)
		{
			man.transformManager->SetPosition(sluaghSoul, man.transformManager->GetPosition(deadSluagh));
			Utilz::GUID an[] = { "TopIdleAnim_MCModell.anim", "BottomIdleAnim_MCModell.anim" };
			man.animationManager->Start(sluaghSoul, an, 2, 5.0f, Core::AnimationFlags::BLENDTO | Core::AnimationFlags::LOOP);
			hack = false;			
		}

		man.transformManager->Move(sluaghSoul, DirectX::XMFLOAT3(0.0f, 0.5f * dt, 0.0f));

	}
	if (sys.window->ButtonPressed(Window::KeyReturn))
		return State::CHARACTER_CREATION_STATE;
	return stateToReturn;
}
