#include "ConsoleTest.h"
#include <Core\Engine.h>
#include <Utilz\Timer.h>
#include <sstream>
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
	auto& dc = engine.GetDevConsole();
	auto& com = engine.GetCollisionManager();

	Entity cube = em.Create();
	Entity camera = em.Create();
	tm.Create(cube);
	tm.Create(camera, { 0,0,-5.0f });
	rm.CreateRenderableObject(cube, "Placeholder_Block.mesh");
	rm.ToggleRenderableObject(cube, true);
	com.CreateBoundingHierarchy(cube, "Placeholder_Block.mesh");
	CameraBindInfoStruct cbis;
	cbis.aspectRatio = 1280.0f / 720.0f;
	cm.Bind(camera, cbis);
	cm.SetActive(camera);
	
	

	enum Keys
	{
		EXIT,
		TOGGLE_CONSOLE,
		PRINT_MESSAGE,
		PRINT_ERROR,
		ALLOCATE,
		DEALLOCATE,
		CLICK,
		SPAM_MESSAGES
	};
	window->MapActionButton(TOGGLE_CONSOLE, Window::Key1);
	window->MapActionButton(EXIT, Window::KeyEscape);
	window->MapActionButton(PRINT_MESSAGE, Window::Key2);
	window->MapActionButton(PRINT_ERROR, Window::Key3);
	window->MapActionButton(ALLOCATE, Window::KeyP);
	window->MapActionButton(DEALLOCATE, Window::KeyO);
	window->MapActionButton(CLICK, Window::MouseLeft);
	window->MapActionButton(SPAM_MESSAGES, Window::KeyI);
	std::vector<std::string> graphTest;
	Utilz::Timer timer;

	bool run = true;
	bool cameraControls = false;
	bool consoleOn = false;
	dc.AddCommand([&run](int argc, char** argv) {run = false; }, "exit", "Exits the test");
	dc.AddCommand([&cameraControls,&dc](int argc, char** argv)
	{
		if (argc == 1)
			cameraControls = !cameraControls;
		else if(argc == 2)
			cameraControls = argv[1][0] == '1';
		else
			dc.Print("Invalid number of arguments passed to command: " + std::string("camera_control"), "Error");

	}, "camera_control", "camera_control [0/1]\nTurns the camera controls on or off.\n If no argument is passed, the controls are toggled.");
	while (run)
	{
		timer.Tick();
		float dt = timer.GetDelta();
		engine.BeginFrame();

		if (window->ButtonPressed(TOGGLE_CONSOLE))
		{
			//consoleOn = !consoleOn;
			dc.Toggle();
		}
		if (window->ButtonPressed(PRINT_MESSAGE))
			dc.Print("I am a message.\n");
		if (window->ButtonPressed(PRINT_ERROR))
			dc.Print("I am a message.\n", "Error");
		if (window->ButtonPressed(EXIT))
			run = false;
		
		if (window->ButtonPressed(ALLOCATE))
		{
			graphTest.push_back("");
			graphTest.back().resize(1024 * 1024*10, 'a');
		}
		if (window->ButtonPressed(DEALLOCATE))
		{
			if(graphTest.size())
				graphTest.pop_back();
		}
		if(window->ButtonDown(SPAM_MESSAGES))
		{
			static int spams = 0;
			dc.Print("SPAMMALAMMA" + std::to_string(spams++));
		}

		if(window->ButtonPressed(CLICK))
		{
			int x, y;
			DirectX::XMVECTOR dir;
			DirectX::XMVECTOR ori;
			window->GetMousePos(x, y);
			cm.WorldSpaceRayFromScreenPos(x, y, window->Width(), window->Height(), ori, dir);
			Entity ent;
			if(com.Pick(ori, dir, ent))
			{
				std::stringstream ss;
				ss << "Picked entity: " << ent.id;
				dc.Print(ss.str(), "Picking");
			}
		}
		

		if(cameraControls)
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
