
#include <Gameplay\Game.h>
#include <Windows.h>

#ifdef _DEBUG
#pragma comment(lib, "CoreD.lib")
#else
#pragma comment(lib, "Core.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "GameplayD.lib")
#else
#pragma comment(lib, "Gameplay.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "WindowD.lib")
#else
#pragma comment(lib, "Window.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "ImGuiDX11SDLD.lib")
#else
#pragma comment(lib, "ImGuiDX11SDL.lib")
#endif
#include <Imgui\imgui.h>
using namespace SE;
int InitBloom(SE::Core::IEngine::Subsystems& subSystem, SE::Core::IEngine::ManagerWrapper& ma)
{
	auto res = subSystem.resourceHandler->LoadResource("HorizontalBloomPass.hlsl", [subSystem](auto guid, auto data, auto size) {
		auto res = subSystem.renderer->GetPipelineHandler()->CreateComputeShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::InvokeReturn::FAIL;
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	if (res < 0)
		return -1;
	res = subSystem.resourceHandler->LoadResource("VerticalBloomPass.hlsl", [subSystem](auto guid, auto data, auto size) {
		auto res = subSystem.renderer->GetPipelineHandler()->CreateComputeShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::InvokeReturn::FAIL;
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	if (res < 0)
		return -2;
	//static float bloomP[4] = { 0.3f, 0.6f, 2.0f, 0.667f };
	static float bloomP[4] = { 0.45f, 0.3f, 0.75f, 0.75f };
	subSystem.renderer->GetPipelineHandler()->UpdateConstantBuffer("BloomProperties", bloomP, sizeof(bloomP));

	Graphics::BlendState bs;
	bs.enable = true;
	bs.blendOperation = Graphics::BlendOperation::ADD;
	bs.blendOperationAlpha = Graphics::BlendOperation::MAX;
	bs.srcBlend = Graphics::Blend::SRC_COLOR;
	bs.srcBlendAlpha = Graphics::Blend::ONE;
	bs.dstBlend = Graphics::Blend::ONE;
	bs.dstBlendAlpha = Graphics::Blend::ONE;

	res = subSystem.renderer->GetPipelineHandler()->CreateBlendState("BloomBS", bs);
	if (res < 0)
		return -3;


	Graphics::Viewport vp;
	vp.height = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 640) / 4.0f;
	vp.width = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800) / 4.0f;
	vp.topLeftX = 0;
	vp.topLeftY = 0;
	vp.maxDepth = 1;
	vp.minDepth = 0.01;

	vp.width = 640;
	vp.height = 360;
	vp.topLeftX = 0;
	vp.topLeftY = 0;
	subSystem.renderer->GetPipelineHandler()->CreateViewport("bloomDownVP", vp);

	// Create a job for rendering the bloomTarget texture(Debug purposes)
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
	drawBloomTexture.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
	drawBloomTexture.vertexCount = 3;
	drawBloomTexture.indexCount = 0;
	drawBloomTexture.maxInstances = 0;
	//subSystem.renderer->AddRenderJob(drawBloomTexture, Graphics::RenderGroup::POST_PASS_4);


	// Create a job for downsampling the bloomTarget

	Graphics::RenderTarget rt;
	rt.bindAsShaderResource = true;
	rt.format = Graphics::TextureFormat::R8G8B8A8_UNORM;
	rt.width = 640;
	rt.height = 360;
	res = subSystem.renderer->GetPipelineHandler()->CreateRenderTarget("bloomDownTarget", rt);
	if (res < 0)
		return -4;

	drawBloomTexture.pipeline.RStage.viewport = "bloomDownVP";
	drawBloomTexture.pipeline.OMStage.renderTargets[0] = "bloomDownTarget";
	subSystem.renderer->AddRenderJob(drawBloomTexture, Graphics::RenderGroup::POST_PASS_0);


	// Create job for rendering the downsampled render target
	drawBloomTexture.pipeline.PSStage.textures[0] = "bloomDownTarget";
	drawBloomTexture.pipeline.RStage.viewport = "topdown1left";
	drawBloomTexture.pipeline.OMStage.renderTargets[0] = "backbuffer";

	//subSystem.renderer->AddRenderJob(drawBloomTexture, Graphics::RenderGroup::POST_PASS_4);


	// Create a job for horizontal blur
	Graphics::UnorderedAccessView uav;
	uav.bindAsShaderResource = true;
	uav.clearColor[0] = 0.0f;
	uav.clearColor[1] = 0.0f;
	uav.clearColor[2] = 0.0f;
	uav.clearColor[3] = 0.0f;
	uav.format = Graphics::TextureFormat::R8G8B8A8_UNORM;
	uav.width = 640;
	uav.height = 360;

	res = subSystem.renderer->GetPipelineHandler()->CreateUnorderedAccessView("BloomUAV1", uav);
	if (res < 0)
		return -5;

	res = subSystem.renderer->GetPipelineHandler()->CreateUnorderedAccessView("BloomUAV2", uav);
	if (res < 0)
		return -6;

	Graphics::RenderJob horizontalPass;
	horizontalPass.pipeline.CSStage.shader = "HorizontalBloomPass.hlsl";
	horizontalPass.pipeline.CSStage.textures[0] = "bloomDownTarget";
	horizontalPass.pipeline.CSStage.textureCount = 1;
	horizontalPass.pipeline.CSStage.textureBindings[0] = "inTex";
	horizontalPass.pipeline.CSStage.uavs[0] = "BloomUAV1";
	horizontalPass.pipeline.CSStage.uavCount = 1;
	horizontalPass.ThreadGroupCountX = 16;
	horizontalPass.ThreadGroupCountY = 15;
	horizontalPass.ThreadGroupCountZ = 1;

	subSystem.renderer->AddRenderJob(horizontalPass, Graphics::RenderGroup::POST_PASS_1);

	// Create job for rendering the horizontal blured.
	drawBloomTexture.pipeline.PSStage.textures[0] = "BloomUAV1";
	drawBloomTexture.pipeline.RStage.viewport = "topdown2left";
	drawBloomTexture.pipeline.OMStage.renderTargets[0] = "backbuffer";

//	subSystem.renderer->AddRenderJob(drawBloomTexture, Graphics::RenderGroup::POST_PASS_4);


	// Create job for vertical pass
	Graphics::RenderJob verticalPass;
	verticalPass.pipeline.CSStage.shader = "VerticalBloomPass.hlsl";
	verticalPass.pipeline.CSStage.textures[0] = "BloomUAV1";
	verticalPass.pipeline.CSStage.textures[1] = "backbuffer";
	verticalPass.pipeline.CSStage.textureCount = 1;
	verticalPass.pipeline.CSStage.textureBindings[0] = "inTex";
	verticalPass.pipeline.CSStage.uavs[0] = "BloomUAV2";
	verticalPass.pipeline.CSStage.uavCount = 1;
	verticalPass.ThreadGroupCountX = 16;
	verticalPass.ThreadGroupCountY = 15;
	verticalPass.ThreadGroupCountZ = 1;

	subSystem.renderer->AddRenderJob(verticalPass, Graphics::RenderGroup::POST_PASS_2);

	// Create job for rendering the vertical blured.
	drawBloomTexture.pipeline.PSStage.textures[0] = "BloomUAV2";
	drawBloomTexture.pipeline.RStage.viewport = Utilz::GUID();
	drawBloomTexture.pipeline.OMStage.renderTargets[0] = "backbuffer";
	drawBloomTexture.pipeline.OMStage.blendState = "BloomBS";
	subSystem.renderer->AddRenderJob(drawBloomTexture, Graphics::RenderGroup::POST_PASS_3);
	auto bp = &bloomP;
	subSystem.devConsole->AddFrameCallback([subSystem, bp]() {
		
		auto& b = *bp;
		ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());


		static bool bloom_set;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::MenuItem("Bloom Settings", nullptr, &bloom_set);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (bloom_set)
		{
			if (ImGui::SliderFloat("Base Muliplier", &b[0], 0.0f, 20.0f))
				subSystem.renderer->GetPipelineHandler()->UpdateConstantBuffer("BloomProperties", b, sizeof(bloomP));
			if (ImGui::SliderFloat("Fade Exponent", &b[1], 0.0f, 10.0f))
				subSystem.renderer->GetPipelineHandler()->UpdateConstantBuffer("BloomProperties", b, sizeof(bloomP));
			if (ImGui::SliderFloat("Additive Color Strength Multiplier", &b[2], 0.0f, 2.0f))
				subSystem.renderer->GetPipelineHandler()->UpdateConstantBuffer("BloomProperties", b, sizeof(bloomP));
			if (ImGui::SliderFloat("Bloom Color Threshold", &b[3], 0.0f, 1.0f))
				subSystem.renderer->GetPipelineHandler()->UpdateConstantBuffer("BloomProperties", b, sizeof(bloomP));
		}
	});

	return 0;
}
int InitializeFogDependencies(SE::Core::IEngine::Subsystems& subSystem)
{
	ResourceHandler::Callbacks topVertexShaderCallbacks;
	topVertexShaderCallbacks.loadCallback = [subSystem](auto guid, auto data, auto size, auto udata, auto usize)
	{
		*usize = size;
		int res = subSystem.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};
	topVertexShaderCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	topVertexShaderCallbacks.destroyCallback = [](auto guid, auto data, auto size) {

	};
	int res = subSystem.resourceHandler->LoadResource("FogTopVS.hlsl", topVertexShaderCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
	{
		throw std::exception("Could not load fog vertex shader");
		return -1;
	}



	ResourceHandler::Callbacks bottomVertexShaderCallbacks;
	bottomVertexShaderCallbacks.loadCallback = [subSystem](auto guid, auto data, auto size, auto udata, auto usize)
	{
		*usize = size;
		int res = subSystem.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};
	bottomVertexShaderCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	bottomVertexShaderCallbacks.destroyCallback = [](auto guid, auto data, auto size) {

	};
	res = subSystem.resourceHandler->LoadResource("FogBottomVS.hlsl", bottomVertexShaderCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
	{
		throw std::exception("Could not load fog vertex shader");
		return -1;
	}



	ResourceHandler::Callbacks geometryShaderCallbacks;
	geometryShaderCallbacks.loadCallback = [subSystem](auto guid, auto data, auto size, auto udata, auto usize)
	{
		*usize = size;
		int res = subSystem.renderer->GetPipelineHandler()->CreateGeometryShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};
	geometryShaderCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	geometryShaderCallbacks.destroyCallback = [](auto guid, auto data, auto size) {

	};
	res = subSystem.resourceHandler->LoadResource("FogTopGS.hlsl", geometryShaderCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
	{
		throw std::exception("Could not load fog geometry shader");
		return -1;
	}



	ResourceHandler::Callbacks topPixelShaderCallbacks;
	topPixelShaderCallbacks.loadCallback = [subSystem](auto guid, auto data, auto size, auto udata, auto usize)
	{
		*usize = size;
		int res = subSystem.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};
	topPixelShaderCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	topPixelShaderCallbacks.destroyCallback = [](auto guid, auto data, auto size) {

	};
	res = subSystem.resourceHandler->LoadResource("FogTopPS.hlsl", topPixelShaderCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
	{
		throw std::exception("Could not load fog top pixel shader");
		return -1;
	}



	ResourceHandler::Callbacks bottomPixelShaderCallbacks;
	bottomPixelShaderCallbacks.loadCallback = [subSystem](auto guid, auto data, auto size, auto udata, auto usize)
	{
		*usize = size;
		int res = subSystem.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};
	bottomPixelShaderCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	bottomPixelShaderCallbacks.destroyCallback = [](auto guid, auto data, auto size) {

	};
	res = subSystem.resourceHandler->LoadResource("FogBottomPS.hlsl", bottomPixelShaderCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
	{
		throw std::exception("Could not load fog bottom pixel shader");
		return -1;
	}



	ResourceHandler::Callbacks albedoTextureCallbacks;
	albedoTextureCallbacks.loadCallback = [subSystem](auto guid, auto data, auto size, auto udata, auto usize)
	{
		Graphics::TextureDesc td;
		memcpy(&td, data, sizeof(td));
		*usize = size - sizeof(td);
		// Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel
		if (td.width * td.height * 4 != size - sizeof(td))
			return ResourceHandler::LoadReturn::FAIL;

		void* rawTextureData = ((char*)data) + sizeof(td);
		auto result = subSystem.renderer->GetPipelineHandler()->CreateTexture(guid, rawTextureData, td.width, td.height);
		if (result < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};

	albedoTextureCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	albedoTextureCallbacks.destroyCallback = [subSystem](auto guid, auto data, auto size) {
		subSystem.renderer->GetPipelineHandler()->DestroyTexture(guid);
	};

	res = subSystem.resourceHandler->LoadResource("Fog_AlbedoTexture.png", albedoTextureCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
	{
		throw std::exception("Could not load fog albedo texture");
		return -1;
	}


	ResourceHandler::Callbacks normalTextureCallbacks;
	normalTextureCallbacks.loadCallback = [subSystem](auto guid, auto data, auto size, auto udata, auto usize)
	{
		Graphics::TextureDesc td;
		memcpy(&td, data, sizeof(td));
		*usize = size - sizeof(td);
		// Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel
		if (td.width * td.height * 4 != size - sizeof(td))
			return ResourceHandler::LoadReturn::FAIL;

		void* rawTextureData = ((char*)data) + sizeof(td);
		auto result = subSystem.renderer->GetPipelineHandler()->CreateTexture(guid, rawTextureData, td.width, td.height);
		if (result < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};

	normalTextureCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	normalTextureCallbacks.destroyCallback = [subSystem](auto guid, auto data, auto size) {
		subSystem.renderer->GetPipelineHandler()->DestroyTexture(guid);
	};

	res = subSystem.resourceHandler->LoadResource("Fog_NormalTexture.png", normalTextureCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
	{
		throw std::exception("Could not load fog normal texture");
		return -1;
	}



	Graphics::BlendState fogBlendState;

	fogBlendState.enable = true;
	fogBlendState.srcBlend = Graphics::Blend::SRC_ALPHA;
	fogBlendState.srcBlendAlpha = Graphics::Blend::ONE;
	fogBlendState.dstBlend = Graphics::Blend::INV_SRC_ALPHA;
	fogBlendState.dstBlendAlpha = Graphics::Blend::ONE;
	fogBlendState.blendOperation = Graphics::BlendOperation::ADD;
	fogBlendState.blendOperationAlpha = Graphics::BlendOperation::ADD;

	subSystem.renderer->GetPipelineHandler()->CreateBlendState("FogBs", fogBlendState);



	Graphics::SamplerState fogSampler;

	fogSampler.addressU = Graphics::AddressingMode::WRAP;
	fogSampler.addressV = Graphics::AddressingMode::WRAP;
	fogSampler.addressW = Graphics::AddressingMode::WRAP;

	fogSampler.filter = Graphics::Filter::ANISOTROPIC;
	fogSampler.maxAnisotropy = 4;

	subSystem.renderer->GetPipelineHandler()->CreateSamplerState("FogSs", fogSampler);



	Graphics::DepthStencilState fogDss;

	fogDss.enableDepth = true;
	fogDss.writeDepth = false;
	fogDss.comparisonOperation = Graphics::ComparisonOperation::LESS_EQUAL;

	subSystem.renderer->GetPipelineHandler()->CreateDepthStencilState("FogDss", fogDss);



	return 0;
}

using namespace SE;
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	
	SE::Gameplay::Game game;
	auto engine = Core::CreateEngine();
	Core::IEngine::InitializationInfo engineInit;
	engineInit.subSystems.optionsHandler = Core::CreateOptionsHandler();
	auto res = engineInit.subSystems.optionsHandler->Initialize("Config.ini");
	if (res < 0)
		throw std::exception("Could not initiate optionsHandler. Something went wrong with the Config.ini file");

	Window::InitializationInfo initInfo;
	initInfo.winState = Window::WindowState::Record;
	//initInfo.winState = Window::WindowState::Playback;
	initInfo.file = "Recordings/RecordingTuesday Tue Dec 12 085104 2017.bin";
	engineInit.subSystems.window = Window::CreateNewWindow();
	engineInit.subSystems.window->Initialize(initInfo);
	auto result= engine->Init(engineInit);
	if (result < 0)
		return -1;


	auto s = engine->GetSubsystems();
	auto m = engine->GetManagers();
	result = InitBloom(s, m);
	if (result < 0)
		return result;

	result = InitializeFogDependencies(s);
	if (result < 0)
		return result;

	
	game.Initiate(engine);
	game.Run();

	game.Shutdown();
	engine->Release();
	delete engine;

	return 0;
}