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
			Fullscreen,
			FrameTime
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
				rm.CreateRenderableObject(ents[i], Utilz::GUID("pCube1_Placeholder_Block.mesh"));
				rm.ToggleRenderableObject(ents[i], true);

			}

			auto light = em.Create();
			transformManager.Create(light);

			Graphics::LightData data;
			data.colour = DirectX::XMFLOAT4(1.0, 0.1, 0.1, 1.0);
			data.pos = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 30.0);

			lightManager.AddLight(light, data);
			lightManager.ToggleLight(light, true);
			//lightManager.RemoveLight(ent);

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
			w->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
			w->MapActionButton(ActionButton::FrameTime, Window::KeyF);

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


				if (w->ButtonDown(ActionButton::Up))
					transformManager.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.11f*dt });
				if (w->ButtonDown(ActionButton::Down))
					transformManager.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.11f*dt });
				if (w->ButtonDown(ActionButton::Right))
					transformManager.Move(camera, DirectX::XMFLOAT3{ 0.11f*dt, 0.0f, 0.0f });
				if (w->ButtonDown(ActionButton::Left))
					transformManager.Move(camera, DirectX::XMFLOAT3{ -0.11f*dt, 0.0f, 0.0f });

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
					console->Print("Frametime: %f ms\n", 1.0f / timer.GetDeltaMilliseconds());
				}
				engine.Frame(0.01f);
			}


			engine.Release();
			return true;
		}
	}
}