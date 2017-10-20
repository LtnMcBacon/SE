#include <Utilz\Timer.h>
#include "BloomTest.h"
#include <Core\Engine.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

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
	Rise,
	Sink,
	TL,
	Console
};

namespace SE
{
	namespace Test
	{
		BloomTest::BloomTest()
		{
		}


		BloomTest::~BloomTest()
		{
		}

		bool BloomTest::Run(Utilz::IConsoleBackend* console)
		{
			auto& e = Core::Engine::GetInstance();
			e.Init();

			auto& em = e.GetEntityManager();
			auto& rm = e.GetRenderableManager();
			auto& tm = e.GetTransformManager();
			auto& cm = e.GetCameraManager();
			auto& am = e.GetAnimationManager();
			auto& mm = e.GetMaterialManager();
			auto& level = em.Create();
			auto& mainC = em.Create();
			auto& camera = em.Create();
			auto& lm = e.GetLightManager();


			auto renderer = e.GetRenderer();
			auto resourceManager = e.GetResourceHandler();


			int horizontalHandle, verticalHandle;

			resourceManager->LoadResource("ComputeX.hlsl", [renderer, &horizontalHandle](auto guid, auto data, auto size) {
				horizontalHandle = renderer->CreateComputeShader(data, size);

				if (horizontalHandle > -1)
					return ResourceHandler::InvokeReturn::DecreaseRefcount;
				else
					return ResourceHandler::InvokeReturn::Fail;
			});

			resourceManager->LoadResource("ComputeY.hlsl", [renderer, &verticalHandle](auto guid, auto data, auto size) {
				verticalHandle = renderer->CreateComputeShader(data, size);

				if (verticalHandle > -1)
					return ResourceHandler::InvokeReturn::DecreaseRefcount;
				else
					return ResourceHandler::InvokeReturn::Fail;
			});


			renderer->EnableBloom(horizontalHandle, verticalHandle);



			/////////



			auto handle = e.GetWindow();

			tm.Create(level);

			cm.Bind(camera);
			cm.SetActive(camera);
			tm.SetRotation(camera, 0.0f, 0.0f, 0.0f);
			tm.SetPosition(camera, { 0.0f, 1.0f, -5.0f });

			handle->MapActionButton(ActionButton::Exit, Window::KeyEscape);
			handle->MapActionButton(ActionButton::Hide, Window::KeyO);
			handle->MapActionButton(ActionButton::Show, Window::KeyK);
			handle->MapActionButton(ActionButton::Up, Window::KeyW);
			handle->MapActionButton(ActionButton::Down, Window::KeyS);
			handle->MapActionButton(ActionButton::Left, Window::KeyA);
			handle->MapActionButton(ActionButton::Right, Window::KeyD);
			handle->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
			handle->MapActionButton(ActionButton::TL, Window::KeyLeft);

			handle->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
			handle->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

			handle->MapActionButton(ActionButton::Console, Window::Key1);

			/*rm.CreateRenderableObject(level, Utilz::GUID("Placeholder_level.obj"));
			rm.ToggleRenderableObject(level, true);*/

			tm.Create(mainC);
			tm.SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
			tm.SetRotation(mainC, 0.0f, 3.14f, 0.0f);

			Core::MaterialManager::CreateInfo info;
			Utilz::GUID textures[] = { Utilz::GUID("texture8.sei") };
			Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex") };
			auto shader = Utilz::GUID("SimpleLightBloomPS.hlsl");
			info.shader = shader;
			info.shaderResourceNames = resourceNames;
			info.textureFileNames = textures;
			info.textureCount = 2;

			mm.Create(mainC, info, true);

			rm.CreateRenderableObject(mainC, Utilz::GUID("MCModell.mesh"), true);
			rm.ToggleRenderableObject(mainC, true);

			auto& l = em.Create();
			Core::AddLightInfo d;
			d.radius = 100.0f;
			d.pos = { 0.0f, 5.0f, -5.0f };
			d.color = { 1, 1,1 };
			lm.AddLight(l, d);
			lm.ToggleLight(l, true);

			e.GetWindow()->MapActionButton(0, Window::KeyEscape);

			bool running = true;

			Utilz::Timer timer;
			while (running)
			{
				if (e.GetWindow()->ButtonPressed(0))
					running = false;

				timer.Tick();
				float dt = (float)timer.GetDelta();

				if (handle->ButtonDown(ActionButton::Up))
					tm.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.01f*dt });
				if (handle->ButtonDown(ActionButton::Down))
					tm.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.01f*dt });
				if (handle->ButtonDown(ActionButton::Right))
					tm.Move(camera, DirectX::XMFLOAT3{ 0.01f*dt, 0.0f, 0.0f });
				if (handle->ButtonDown(ActionButton::Left))
					tm.Move(camera, DirectX::XMFLOAT3{ -0.01f*dt, 0.0f, 0.0f });
				if (handle->ButtonDown(ActionButton::Rise))
					tm.Move(camera, DirectX::XMFLOAT3{ 0.0f, -0.01f*dt, 0.0f });
				if (handle->ButtonDown(ActionButton::Sink))
					tm.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.01f*dt, 0.0f });
				if (handle->ButtonDown(ActionButton::TL))
					tm.Rotate(camera, 0.0f, 0.01f, 0.0f);
				if (handle->ButtonDown(ActionButton::Console))
					e.GetDevConsole().Toggle();
				tm.Rotate(mainC, 0.0f, 0.01f, 0.0f);
				//tm.Move(mainC, { 0.01f, 0.0f, 0.0f });
				e.Frame(dt);
			}

			e.Release();

			return true;
		}
	}
}