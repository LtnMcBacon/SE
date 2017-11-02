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
			bool result = false;
			if(res < 0)	
				goto error;
			{
				auto managers = engine->GetManagers();
				auto subSystem = engine->GetSubsystems();


				res = subSystem.resourceHandler->LoadResource("HorizontalBloomPass.hlsl", [subSystem](auto guid, auto data, auto size) {
					auto res = subSystem.renderer->GetPipelineHandler()->CreateComputeShader(guid, data, size);
					if (res < 0)
						return ResourceHandler::InvokeReturn::FAIL;
					return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
				});
				if (res < 0)
					goto error;
				res = subSystem.resourceHandler->LoadResource("VerticalBloomPass.hlsl", [subSystem](auto guid, auto data, auto size) {
					auto res = subSystem.renderer->GetPipelineHandler()->CreateComputeShader(guid, data, size);
					if (res < 0)
						return ResourceHandler::InvokeReturn::FAIL;
					return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
				});
				if (res < 0)
					goto error;

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
				managers.materialManager->Create(mainC, info);

				managers.renderableManager->CreateRenderableObject(mainC, { "MCModell.mesh" });
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
				vp.height = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 640) / 3.0f;
				vp.width = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800) / 3.0f;
				vp.topLeftX = 0;
				vp.topLeftY = 0;
				vp.maxDepth = 1;
				vp.minDepth = 0.01;

				subSystem.renderer->GetPipelineHandler()->CreateViewport("topleft", vp);
				vp.topLeftY = (subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 800) / 3.0f)*1;
				subSystem.renderer->GetPipelineHandler()->CreateViewport("topdown1left", vp);

				Graphics::RenderJob drawBloomTexture;
				drawBloomTexture.pipeline.VSStage.shader = "FullscreenQuad"; // A default shader
				drawBloomTexture.pipeline.PSStage.shader = "MultiPS";// A default shader, also SinglePS
				drawBloomTexture.pipeline.PSStage.textureCount = 1;
				drawBloomTexture.pipeline.PSStage.textures[0] = "bloomTarget"; // Created in material manager
				drawBloomTexture.pipeline.PSStage.textureBindings[0] = "gTexture"; // A default texturebinding for MultiPS and SinglePS
				drawBloomTexture.pipeline.OMStage.renderTargets[0] = "backbuffer"; // The default render target
				drawBloomTexture.pipeline.OMStage.renderTargetCount = 1;
				drawBloomTexture.pipeline.OMStage.depthStencilView = Utilz::GUID(); // Make sure we use no depth testing.
				drawBloomTexture.pipeline.PSStage.samplers[0] = "AnisotropicSampler";
				drawBloomTexture.pipeline.PSStage.samplerCount = 1;
				drawBloomTexture.pipeline.RStage.viewport = "topleft";
				drawBloomTexture.vertexCount = 3;
				drawBloomTexture.indexCount = 0;
				drawBloomTexture.maxInstances = 0;

				subSystem.renderer->AddRenderJob(drawBloomTexture, Graphics::RenderGroup::POST_PASS_3);
				drawBloomTexture.pipeline.PSStage.textures[0] = "BloomUAV1"; 
				drawBloomTexture.pipeline.RStage.viewport = "topdown1left";
				subSystem.renderer->AddRenderJob(drawBloomTexture, Graphics::RenderGroup::POST_PASS_3);


				Graphics::UnorderedAccessView uav;
				uav.bindAsShaderResource = true;
				uav.clearColor[0] = 0.0f;
				uav.clearColor[1] = 1.0f;
				uav.clearColor[2] = 0.0f;
				uav.clearColor[3] = 0.0f;
				uav.format = Graphics::TextureFormat::R8G8B8A8_UNORM;
				uav.width = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 640);
				uav.height = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 800);

				res = subSystem.renderer->GetPipelineHandler()->CreateUnorderedAccessView("BloomUAV1", uav);
				if (res < 0)
					goto error;

				/*res = subSystem.renderer->GetPipelineHandler()->CreateUnorderedAccessView("BloomUAV2", uav);
				if (res < 0)
					goto error;*/

				Graphics::RenderJob horizontalPass;
				horizontalPass.pipeline.CSStage.shader = "HorizontalBloomPass.hlsl"; // TODO: Should unset all other shaders automaticly when creating a CS job.
				horizontalPass.pipeline.CSStage.textures[0] = "bloomTarget";
				horizontalPass.pipeline.CSStage.textureCount = 1;
				horizontalPass.pipeline.CSStage.textureBindings[0] = "inTex";
				//horizontalPass.pipeline.CSStage.UAV[0] = "BloomUAV1";


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

				result = true;
			}
		error:
			engine->Release();
			delete engine;

			return result;
		}
	}
}
