#include "LightTest.h"
#include <Core\Engine.h>
#include <Utilz/Timer.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#pragma comment(lib, "windowD.lib")
#pragma comment(lib, "gameplayD.lib")
#else
#pragma comment(lib, "core.lib")
#pragma comment(lib, "window.lib")
#pragma comment(lib, "gameplay.lib")
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
		};

		bool LightTest::Run(SE::Utilz::IConsoleBackend* console)
		{
			auto& engine = Core::Engine::GetInstance();
			engine.Init(Core::Engine::InitializationInfo());
			auto& em = engine.GetEntityManager();
			auto& mm = engine.GetMaterialManager();
			auto& cm = engine.GetCameraManager();
			auto& rm = engine.GetRenderableManager();
			auto& lightManager = engine.GetLightManager();
			auto& transformManager = engine.GetTransformManager();

			Core::Entity entity = em.Create();
			const int numEnts = 50;
			Core::Entity ents[numEnts];
			Core::MaterialManager::CreateInfo info;
			Utilz::GUID textures[] = { Utilz::GUID("TestMesh_Diffuse.sei"), Utilz::GUID("purewhite.sei") };
			Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex"), Utilz::GUID("diffuseTexSec") };
			auto shader = Utilz::GUID("SimpleLightPS.hlsl");
			info.shader = shader;
			info.shaderResourceNames = resourceNames;
			info.textureFileNames = textures;
			info.textureCount = 2;
			for (int i = 0; i < numEnts; i++)
			{
				ents[i] = em.Create();
				mm.Create(ents[i], info);
				transformManager.Create(ents[i], { (float)(i*3.0f),0.0f,(float)((i * 3) % 2) }, { 0.0f,0.0f,0.0f }, { 5.02f,5.02f,5.02f });
				//tm.Create(ents[i]);
				rm.CreateRenderableObject(ents[i], Utilz::GUID("MCModell.mesh"));
				rm.ToggleRenderableObject(ents[i], true);
			}

			Core::Entity light[5];
			for (int i = 0; i < 5; i++)
			{
				light[i] = em.Create();
				transformManager.Create(light[i], DirectX::XMFLOAT3(20.0 * i, 0.0, 0.0));
			}

#pragma region LightDataSet
			Graphics::LightData data;
			//Light 1
			data.colour = DirectX::XMFLOAT4(1.0, 0.1, 0.1, 1.0);
			data.pos = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 30.0);
			lightManager.AddLight(light[0], data);
			lightManager.ToggleLight(light[0], true);

			//Light 2
			data.colour = DirectX::XMFLOAT4(0.3, 0.8, 0.2, 1.0);
			data.pos = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 10.0);
			lightManager.AddLight(light[1], data);
			lightManager.ToggleLight(light[1], true);

			//Light 3
			data.colour = DirectX::XMFLOAT4(0.2, 0.1, 0.8, 1.0);
			data.pos = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 10.0);
			lightManager.AddLight(light[2], data);
			lightManager.ToggleLight(light[2], true);

			//Light 4
			data.colour = DirectX::XMFLOAT4(0.8, 0.1, 0.2, 1.0);
			data.pos = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 10.0);
			lightManager.AddLight(light[3], data);
			lightManager.ToggleLight(light[3], true);

			//Light 5
			data.colour = DirectX::XMFLOAT4(0.1, 0.8, 0.3, 1.0);
			data.pos = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 10.0);
			lightManager.AddLight(light[4], data);
			lightManager.ToggleLight(light[4], true);
