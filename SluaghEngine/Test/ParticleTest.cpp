#include "ParticleTest.h"
#include <Core\IEngine.h>
#include <Imgui\imgui.h>

bool SE::Test::ParticleTest::Run(SE::DevConsole::IConsole * console)
{
	auto engine = Core::CreateEngine();
	engine->Init();

	auto subSystems = engine->GetSubsystems();
	auto managers = engine->GetManagers();
	ImGui::SetCurrentContext((ImGuiContext*)subSystems.devConsole->GetContext());
	auto frost = managers.entityManager->Create();
	auto fire = managers.entityManager->Create();
	auto torch = managers.entityManager->Create();
	Core::IParticleSystemManager::CreateInfo particleInfo;
	particleInfo.systemFile;
	managers.transformManager->Create(frost, {1, 0.5, 1});
	managers.transformManager->Create(fire, { 0, 0, 0});
	managers.renderableManager->CreateRenderableObject(torch, { "Cube.mesh" });
	managers.transformManager->Create(torch);
	managers.transformManager->BindChild(torch, fire);
	managers.renderableManager->ToggleRenderableObject(torch, true);
	managers.particleSystemManager->CreateSystem(frost, { "frostBall.pts" });
	managers.particleSystemManager->CreateSystem(fire, { "fireBall.txt" });
	Core::ICameraManager::CreateInfo camInfo;
	camInfo.posistion = { 0.0f, 0.0f, -20.0f };
	
	managers.transformManager->Move(managers.cameraManager->GetActive(), DirectX::XMFLOAT3{ 0, 0, -5 });
	
	float pos[3] = {0, 0, 0};
	subSystems.window->MapActionButton(Window::KeyEscape, Window::KeyEscape);
	while (!subSystems.window->ButtonPressed(Window::KeyEscape))
	{
		engine->BeginFrame();

		if (ImGui::InputFloat3("Pos", pos))
		{
			managers.transformManager->SetPosition(torch, { pos[0], pos[1], pos[2] });

		}

		engine->EndFrame();
	}

	engine->Release();
	delete engine;
	return false;
}

SE::Test::ParticleTest::ParticleTest()
{
}


SE::Test::ParticleTest::~ParticleTest()
{
}
