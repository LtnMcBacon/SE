#include "MaterialManagerTest.h"
#include <Core\IEngine.h>

#include <Utilz\Timer.h>

SE::Test::MaterialManagerTest::MaterialManagerTest()
{
}


SE::Test::MaterialManagerTest::~MaterialManagerTest()
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
	Fullscreen
};
bool SE::Test::MaterialManagerTest::Run(DevConsole::IConsole * console)
{
	auto engine = Core::CreateEngine();
	engine->Init();
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	Core::Entity entity = managers.entityManager->Create();

	Core::IMaterialManager::CreateInfo info;
	auto material = Utilz::GUID("MCModell.mat");
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.materialFile = material;


	
	managers.materialManager->Create(entity, info);
	auto& camera = managers.entityManager->Create();
	managers.cameraManager->Create(camera);
	managers.cameraManager->SetActive(camera);
	managers.transformManager->SetRotation(camera, 0.9f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(camera, { 0.0f, 10.0f, -20.0f });


	managers.transformManager->Create(entity);

	managers.renderableManager->CreateRenderableObject(entity, { "MCModell.mesh" });
	managers.renderableManager->ToggleRenderableObject(entity, true);

	subSystem.window->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	subSystem.window->MapActionButton(ActionButton::Hide, Window::KeyO);
	subSystem.window->MapActionButton(ActionButton::Show, Window::KeyK);
	subSystem.window->MapActionButton(ActionButton::Up, Window::KeyW);
	subSystem.window->MapActionButton(ActionButton::Down, Window::KeyS);
	subSystem.window->MapActionButton(ActionButton::Left, Window::KeyA);
	subSystem.window->MapActionButton(ActionButton::Right, Window::KeyD);
	subSystem.window->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);

	bool running = true;
	Utilz::Timer timer;

	bool full = subSystem.optionsHandler->GetOptionBool("Window", "fullScreen", 0);
	while (running)
	{
		timer.Tick();
		float dt = timer.GetDelta();
		if (subSystem.window->ButtonPressed(ActionButton::Exit))
			running = false;
		
		if (subSystem.window->ButtonPressed(ActionButton::Hide))
			managers.renderableManager->ToggleRenderableObject(entity, false);
		if (subSystem.window->ButtonPressed(ActionButton::Show))
			managers.renderableManager->ToggleRenderableObject(entity, true);

		if (subSystem.window->ButtonPressed(ActionButton::Fullscreen))
		{
			full = full ? 0 : 1;
			subSystem.optionsHandler->SetOptionBool("Window", "fullScreen", full);
			subSystem.optionsHandler->Trigger();
		}

		if (subSystem.window->ButtonDown(ActionButton::Up))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Down))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Right))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Left))
			managers.transformManager->Move(camera, DirectX::XMFLOAT3{ -0.01f*dt, 0.0f, 0.0f });

		engine->BeginFrame();
		engine->EndFrame();
	}


	engine->Release(); delete engine;

	return true;
}
