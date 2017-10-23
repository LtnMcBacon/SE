#include "InstancingTest.h"
#include <Core/IEngine.h>
#include <Utilz/Timer.h>
SE::Test::InstancingTest::InstancingTest()
{
}

SE::Test::InstancingTest::~InstancingTest()
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
	FrameTime
};
bool SE::Test::InstancingTest::Run(DevConsole::IConsole* console)
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
		managers.transformManager->Create(ents[i], { (float)(i*3.0f),0.0f,(float)((i * 3) % 2) },{0.0f,0.0f,0.0f},{5.02f,5.02f,5.02f});
		//managers.transformManager->Create(ents[i]);
		managers.renderableManager->CreateRenderableObject(ents[i], { "Placeholder_Block.mesh" });
		managers.renderableManager->ToggleRenderableObject(ents[i], true);

	}

	managers.renderableManager->ToggleRenderableObject(ents[2], false);
	managers.renderableManager->ToggleRenderableObject(ents[2], true);
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

	bool running = true;
	Utilz::Timer timer;

	while (running)
	{
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

		if(subSystem.window->ButtonPressed(ActionButton::Hide))
		{
			for(int i = 0; i< numEnts; i++)
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
		if(subSystem.window->ButtonPressed(ActionButton::FrameTime))
		{
			console->Print("Frametime: %f ms\n", 1.0f / timer.GetDelta());
		}
		engine->BeginFrame();
		engine->EndFrame();
	}


	engine->Release(); delete engine;

	return true;
}
