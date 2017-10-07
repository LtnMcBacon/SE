#include "ConsoleTest.h"
#include <Core\Engine.h>
#include <Utilz\Timer.h>
using namespace SE;
using namespace SE::Core;
SE::Test::ConsoleTest::ConsoleTest()
{
}

SE::Test::ConsoleTest::~ConsoleTest()
{
}

bool SE::Test::ConsoleTest::Run(Utilz::IConsoleBackend * console)
{
	auto& engine = Core::Engine::GetInstance();
	engine.Init();
	auto window = engine.GetWindow();
	auto& em = engine.GetEntityManager();
	auto& tm = engine.GetTransformManager();
	auto& cm = engine.GetCameraManager();
	auto& rm = engine.GetRenderableManager();

	Entity cube = em.Create();
	Entity camera = em.Create();
	tm.Create(cube);
	tm.Create(camera, { 0,0,-5.0f });
	rm.CreateRenderableObject(cube, "Placeholder_Block.mesh");
	rm.ToggleRenderableObject(cube, true);
	CameraBindInfoStruct cbis;
	cbis.aspectRatio = 1280.0f / 720.0f;
	cm.Bind(camera, cbis);
	cm.SetActive(camera);
	

	enum Keys
	{
		EXIT,
		TOGGLE_CONSOLE
	};
	window->MapActionButton(TOGGLE_CONSOLE, Window::Key1);
	window->MapActionButton(EXIT, Window::KeyEscape);
	Utilz::Timer timer;

	bool run = true;
	bool consoleOn = false;
	while (run)
	{
		timer.Tick();
		float dt = timer.GetDelta<std::chrono::milliseconds>();
		engine.BeginFrame();

		if (window->ButtonPressed(TOGGLE_CONSOLE))
			consoleOn = !consoleOn;
		if (window->ButtonPressed(EXIT))
			run = false;
		if (consoleOn)
			ShowExampleAppConsole(&consoleOn);

		{
			static float fov = 0.0f;
		}

		engine.Frame(dt);
	}



	engine.Release();
	return true;
}
