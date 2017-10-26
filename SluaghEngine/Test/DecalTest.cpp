#include "DecalTest.h"
#include <Core/IEngine.h>
#include <Imgui/imgui.h>
SE::Test::DecalTest::DecalTest()
{
}

SE::Test::DecalTest::~DecalTest()
{
}

bool SE::Test::DecalTest::Run(DevConsole::IConsole* console)
{
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

	auto em = managers.entityManager;
	auto rm = managers.renderableManager;
	auto cm = managers.cameraManager;
	auto dm = managers.decalManager;
	auto tm = managers.transformManager;
	auto window = subSystem.window;

	Core::Entity camera = em->Create();
	Core::ICameraManager::CreateInfo cmci;
	cmci.aspectRatio = 1280.0f / 720.0f;
	tm->Create(camera, { 0, 0, -10 });
	cm->Create(camera, cmci);
	cm->SetActive(camera);

	Core::Entity box = em->Create();
	tm->Create(box, { 0,0,0 }, { 0,1.48f,0 }, { 5,5,3 });
	Core::IRenderableManager::CreateInfo rmci;
	rmci.meshGUID = "Placeholder_Block.mesh";
	rmci.transparent = false;
	rmci.wireframe = false;
	rm->CreateRenderableObject(box, rmci, false);
	rm->ToggleRenderableObject(box, true);

	Core::Entity decal = em->Create();
	tm->Create(decal, { 0,0, -0.25f });
	dm->Create(decal, "BlackPink.sei");
	
	window->MapActionButton(Window::KeyEscape, Window::KeyEscape);

	bool running = true;
	while(running)
	{
		engine->BeginFrame();
		if (window->ButtonPressed(Window::KeyEscape))
			running = false;
		engine->EndFrame();
	}
	engine->Release();
	delete engine;
	return true;
}
