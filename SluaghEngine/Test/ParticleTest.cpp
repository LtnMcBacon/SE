#include "ParticleTest.h"
#include <Core\IEngine.h>
#include <Imgui\imgui.h>

bool SE::Test::ParticleTest::Run(SE::DevConsole::IConsole * console)
{
	auto engine = Core::CreateEngine();
	auto res = engine->Init();

	auto subSystems = engine->GetSubsystems();
	auto managers = engine->GetManagers();
//	ImGui::SetCurrentContext((ImGuiContext*)subSystems.devConsole->GetContext());

	auto& camera = managers.entityManager->Create();

	managers.cameraManager->Create(camera);
	managers.cameraManager->SetActive(camera);
	managers.transformManager->SetRotation(camera, 0.0f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(camera, { 0.0f, 1.0f, -5.0f });

	auto frost = managers.entityManager->Create();
	auto fire = managers.entityManager->Create();
	auto torch = managers.entityManager->Create();
	managers.particleSystemManager->CreateSystem(frost, { "frostBall.pts" });
	managers.particleSystemManager->CreateSystem(fire, { "fireBall.txt" });
	managers.particleSystemManager->ToggleVisible(fire, true);
	managers.particleSystemManager->ToggleVisible(frost, true);
	
	Core::IParticleSystemManager::CreateInfo particleInfo;
	particleInfo.systemFile;
	managers.transformManager->Create(frost, {1, 0.5, 1});
	managers.transformManager->Create(fire, { 0, 0, 0});
	managers.renderableManager->CreateRenderableObject(torch, { "Cube.mesh" });
	managers.transformManager->Create(torch);
	managers.transformManager->BindChild(torch, fire);
	managers.renderableManager->ToggleRenderableObject(torch, true);
	managers.transformManager->Move(managers.cameraManager->GetActive(), DirectX::XMFLOAT3{ 0, 0, -5 });
	

	//for (int i = 0; i < 10; i++)
	//{
	//	auto ent = managers.entityManager->Create();
	//	managers.transformManager->Create(ent, { (float)i, 0,0 });
	//	managers.particleSystemManager->CreateSystem(ent, { "torchParticle.pts" });
	//	managers.particleSystemManager->ToggleVisible(ent, true);
	//}

	float pos[3] = {0, 0, 0};
	int i = 0;


	subSystems.window->MapActionButton(0, Window::KeyA);
	subSystems.window->MapActionButton(1, Window::KeyD);

	while (i++ < 2000)
	{
		engine->BeginFrame();
		//ImGui::ProgressBar((float)i / 2000.0f, { 400, 30 }, "Testing");

	/*	if (ImGui::InputFloat3("Pos", pos))
		{
			managers.transformManager->SetPosition(torch, { pos[0], pos[1], pos[2] });

		}*/
		if (subSystems.window->ButtonDown(0))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ -0.01f, 0.0f, 0.0f });
		if (subSystems.window->ButtonDown(1))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.01f, 0.0f, 0.0f });
		engine->EndFrame();
	}

	engine->Release();
	delete engine;
	return true;
}

SE::Test::ParticleTest::ParticleTest()
{
}


SE::Test::ParticleTest::~ParticleTest()
{
}
