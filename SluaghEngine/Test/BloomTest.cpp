#include "BloomTest.h"
#include <Core\IEngine.h>
#include <Utilz\Timer.h>

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
	TL
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

		bool BloomTest::Run(DevConsole::IConsole* console)
		{
			auto engine = Core::CreateEngine();
			auto res = engine->Init();
			if (res < 0)
			{
				delete engine;
				return false;
			}
				
			auto managers = engine->GetManagers();
			auto subSystem = engine->GetSubsystems();

			int horizontalHandle, verticalHandle;

		/*	subSystem.resourceHandler->LoadResource("HorizontalBloomPass.hlsl", [subSystem, &horizontalHandle](auto guid, auto data, auto size) {
				horizontalHandle = subSystem.renderer->CreateComputeShader(data, size);

				if (horizontalHandle > -1)
					return ResourceHandler::InvokeReturn::DecreaseRefcount;
				else
					return ResourceHandler::InvokeReturn::Fail;
			});

			subSystem.resourceHandler->LoadResource("VerticalBloomPass.hlsl", [subSystem, &verticalHandle](auto guid, auto data, auto size) {
				verticalHandle = subSystem.renderer->CreateComputeShader(data, size);

				if (verticalHandle > -1)
					return ResourceHandler::InvokeReturn::DecreaseRefcount;
				else
					return ResourceHandler::InvokeReturn::Fail;
			});


			subSystem.renderer->EnableBloom(horizontalHandle, verticalHandle);*/



			//
			// Modified RenderableManagerTest Copy Below
			//

			auto camera = managers.entityManager->Create();
			managers.cameraManager->Create(camera);
			managers.cameraManager->SetActive(camera);
			managers.transformManager->SetRotation(camera, 0.0f, 0.0f, 0.0f);
			managers.transformManager->SetPosition(camera, { 0.0f, 1.0f, -5.0f });

			subSystem.window->MapActionButton(ActionButton::Exit, Window::KeyEscape);
			subSystem.window->MapActionButton(ActionButton::Hide, Window::KeyO);
			subSystem.window->MapActionButton(ActionButton::Show, Window::KeyK);
			subSystem.window->MapActionButton(ActionButton::Up, Window::KeyW);
			subSystem.window->MapActionButton(ActionButton::Down, Window::KeyS);
			subSystem.window->MapActionButton(ActionButton::Left, Window::KeyA);
			subSystem.window->MapActionButton(ActionButton::Right, Window::KeyD);
			subSystem.window->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
			subSystem.window->MapActionButton(ActionButton::TL, Window::KeyLeft);

			subSystem.window->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
			subSystem.window->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

			/*rm.CreateRenderableObject(level, Utilz::GUID("Placeholder_level.obj"));
			rm.ToggleRenderableObject(level, true);*/

			auto mainC = managers.entityManager->Create();
			managers.transformManager->Create(mainC);
			managers.transformManager->SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
			managers.transformManager->SetRotation(mainC, 0.0f, 3.14f, 0.0f);

			Core::IMaterialManager::CreateInfo info;
			auto material = Utilz::GUID("MCModell.mat");
			auto shader = Utilz::GUID("SimpleLightBloomPS.hlsl");
			info.shader = shader;
			info.materialFile = material;
			info.bloom = true;
			managers.materialManager->Create(mainC, info, true);

			managers.renderableManager->CreateRenderableObject(mainC, { "MCModell.mesh" }, true);
			managers.renderableManager->ToggleRenderableObject(mainC, true);

			auto& l = managers.entityManager->Create();
			Core::ILightManager::CreateInfo d;
			d.radius = 100.0f;
			d.pos = { 0.0f, 5.0f, -5.0f };
			d.color = { 1, 1,1 };
			managers.lightManager->Create(l, d);
			managers.lightManager->ToggleLight(l, true);

			subSystem.window->MapActionButton(0, Window::KeyEscape);

			bool running = true;

			Graphics::Viewport vp;
			vp.height = 200;
			vp.width = 200;
			vp.topLeftX = 0;
			vp.topLeftY = 0;
			vp.maxDepth = 100;
			vp.minDepth = 0.001;
		
			Graphics::RenderJob drawBloomTexture;
			drawBloomTexture.pipeline.VSStage.shader = "FullscreenQuad";
			drawBloomTexture.pipeline.PSStage.shader = "MultiPS";
			drawBloomTexture.pipeline.PSStage.textureCount = 1;
			drawBloomTexture.pipeline.PSStage.textures[0] = "bloomTarget";
			drawBloomTexture.pipeline.PSStage.textureBindings[0] = "gTexture";
			drawBloomTexture.pipeline.OMStage.renderTargets[0] = "backbuffer";
			drawBloomTexture.pipeline.OMStage.renderTargets[1] = Utilz::GUID();
			drawBloomTexture.pipeline.OMStage.renderTargetCount = 2;
			drawBloomTexture.pipeline.OMStage.depthStencilView = "backbuffer";
			drawBloomTexture.pipeline.RStage.viewport = "topleft";
			drawBloomTexture.vertexCount = 3;
			drawBloomTexture.indexCount = 0;
			drawBloomTexture.maxInstances = 0;

			subSystem.renderer->AddRenderJob(drawBloomTexture, Graphics::RenderGroup::POST_PASS);

			Utilz::Timer timer;
			subSystem.devConsole->Toggle();

			auto e1 = managers.entityManager->Create();
			managers.entityManager->Destroy(e1);

			auto e2 = managers.entityManager->Create();
			managers.entityManager->Destroy(e2);

			auto e3 = managers.entityManager->Create();
			managers.entityManager->Destroy(e3);
			while (running)
			{
				if (subSystem.window->ButtonPressed(0))
					running = false;

				timer.Tick();
				float dt = (float)timer.GetDelta();

				if (subSystem.window->ButtonDown(ActionButton::Up))
					managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.01f*dt });
				if (subSystem.window->ButtonDown(ActionButton::Down))
					managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.01f*dt });
				if (subSystem.window->ButtonDown(ActionButton::Right))
					managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.01f*dt, 0.0f, 0.0f });
				if (subSystem.window->ButtonDown(ActionButton::Left))
					managers.transformManager->Move(camera, DirectX::XMFLOAT3{ -0.01f*dt, 0.0f, 0.0f });
				if (subSystem.window->ButtonDown(ActionButton::Rise))
					managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, -0.01f*dt, 0.0f });
				if (subSystem.window->ButtonDown(ActionButton::Sink))
					managers.transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.01f*dt, 0.0f });
				if (subSystem.window->ButtonDown(ActionButton::TL))
					managers.transformManager->Rotate(camera, 0.0f, 0.01f, 0.0f);
				managers.transformManager->Rotate(mainC, 0.0f, 0.01f, 0.0f);
				//tm.Move(mainC, { 0.01f, 0.0f, 0.0f });

				engine->BeginFrame();

				engine->EndFrame();
			}


			engine->Release();
			delete engine;

			return true;
		}
	}
}
