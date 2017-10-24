#include "LightTest.h"
#include <Core\IEngine.h>
#include <Utilz/Timer.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#pragma comment(lib, "windowD.lib")
#else
#pragma comment(lib, "core.lib")
#pragma comment(lib, "window.lib")
#endif

namespace SE
{
	namespace Test
	{
		LightTest::LightTest()
		{

		}
		LightTest::~LightTest()
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
			LightUp,
			LightDown,
			LightLeft,
			LightRight,
			Fullscreen,
			FrameTime,
			ReLight1,
			LiLight1,
			ReLight2,
			LiLight2,
			ReLight3,
			LiLight3,
			ReLight4,
			LiLight4,
			ToggleWire
		};

		bool LightTest::Run(SE::DevConsole::IConsole* console)
		{
			auto engine = Core::CreateEngine();
			engine->Init();
			auto managers = engine->GetManagers();
			auto subSystem = engine->GetSubsystems();

			Core::Entity entity = managers.entityManager->Create();
			const int numEnts = 50;
			Core::Entity ents[numEnts];
			Core::Entity entsTrans[2];
			Core::IMaterialManager::CreateInfo info;
			Utilz::GUID material = Utilz::GUID("MCModell.mat");
			auto shader = Utilz::GUID("SimpleLightPS.hlsl");
			info.shader = shader;
			info.materialFile = material;

			for (int i = 0; i < 2; i++)
			{
				entsTrans[i] = managers.entityManager->Create();
				managers.materialManager->Create(entsTrans[i], info);
				managers.transformManager->Create(entsTrans[i], { (float)(i*3.0f),0.0f,(float)(i * 2.5f - 5.0f) }, { 0.0f,0.0f,0.0f }, { 5.02f,5.02f,5.02f });
				managers.renderableManager->CreateRenderableObject(entsTrans[i], { "MCModell.mesh" });
				managers.renderableManager->ToggleRenderableObject(entsTrans[i], true);
			}

			for (int i = 0; i < numEnts; i++)
			{
				ents[i] = managers.entityManager->Create();
				managers.materialManager->Create(ents[i], info);
				managers.transformManager->Create(ents[i], { (float)(i*3.0f),0.0f,(float)((i * 3) % 2) }, { 0.0f,0.0f,0.0f }, { 5.02f,5.02f,5.02f });
				managers.renderableManager->CreateRenderableObject(ents[i], { "MCModell.mesh" });
				managers.renderableManager->ToggleRenderableObject(ents[i], true);
			}

			Core::Entity light[5];
			for (int i = 0; i < 5; i++)
			{
				light[i] = managers.entityManager->Create();
			}

#pragma region LightDataSet
			Core::ILightManager::CreateInfo data;
			//Light 1
			data.color = DirectX::XMFLOAT3(1.0, 0.1, 0.1);
			data.pos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
			data.radius = 30.0f;
			managers.lightManager->Create(light[0], data);
			managers.lightManager->ToggleLight(light[0], true);

			//Light 2
			data.color = DirectX::XMFLOAT3(0.3, 0.8, 0.2);
			data.pos = DirectX::XMFLOAT3(20.0, 0.0, 0.0);
			data.radius = 10.0;
			managers.lightManager->Create(light[1], data);
			managers.lightManager->ToggleLight(light[1], true);

			//Light 3
			data.color = DirectX::XMFLOAT3(0.2, 0.1, 0.8);
			data.pos = DirectX::XMFLOAT3(40.0, 0.0, 0.0);
			data.radius = 10.0;
			managers.lightManager->Create(light[2], data);
			managers.lightManager->ToggleLight(light[2], true);

			//Light 4
			data.color = DirectX::XMFLOAT3(0.8, 0.1, 0.2);
			data.pos = DirectX::XMFLOAT3(60.0, 0.0, 0.0);
			data.radius = 10.0;
			managers.lightManager->Create(light[3], data);
			managers.lightManager->ToggleLight(light[3], true);

			//Light 5
			data.color = DirectX::XMFLOAT3(0.4, 0.4, 0.4);
			data.pos = DirectX::XMFLOAT3(80.0, -10.0, 0.0);
			data.radius = 150.0;
			managers.lightManager->Create(light[4], data);
			managers.lightManager->ToggleLight(light[4], true);
#pragma endregion LightDataSet

			

			Core::Entity camera = managers.cameraManager->GetActive();
			managers.transformManager->SetRotation(camera, 0.9f, 0.0f, 0.0f);
			managers.transformManager->SetPosition(camera, { 0.0f, 10.0f, -20.0f });

			subSystem.window->MapActionButton(ActionButton::Exit, Window::KeyEscape);
			subSystem.window->MapActionButton(ActionButton::Hide, Window::KeyO);
			subSystem.window->MapActionButton(ActionButton::Show, Window::KeyK);
			subSystem.window->MapActionButton(ActionButton::Up, Window::KeyW);
			subSystem.window->MapActionButton(ActionButton::Down, Window::KeyS);
			subSystem.window->MapActionButton(ActionButton::Left, Window::KeyA);
			subSystem.window->MapActionButton(ActionButton::Right, Window::KeyD);
			subSystem.window->MapActionButton(ActionButton::LightUp, Window::KeyR);
			subSystem.window->MapActionButton(ActionButton::LightDown, Window::KeyC);
			subSystem.window->MapActionButton(ActionButton::LightLeft, Window::KeyQ);
			subSystem.window->MapActionButton(ActionButton::LightRight, Window::KeyE);
			subSystem.window->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
			subSystem.window->MapActionButton(ActionButton::FrameTime, Window::KeyF);
			subSystem.window->MapActionButton(ActionButton::ReLight1, Window::Key1);
			subSystem.window->MapActionButton(ActionButton::LiLight1, Window::Key2);
			subSystem.window->MapActionButton(ActionButton::ReLight2, Window::Key3);
			subSystem.window->MapActionButton(ActionButton::LiLight2, Window::Key4);
			subSystem.window->MapActionButton(ActionButton::ReLight3, Window::Key5);
			subSystem.window->MapActionButton(ActionButton::LiLight3, Window::Key6);
			subSystem.window->MapActionButton(ActionButton::ReLight4, Window::Key7);
			subSystem.window->MapActionButton(ActionButton::LiLight4, Window::Key8);
			subSystem.window->MapActionButton(ActionButton::ToggleWire, Window::KeyT);

			
			bool running = true;
			Utilz::Timer timer;
			bool toggle = true;
			managers.renderableManager->ToggleTransparency(entsTrans[0], true);
			managers.renderableManager->ToggleTransparency(entsTrans[1], true);

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
				if (subSystem.window->ButtonDown(ActionButton::LightUp))
					managers.transformManager->Move(light[0], DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.11f*dt });
				if (subSystem.window->ButtonDown(ActionButton::LightDown))
					managers.transformManager->Move(light[0], DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.11f*dt });
				if (subSystem.window->ButtonDown(ActionButton::LightRight))
					managers.transformManager->Move(light[0], DirectX::XMFLOAT3{ 0.11f*dt, 0.0f, 0.0f });
				if (subSystem.window->ButtonDown(ActionButton::LightLeft))
					managers.transformManager->Move(light[0], DirectX::XMFLOAT3{ -0.11f*dt, 0.0f, 0.0f });
				if (subSystem.window->ButtonDown(ActionButton::ReLight1))
					managers.lightManager->ToggleLight(light[1], false);
				if (subSystem.window->ButtonDown(ActionButton::LiLight1))
					managers.lightManager->ToggleLight(light[1], true);
				if (subSystem.window->ButtonDown(ActionButton::ReLight2))
					managers.lightManager->ToggleLight(light[2], false);
				if (subSystem.window->ButtonDown(ActionButton::LiLight2))
					managers.lightManager->ToggleLight(light[2], true);
				if (subSystem.window->ButtonDown(ActionButton::ReLight3))
					managers.lightManager->ToggleLight(light[3], false);
				if (subSystem.window->ButtonDown(ActionButton::LiLight3))
					managers.lightManager->ToggleLight(light[3], true);
				if (subSystem.window->ButtonDown(ActionButton::ReLight4))
					managers.lightManager->ToggleLight(light[4], false);
				if (subSystem.window->ButtonDown(ActionButton::LiLight4))
					managers.lightManager->ToggleLight(light[4], true);
				if (subSystem.window->ButtonDown(ActionButton::ToggleWire))
				{
					managers.renderableManager->ToggleWireframe(ents[3], !toggle);
					if (toggle == true)
						toggle = false;
					else
						toggle = true;
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
	}
}