#pragma endregion LightDataSet

			

			const auto camera = em.Create();
			cm.Bind(camera);
			cm.SetActive(camera);
			transformManager.SetRotation(camera, 0.9f, 0.0f, 0.0f);
			transformManager.SetPosition(camera, { 0.0f, 10.0f, -20.0f });

			auto w = engine.GetWindow();


			w->MapActionButton(ActionButton::Exit, Window::KeyEscape);
			w->MapActionButton(ActionButton::Hide, Window::KeyO);
			w->MapActionButton(ActionButton::Show, Window::KeyK);
			w->MapActionButton(ActionButton::Up, Window::KeyW);
			w->MapActionButton(ActionButton::Down, Window::KeyS);
			w->MapActionButton(ActionButton::Left, Window::KeyA);
			w->MapActionButton(ActionButton::Right, Window::KeyD);
			w->MapActionButton(ActionButton::LightUp, Window::KeyR);
			w->MapActionButton(ActionButton::LightDown, Window::KeyC);
			w->MapActionButton(ActionButton::LightLeft, Window::KeyQ);
			w->MapActionButton(ActionButton::LightRight, Window::KeyE);
			w->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
			w->MapActionButton(ActionButton::FrameTime, Window::KeyF);
			w->MapActionButton(ActionButton::ReLight1, Window::Key1);
			w->MapActionButton(ActionButton::LiLight1, Window::Key2);
			w->MapActionButton(ActionButton::ReLight2, Window::Key3);
			w->MapActionButton(ActionButton::LiLight2, Window::Key4);
			w->MapActionButton(ActionButton::ReLight3, Window::Key5);
			w->MapActionButton(ActionButton::LiLight3, Window::Key6);
			w->MapActionButton(ActionButton::ReLight4, Window::Key7);
			w->MapActionButton(ActionButton::LiLight4, Window::Key8);

			bool running = true;
			Utilz::Timer timer;
			auto& oh = engine.GetOptionHandler();

			int full = oh.GetOption("Window", "fullScreen", 0);
			while (running)
			{
				timer.Tick();
				float dt = timer.GetDelta();
				if (w->ButtonPressed(ActionButton::Exit))
					running = false;


				if (w->ButtonDown(ActionButton::Up))
					transformManager.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.11f*dt });
				if (w->ButtonDown(ActionButton::Down))
					transformManager.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.11f*dt });
				if (w->ButtonDown(ActionButton::Right))
					transformManager.Move(camera, DirectX::XMFLOAT3{ 0.11f*dt, 0.0f, 0.0f });
				if (w->ButtonDown(ActionButton::Left))
					transformManager.Move(camera, DirectX::XMFLOAT3{ -0.11f*dt, 0.0f, 0.0f });
				if (w->ButtonDown(ActionButton::LightUp))
					transformManager.Move(light[0], DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.11f*dt });
				if (w->ButtonDown(ActionButton::LightDown))
					transformManager.Move(light[0], DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.11f*dt });
				if (w->ButtonDown(ActionButton::LightRight))
					transformManager.Move(light[0], DirectX::XMFLOAT3{ 0.11f*dt, 0.0f, 0.0f });
				if (w->ButtonDown(ActionButton::LightLeft))
					transformManager.Move(light[0], DirectX::XMFLOAT3{ -0.11f*dt, 0.0f, 0.0f });
				if (w->ButtonDown(ActionButton::ReLight1))
					lightManager.ToggleLight(light[1], false);
				if (w->ButtonDown(ActionButton::LiLight1))
					lightManager.ToggleLight(light[1], true);
				if (w->ButtonDown(ActionButton::ReLight2))
					lightManager.ToggleLight(light[2], false);
				if (w->ButtonDown(ActionButton::LiLight2))
					lightManager.ToggleLight(light[2], true);
				if (w->ButtonDown(ActionButton::ReLight3))
					lightManager.ToggleLight(light[3], false);
				if (w->ButtonDown(ActionButton::LiLight3))
					lightManager.ToggleLight(light[3], true);
				if (w->ButtonDown(ActionButton::ReLight4))
					lightManager.ToggleLight(light[4], false);
				if (w->ButtonDown(ActionButton::LiLight4))
					lightManager.ToggleLight(light[4], true);

				if (w->ButtonPressed(ActionButton::Hide))
				{
					for (int i = 0; i< numEnts; i++)
					{
						rm.ToggleRenderableObject(ents[i], false);
					}
				}
				if (w->ButtonPressed(ActionButton::Show))
				{
					for (int i = 0; i< numEnts; i++)
					{
						rm.ToggleRenderableObject(ents[i], true);
					}
				}
				if (w->ButtonPressed(ActionButton::FrameTime))
				{
					console->Print("Frametime: %f ms\n", timer.GetDelta());
				}
				engine.Frame(0.01f);
			}


			engine.Release();
			return true;
		}
	}
}