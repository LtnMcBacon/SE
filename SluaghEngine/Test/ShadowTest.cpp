
#include "ShadowTest.h"
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


SE::Test::ShadowTest::ShadowTest() {


}


SE::Test::ShadowTest::~ShadowTest() {


}

bool SE::Test::ShadowTest::Run(DevConsole::IConsole* console) {

	StartProfile;

	Utilz::CPUTimeCluster timers;

	timers.Start(CREATE_ID_HASH("Init"));
	auto engine = Core::CreateEngine();
	auto r = engine->Init();
	if (r < 0)
	{
		delete engine;
		return false;
	}

	// Get managers and subsystem
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());

	// Create entities
	auto& mainC = managers.entityManager->Create();
	auto& floor = managers.entityManager->Create();
	auto& camera = managers.entityManager->Create();

	// Create camera
	managers.cameraManager->Create(camera);
	managers.cameraManager->SetActive(camera);

	// Set camera transformations
	managers.transformManager->SetRotation(camera, 0.0f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(camera, { 0.0f, 1.0f, -5.0f });

	// Map keyboard buttons
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

	// Create transformation for character and floor
	managers.transformManager->Create(mainC);
	managers.transformManager->Create(floor);

	managers.transformManager->SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	managers.transformManager->SetRotation(mainC, 0.0f, 3.14f, 0.0f);

	managers.transformManager->SetPosition(floor, DirectX::XMFLOAT3(0.0f, -1.0f, 5.0));
	managers.transformManager->SetRotation(floor, -DirectX::XM_PI / 2, 0.0f, 0.0f);
	managers.transformManager->SetScale(floor, DirectX::XMFLOAT3(30.0f, 1.0f, 30.0));

	// Create material for character
	Core::IMaterialManager::CreateInfo info;
	auto material = Utilz::GUID("Placeholder_Block.mat");
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	info.shader = shader;
	info.materialFile = material;

	// Create material for floor
	Core::IMaterialManager::CreateInfo floorInfo;
	auto floorMaterial = Utilz::GUID("floorTest.mat");
	floorInfo.shader = shader;
	floorInfo.materialFile = floorMaterial;

	managers.materialManager->Create(mainC, info);
	managers.materialManager->Create(floor, floorInfo);

	// Create renderable objects from the entities
	managers.renderableManager->CreateRenderableObject(mainC, { "Placeholder_Block.mesh" });
	managers.renderableManager->ToggleRenderableObject(mainC, true);
	managers.renderableManager->ToggleShadow(mainC, true);

	managers.renderableManager->CreateRenderableObject(floor, { "floorTest.mesh" });
	managers.renderableManager->ToggleRenderableObject(floor, true);

	// Create light
	auto& l = managers.entityManager->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 0.0f, -5.0f };
	d.color = { 1, 1, 1 };
	d.dir = { 0.0f, 0.0f, 1.0f };
	d.castShadow = true;
	d.isOrtographic = false;

	managers.lightManager->Create(l, d);
	managers.lightManager->ToggleLight(l, true);

	subSystem.window->MapActionButton(0, Window::KeyEscape);

	bool running = true;

	Utilz::Timer timer;
	timers.Stop(CREATE_ID_HASH("Init"));
	timers.Start(CREATE_ID_HASH("Running"));
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
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Down))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Right))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Left))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ -0.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Rise))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, -0.01f*dt, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Sink))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.01f*dt, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::TL))
			managers.transformManager->Rotate(camera, 0.0f, 0.01f, 0.0f);
		//managers.transformManager->Rotate(mainC, 0.0f, 0.01f, 0.0f);
		//managers.transformManager->Move(mainC, { 0.01f, 0.0f, 0.0f });

		engine->BeginFrame();

		engine->EndFrame();
	}

	timers.Stop(CREATE_ID_HASH("Running"));

	Utilz::TimeMap times;
	timers.GetMap(times);
	engine->GetProfilingInformation(times);
	for (auto& t : times)
		console->Print("%s: %f\n", t.first.str, t.second);

	engine->Release(); delete engine;
	ProfileReturnConst(true);
}