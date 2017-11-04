#include "RenderableManagerTest.h"
#include <Core\IEngine.h>
#include <Profiler.h>
#include <Utilz\CpuTimeCluster.h>
#include <Utilz\Timer.h>
#include <Imgui\imgui.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

enum ActionButton
{
	Exit,
	Hide,
	Show,
	Up,
	Down,
	Left,
	Right,
	Fullscreen,
	Rise,
	Sink,
	TL
};


SE::Test::RenderableManagerTest::RenderableManagerTest()
{
}


SE::Test::RenderableManagerTest::~RenderableManagerTest()
{
}

bool SE::Test::RenderableManagerTest::Run(DevConsole::IConsole * console)
{
	StartProfile;

	Utilz::CPUTimeCluster timers;


	timers.Start(("Init"));
	auto engine = Core::CreateEngine();
	auto r = engine->Init();
	if (r < 0)
	{
		delete engine;
		return false;
	}
		
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());

	auto& mainC = managers.entityManager->Create();
	auto& camera = managers.entityManager->Create();

	managers.cameraManager->Create(camera);
	managers.cameraManager->SetActive(camera);
	managers.transformManager->SetRotation(camera, 0.0f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(camera, { 0.0f, 1.0f, -5.0f });

	subSystem.window->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	subSystem.window->MapActionButton(ActionButton::Hide, Window::KeyO);
	subSystem.window->MapActionButton(ActionButton::Show, Window::KeyK);
	subSystem.window->MapActionButton(ActionButton::Up, Window::KeyW);
	subSystem.window->MapActionButton(ActionButton::Down, Window::KeyS);
	subSystem.window->MapActionButton(ActionButton::Left, Window::KeyA);
	subSystem.window->MapActionButton(ActionButton::Right, Window::KeyD);
	subSystem.window->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
	subSystem.window->MapActionButton(ActionButton::TL, Window::KeyLeft);

	subSystem.window->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
	subSystem.window->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

	managers.transformManager->Create(mainC);
	managers.transformManager->SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	managers.transformManager->SetRotation(mainC, 0.0f, 3.14f, 0.0f);

	Core::IMaterialManager::CreateInfo info;
	auto material = Utilz::GUID("Run.mat");
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	info.shader = shader;
	info.materialFile = material;

	managers.materialManager->Create(mainC, info);


	managers.renderableManager->CreateRenderableObject(mainC, { "Run.mesh" });
	managers.renderableManager->ToggleRenderableObject(mainC, true);

	auto& l = managers.entityManager->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 5.0f, -5.0f };
	d.color = { 1, 1,1 };
	managers.lightManager->Create(l, d);
	managers.lightManager->ToggleLight(l, true);

	subSystem.window->MapActionButton(0, Window::KeyEscape);

	bool running = true;



	Utilz::Timer timer;
	timers.Stop(("Init"));
	timers.Start(("Running"));
	subSystem.devConsole->Toggle();

	auto e1 = managers.entityManager->Create();
	managers.entityManager->Destroy(e1);

	auto e2 = managers.entityManager->Create();
	managers.entityManager->Destroy(e2);

	auto e3 = managers.entityManager->Create();
	managers.entityManager->Destroy(e3);
	while (running)
	{
		if (subSystem.window->ButtonPressed(0))
			running = false;

		timer.Tick();
		float dt = (float)timer.GetDelta();

		if (subSystem.window->ButtonDown(ActionButton::Up))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3 { 0.0f, 0.0f, 0.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Down))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3 { 0.0f, 0.0f, -0.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Right))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3 { 0.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Left))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3 { -0.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Rise))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3 { 0.0f, -0.01f*dt, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Sink))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3 { 0.0f, 0.01f*dt, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::TL))
			managers.transformManager->Rotate(camera, 0.0f, 0.01f, 0.0f);
		managers.transformManager->Rotate(mainC, 0.0f, 0.01f, 0.0f);
		//managers.transformManager->Move(mainC, { 0.01f, 0.0f, 0.0f });

		engine->BeginFrame();

		if (ImGui::Button("CreateBakedTest"))
		{
			if (!managers.entityManager->Alive(e1))
			{
				e1 = managers.entityManager->Create();
				managers.transformManager->Create(e1);
				managers.transformManager->SetPosition(e1, DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));
				managers.renderableManager->CreateRenderableObject(e1, { "bakedTest.mesh" });
				managers.renderableManager->ToggleRenderableObject(e1, true);
			}		
		}
		if (ImGui::Button("DestroyBakedTest"))
		{
			managers.entityManager->Destroy(e1);
		}	
		if (ImGui::Button("WireframeBakedTest"))
		{
			static bool e1w = true;
			e1w = !e1w;
			managers.renderableManager->ToggleWireframe(e1, e1w);
			
		}
		if (ImGui::Button("CreateBakedTest2"))
		{
			if (!managers.entityManager->Alive(e2))
			{
				e2 = managers.entityManager->Create();
				managers.transformManager->Create(e2);
				managers.transformManager->SetPosition(e2, DirectX::XMFLOAT3(-2.0f, -2.0f, 0.0f));
				managers.renderableManager->CreateRenderableObject(e2, { "bakedTest2.mesh" });
				managers.renderableManager->ToggleRenderableObject(e2, true);
			}
		}
		if (ImGui::Button("DestroyBakedTest2"))
		{
			managers.entityManager->Destroy(e2);
		}

		if (ImGui::Button("CreateBakedTest3"))
		{
			if (!managers.entityManager->Alive(e3))
			{
				e3 = managers.entityManager->Create();
				managers.transformManager->Create(e3);
				managers.transformManager->SetPosition(e3, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f));
				managers.renderableManager->CreateRenderableObject(e3, { "bakedTest3.mesh" });
				managers.renderableManager->ToggleRenderableObject(e3, true);
			}
		}
		if (ImGui::Button("DestroyBakedTest3"))
		{
			managers.entityManager->Destroy(e3);
		}

		engine->EndFrame();
	}

	timers.Stop(("Running"));
		Utilz::TimeMap times;
	timers.GetMap(times);
	engine->GetProfilingInformation(times);
	for (auto& t : times)
		console->Print("%s: %f\n", t.first.c_str(), t.second);

	engine->Release(); delete engine;
	ProfileReturnConst(true);
}
