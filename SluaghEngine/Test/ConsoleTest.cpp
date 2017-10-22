#include "ConsoleTest.h"
#include <Core\IEngine.h>
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
	auto engine = Core::CreateEngine();
	engine->Init();
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	Entity cube = managers.entityManager->Create();
	Entity camera = managers.entityManager->Create();
	managers.transformManager->Create(cube);
	managers.transformManager->Create(camera, { 0,0,-5.0f });
	managers.renderableManager->CreateRenderableObject(cube, { "Placeholder_Block.mesh" });
	managers.renderableManager->ToggleRenderableObject(cube, true);
	managers.collisionManager->CreateBoundingHierarchy(cube, "Placeholder_Block.mesh");
	Core::ICameraManager::CreateInfo  cbis;
	cbis.aspectRatio = 1280.0f / 720.0f;
	managers.cameraManager->Create(camera, cbis);
	managers.cameraManager->SetActive(camera);
	
	

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
	subSystem.window->MapActionButton(TOGGLE_CONSOLE, Window::Key1);
	subSystem.window->MapActionButton(EXIT, Window::KeyEscape);
	subSystem.window->MapActionButton(PRINT_MESSAGE, Window::Key2);
	subSystem.window->MapActionButton(PRINT_ERROR, Window::Key3);
	subSystem.window->MapActionButton(ALLOCATE, Window::KeyP);
	subSystem.window->MapActionButton(DEALLOCATE, Window::KeyO);
	subSystem.window->MapActionButton(CLICK, Window::MouseLeft);
	subSystem.window->MapActionButton(SPAM_MESSAGES, Window::KeyI);
	std::vector<std::string> graphTest;
	Utilz::Timer timer;

	bool run = true;
	bool cameraControls = false;
	bool consoleOn = false;
	//dc.AddCommand([&run](int argc, char** argv) {run = false; }, "exit", "Exits the test");
	//dc.AddCommand([&cameraControls,&dc](int argc, char** argv)
	//{
	//	if (argc == 1)
	//		cameraControls = !cameraControls;
	//	else if(argc == 2)
	//		cameraControls = argv[1][0] == '1';
	//	else
	//		dc.Print("Invalid number of arguments passed to command: " + std::string("camera_control"), "Error");

	//}, "camera_control", "camera_control [0/1]\nTurns the camera controls on or off.\n If no argument is passed, the controls are toggled.");
	//while (run)
	//{
	//	timer.Tick();
	//	float dt = timer.GetDelta();
	//	engine.BeginFrame();

	//	if (subSystem.window->ButtonPressed(TOGGLE_CONSOLE))
	//	{
	//		//consoleOn = !consoleOn;
	//		dc.Toggle();
	//	}
	//	if (subSystem.window->ButtonPressed(PRINT_MESSAGE))
	//		dc.Print("I am a message.\n");
	//	if (subSystem.window->ButtonPressed(PRINT_ERROR))
	//		dc.Print("I am a message.\n", "Error");
	//	if (subSystem.window->ButtonPressed(EXIT))
	//		run = false;
	//	
	//	if (subSystem.window->ButtonPressed(ALLOCATE))
	//	{
	//		graphTest.push_back("");
	//		graphTest.back().resize(1024 * 1024*10, 'a');
	//	}
	//	if (subSystem.window->ButtonPressed(DEALLOCATE))
	//	{
	//		if(graphTest.size())
	//			graphTest.pop_back();
	//	}
	//	if(subSystem.window->ButtonDown(SPAM_MESSAGES))
	//	{
	//		static int spams = 0;
	//		dc.Print("SPAMMALAMMA" + std::to_string(spams++));
	//	}

	//	if(subSystem.window->ButtonPressed(CLICK))
	//	{
	//		int x, y;
	//		DirectX::XMVECTOR dir;
	//		DirectX::XMVECTOR ori;
	//		subSystem.window->GetMousePos(x, y);
	//		managers.cameraManager->WorldSpaceRayFromScreenPos(x, y, subSystem.window->Width(), subSystem.window->Height(), ori, dir);
	//		Entity ent;
	//		if(managers.collisionManager->Pick(ori, dir, ent))
	//		{
	//			std::stringstream ss;
	//			ss << "Picked entity: " << ent.id;
	//			dc.Print(ss.str(), "Picking");
	//		}
	//	}
	//	

	//	if(cameraControls)
	//	{
	//		static CameraBindInfoStruct camInfo;
	//		static float rotation[3] = { 0.0f, 0.0f, 0.0f };
	//		static float translation[3] = { 0.0f, 0.0f, -5.0f };
	//		
	//		ImGui::Begin("Camera settings");
	//		ImGui::SliderFloat("FOV", &camInfo.fov, 0.01f, 3.14f, "%.3f");
	//		ImGui::Text("Translation");
	//		ImGui::DragFloat3("Translation", translation, 0.25f, -10000.0f, 10000.0f, "%.2f");
	//		ImGui::DragFloat3("Rotation", rotation, 6.28f / 360.0f, -3.14f, 3.14f, "%.2f");
	//		ImGui::End();
	//		managers.cameraManager->UpdateCamera(camera, camInfo);
	//		managers.transformManager->SetPosition(camera, { translation[0], translation[1], translation[2] });
	//		managers.transformManager->SetRotation(camera, rotation[0], rotation[1], rotation[2]);
	//		
	//	}

	//	engine->Frame(dt);
	//}


	engine->Release(); delete engine;
	return true;
}
