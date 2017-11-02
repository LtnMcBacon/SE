#include "DebugRenderTest.h"
#include <Core\IEngine.h>
#include <Utilz/Timer.h>
#include <Imgui/imgui.h>
SE::Test::DebugRenderManagerTest::DebugRenderManagerTest()
{
}

SE::Test::DebugRenderManagerTest::~DebugRenderManagerTest()
{
}

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
	FrameTime,
	Jiggle,
	RemoveStuff,
	Console
};
bool SE::Test::DebugRenderManagerTest::Run(DevConsole::IConsole * console)
{
	auto engine = Core::CreateEngine();
	engine->Init();
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	Core::Entity entity = managers.entityManager->Create();
	const int numEnts = 600;
	Core::Entity ents[numEnts];
	Core::IMaterialManager::CreateInfo info;
	Utilz::GUID material = Utilz::GUID("MCModell.mat");
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.materialFile = material;
	for (int i = 0; i < numEnts; i++)
	{
		ents[i] = managers.entityManager->Create();
		managers.materialManager->Create(ents[i], info);
		managers.transformManager->Create(ents[i], { (float)(i*5.0f),0.0f,0.0f }, { 0.0f,3.14f,0.0f }, { 4.0f,4.0f,4.0f });
		//managers.transformManager->Create(ents[i]);
		managers.renderableManager->CreateRenderableObject(ents[i], { "Placeholder_Block.mesh" });
		managers.renderableManager->ToggleRenderableObject(ents[i], true);
		managers.collisionManager->CreateBoundingHierarchy(ents[i], "Placeholder_Block.mesh");
		managers.debugRenderManager->ToggleDebugRendering(ents[i], true);
	//	managers.debugRenderManager->DrawCross(ents[i], 1.0f);

	}

	const auto camera = managers.entityManager->Create();
	managers.cameraManager->Create(camera);
	managers.cameraManager->SetActive(camera);
	managers.transformManager->SetRotation(camera, 0.9f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(camera, { 0.0f, 10.0f, -20.0f });

	subSystem.window->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	subSystem.window->MapActionButton(ActionButton::Hide, Window::KeyO);
	subSystem.window->MapActionButton(ActionButton::Show, Window::KeyK);
	subSystem.window->MapActionButton(ActionButton::Up, Window::KeyW);
	subSystem.window->MapActionButton(ActionButton::Down, Window::KeyS);
	subSystem.window->MapActionButton(ActionButton::Left, Window::KeyA);
	subSystem.window->MapActionButton(ActionButton::Right, Window::KeyD);
	subSystem.window->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
	subSystem.window->MapActionButton(ActionButton::FrameTime, Window::KeyF);
	subSystem.window->MapActionButton(ActionButton::Jiggle, Window::KeyJ);
	subSystem.window->MapActionButton(RemoveStuff, Window::KeyR);
	subSystem.window->MapActionButton(Console, Window::KeyC);
	

	subSystem.devConsole->Show();
	Utilz::Timer timer;
	uint32_t removeIndex = 0;
	bool showConsole = false;
	bool running = true;
	while (running)
	{
		engine->BeginFrame();
		
		timer.Tick();
		float dt = timer.GetDelta();
		if (subSystem.window->ButtonPressed(ActionButton::Exit))
			running = false;


		if (subSystem.window->ButtonDown(ActionButton::Up))

			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.11f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Down))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.11f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Right))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.11f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Left))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ -0.11f*dt, 0.0f, 0.0f });


		if (subSystem.window->ButtonPressed(ActionButton::Jiggle))
			managers.transformManager->Move(ents[3], DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f });

		if (removeIndex < numEnts && subSystem.window->ButtonDown(RemoveStuff))
			managers.entityManager->Destroy(ents[removeIndex++]);


		if (subSystem.window->ButtonPressed(ActionButton::Console))
		{
			showConsole = !showConsole;
		}

		if (showConsole)
		{
			//ImGui::Text("I am the console.\n");
		}


		if (subSystem.window->ButtonPressed(ActionButton::Hide))
		{
			for (int i = 0; i< numEnts; i++)
			{
				managers.renderableManager->ToggleRenderableObject(ents[i], false);
			}
		}
		if (subSystem.window->ButtonPressed(ActionButton::Show))
		{
			for (int i = 0; i< numEnts; i++)
			{
				managers.renderableManager->ToggleRenderableObject(ents[i], true);
			}
		}
		if (subSystem.window->ButtonPressed(ActionButton::FrameTime))
		{
			console->Print("Frametime: %f ms\n", timer.GetDelta());
		}
		engine->BeginFrame();
		engine->EndFrame();
	}


	engine->Release(); delete engine;

	return true;
}
