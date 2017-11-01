#include "ComputeTest.h"
#include <Core\IEngine.h>



SE::Test::ComputeTest::ComputeTest()
{
}


SE::Test::ComputeTest::~ComputeTest()
{
}

bool SE::Test::ComputeTest::Run(SE::DevConsole::IConsole * console)
{
	bool result = false;
	auto engine = Core::CreateEngine();
	auto res = engine->Init();
	if (res < 0)
		goto error;

	{
		auto managers = engine->GetManagers();
		auto subSystem = engine->GetSubsystems();


		res = subSystem.resourceHandler->LoadResource("ClearTexture.hlsl", [subSystem](auto guid, auto data, auto size) {
			auto res = subSystem.renderer->GetPipelineHandler()->CreateComputeShader(guid, data, size);
			if (res < 0)
				return ResourceHandler::InvokeReturn::Fail;
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		});
		if (res < 0)
			goto error;

		// Create a job for horizontal blur
		Graphics::UnorderedAccessView uav;
		uav.bindAsShaderResource = true;
		uav.clearColor[0] = 0.0f;
		uav.clearColor[1] = 0.0f;
		uav.clearColor[2] = 0.0f;
		uav.clearColor[3] = 0.0f;
		uav.format = Graphics::TextureFormat::R8G8B8A8_UNORM;
		uav.width = 512;
		uav.height = 512;

		res = subSystem.renderer->GetPipelineHandler()->CreateUnorderedAccessView("uav1", uav);
		if (res < 0)
			goto error;

		Graphics::RenderJob horizontalPass;
		horizontalPass.pipeline.CSStage.shader = "ClearTexture.hlsl";
		horizontalPass.pipeline.CSStage.uavs[0] = "uav1";
		horizontalPass.pipeline.CSStage.uavCount = 1;
		horizontalPass.ThreadGroupCountX = 16;
		horizontalPass.ThreadGroupCountY = 16;
		horizontalPass.ThreadGroupCountZ = 1;

		subSystem.renderer->AddRenderJob(horizontalPass, Graphics::RenderGroup::POST_PASS_0);



		subSystem.window->MapActionButton(0, Window::KeyEscape);
		bool running = true;
		while (running)
		{
			if (subSystem.window->ButtonPressed(0))
				running = false;
			engine->BeginFrame();
			engine->EndFrame();
		}
	}


	result = true;
error:
	engine->Release();
	delete engine;
	return result;
}
