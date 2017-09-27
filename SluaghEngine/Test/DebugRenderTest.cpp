#include "DebugRenderTest.h"
#include <Core\Engine.h>
#include <Utilz/Timer.h>

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
	Fullscreen
};
bool SE::Test::DebugRenderManagerTest::Run(Utilz::IConsoleBackend * console)
{
	auto& engine = Core::Engine::GetInstance();
	engine.Init(Core::Engine::InitializationInfo());
	auto& em = engine.GetEntityManager();
	auto& mm = engine.GetMaterialManager();
	auto& cm = engine.GetCameraManager();
	auto& tm = engine.GetTransformManager();
	Core::Entity entity = em.Create();

	Core::MaterialManager::CreateInfo info;
	Utilz::GUID textures[] = { Utilz::GUID("dummytex.sei"), Utilz::GUID("purewhite.sei") };
	Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex"), Utilz::GUID("diffuseTexSec") };
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.shaderResourceNames = resourceNames;
	info.textureFileNames = textures;
	info.textureCount = 2;

	auto camera = em.Create();
	cm.Bind(camera);
	cm.SetActive(camera);
	tm.SetRotation(camera, 0.9f, 0.0f, 0.0f);
	tm.SetPosition(camera, { 0.0f, 10.0f, -20.0f });

	mm.Create(entity, info);

	auto& rm = engine.GetRenderableManager();
	auto& drm = engine.GetDebugRenderManager();

	for(int i = 0; i < 100; i++)
	{
		drm.DrawCross(entity, 3.0f, i * 4 - 50, 10, 10);
	}

	tm.Create(entity);

	rm.CreateRenderableObject(entity, Utilz::GUID("Placeholder_level.obj"));
	rm.ToggleRenderableObject(entity, true);

	auto w = engine.GetWindow();


	w->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	w->MapActionButton(ActionButton::Hide, Window::KeyO);
	w->MapActionButton(ActionButton::Show, Window::KeyK);
	w->MapActionButton(ActionButton::Up, Window::KeyW);
	w->MapActionButton(ActionButton::Down, Window::KeyS);
	w->MapActionButton(ActionButton::Left, Window::KeyA);
	w->MapActionButton(ActionButton::Right, Window::KeyD);
	w->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);

	bool running = true;
	Utilz::Timer timer;
	auto& oh = engine.GetOptionHandler();

	int full = oh.GetOption("Window", "fullScreen", 0);
	while (running)
	{
		timer.Tick();
		float dt = timer.GetDeltaMilliseconds();
		if (w->ButtonPressed(ActionButton::Exit))
			running = false;

		if (w->ButtonPressed(ActionButton::Hide))
			rm.ToggleRenderableObject(entity, false);
		if (w->ButtonPressed(ActionButton::Show))
			rm.ToggleRenderableObject(entity, true);

		if (w->ButtonPressed(ActionButton::Fullscreen))
		{
			full = full ? 0 : 1;
			oh.SetOption("Window", "fullScreen", full);
			oh.Trigger();
		}

		if (w->ButtonDown(ActionButton::Up))
			tm.Move(camera, { 0.0f, 0.0f, 0.01f*dt });
		if (w->ButtonDown(ActionButton::Down))
			tm.Move(camera, { 0.0f, 0.0f, -0.01f*dt });
		if (w->ButtonDown(ActionButton::Right))
			tm.Move(camera, { 0.01f*dt, 0.0f, 0.0f });
		if (w->ButtonDown(ActionButton::Left))
			tm.Move(camera, { -0.01f*dt, 0.0f, 0.0f });

		engine.Frame(0.01f);
	}

	engine.Release();

	return true;
}