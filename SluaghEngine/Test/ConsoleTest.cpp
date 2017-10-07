#include "ConsoleTest.h"
#include <Core\Engine.h>
#include <Utilz\Timer.h>
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

		engine.Frame(dt);
	}



	engine.Release();
	return true;
}
