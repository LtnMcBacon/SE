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
			static CameraBindInfoStruct camInfo;
			static float rotation[3] = { 0.0f, 0.0f, 0.0f };
			static float translation[3] = { 0.0f, 0.0f, -5.0f };
			
			ImGui::Begin("Camera settings");
			ImGui::SliderFloat("FOV", &camInfo.fov, 0.01f, 3.14f, "%.3f");
			ImGui::Text("Translation");
			ImGui::DragFloat3("Translation", translation, 0.25f, -10000.0f, 10000.0f, "%.2f");
			ImGui::DragFloat3("Rotation", rotation, 6.28f / 360.0f, -3.14f, 3.14f, "%.2f");
			ImGui::End();
			cm.UpdateCamera(camera, camInfo);
			tm.SetPosition(camera, { translation[0], translation[1], translation[2] });
			tm.SetRotation(camera, rotation[0], rotation[1], rotation[2]);
			
		}

		engine.Frame(dt);
	}



	engine.Release();
	return true;
}
