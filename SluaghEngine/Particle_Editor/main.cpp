#include <Core\IEngine.h>
#include <Imgui\imgui.h>
#include <Utilz\Timer.h>
#include <Graphics\ParticleSystemJob.h>
#include "ParticleEmitter.h"

using namespace SE;
using namespace DirectX;
using namespace Graphics;
#ifdef _DEBUG

#pragma comment(lib, "ImGuiDX11SDLD.lib")

#else

#pragma comment(lib, "ImGuiDX11SDL.lib");

#endif
#ifdef _DEBUG
#pragma comment(lib, "CoreD.lib")
#else
#pragma comment(lib, "Core.lib")
#endif
struct moveMentStruct {
	XMFLOAT2 vel;
	XMFLOAT2 pad;
	float emitPos[3];
	float pad1;
	float emitRange[3];
	float pad5;
	float gravity[3];
	float pad4;
	float color[3];
	float pad2;
	float speed;
	float emitRate;
	float lifeTime;
	float tangentValue;
	float radialValue;
	float gravityValue;
	unsigned int circular;
	unsigned int gravityCheck;
};
moveMentStruct createParticleBuffer();
int main()
{
	ParticleEmitter emitter;
	auto engine = Core::CreateEngine();
	auto result = engine->Init();
	if (result < 0)
		return -1;
	auto subSystem = engine->GetSubsystems();
	auto window = subSystem.window;
	auto Renderer = subSystem.renderer;
	auto pipelineHandler = Renderer->GetPipelineHandler();
	Pipeline pipeline;
	Utilz::Timer time;
	auto ResourceHandle = subSystem.resourceHandler;
	
	ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());
	srand(time.GetDelta());

	ResourceHandle->LoadResource("ParticleGS.hlsl", [&pipelineHandler](auto guid, void* data, size_t size) {
		pipelineHandler->CreateGeometryShader(guid, data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	ResourceHandle->LoadResource("ParticleGSUpdate.hlsl", [&pipelineHandler](auto guid, void* data, size_t size) {
		pipelineHandler->CreateGeometryShaderStreamOut(guid, data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	ResourceHandle->LoadResource("ParticleVS.hlsl", [&pipelineHandler](auto guid, void* data, size_t size) {
		pipelineHandler->CreateVertexShader(guid ,data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	ResourceHandle->LoadResource("ParticlePS.hlsl", [&pipelineHandler](auto guid,void* data, size_t size) {
		pipelineHandler->CreatePixelShader(guid, data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	ResourceHandle->LoadResource("fire_texture.sei", [&pipelineHandler](auto guid, void* data, size_t size) {
		Graphics::TextureDesc texDesc;
		texDesc = *(TextureDesc*)data;
		data = (char*)data + sizeof(TextureDesc);
		pipelineHandler->CreateTexture(guid, data, texDesc.width, texDesc.height);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});

	ParticleEmitter::Particle p[2];
	p[0].opacity = 1.0f;
	p[1].opacity = 1.0f;

	
	//Setting blend state
	Graphics::BlendState bs;
	bs.enable = true;
	bs.blendOperation = Graphics::BlendOperation::ADD;
	bs.blendOperationAlpha = Graphics::BlendOperation::MAX;
	bs.srcBlend = Graphics::Blend::SRC_ALPHA;
	bs.srcBlendAlpha = Graphics::Blend::ONE;
	bs.dstBlend = Graphics::Blend::INV_SRC_ALPHA;
	bs.dstBlendAlpha = Graphics::Blend::ONE;

	pipelineHandler->CreateBlendState("ParticleBlend", bs);

	//Setting particle sampler state
	Graphics::SamplerState sampState;
	sampState.addressU = AddressingMode::CLAMP;
	sampState.addressV = AddressingMode::CLAMP;
	sampState.addressW = AddressingMode::CLAMP;
	sampState.filter = Filter::LINEAR;
	sampState.maxAnisotropy = 0;
	pipelineHandler->CreateSamplerState("ParticleSampler", sampState);

	//Pipeline for the update geometry shader
	int k = pipelineHandler->CreateBuffer("OutStreamBuffer1", nullptr, 0, sizeof(ParticleEmitter::Particle), 10000, BufferFlags::BIND_VERTEX | BufferFlags::BIND_STREAMOUT);
	pipelineHandler->CreateBuffer("OutStreamBuffer2", p, 2, sizeof(ParticleEmitter::Particle), 10000, BufferFlags::BIND_VERTEX | BufferFlags::BIND_STREAMOUT);
	pipeline.IAStage.vertexBuffer = "OutStreamBuffer1";
	pipeline.IAStage.topology = PrimitiveTopology::POINT_LIST;
	pipeline.IAStage.inputLayout = "ParticleVS.hlsl";
	pipeline.VSStage.shader = "ParticleVS.hlsl";
	pipeline.GSStage.shader = "ParticleGSUpdate.hlsl";
	pipeline.SOStage.streamOutTarget = "OutStreamBuffer2";
	
	pipelineHandler->CreateDepthStencilState("noDepth", {false, false, ComparisonOperation::NO_COMPARISON});

	XMFLOAT3 lookAt = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 eyePos = { 0.0f, 0.0f, -5.0f };
	XMFLOAT3 upVec = { 0.0f, 1.0f, 0.0f };
	XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&lookAt), XMLoadFloat3(&upVec));

	XMFLOAT4X4 cameraMatrix;
	XMMATRIX camera = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, (float)window->Width() / window->Height(), 0.01f, 100.0f));
	XMMATRIX viewProj = viewMatrix * camera;
	DirectX::XMStoreFloat4x4(&cameraMatrix, camera);


	//Constant buffer for the geometry shader that updates the particles, also connected to ImGui

	moveMentStruct movBuffer = createParticleBuffer();

	// Variables for randomization of particle attributes
	
	float minEmit = 0.0f;
	float maxEmit = 0.0f;
	float velocityRangeX[2] = { -1.0f , 1.0f };
	float velocityRangeY[2] = { -1.0f , 1.0f };
	float emitRangeX[2] = { 0.0f, 0.0f };
	float emitRangeY[2] = { 0.0f, 0.0f };

	RenderJob updateParticleJob;
	updateParticleJob.pipeline = pipeline;
	updateParticleJob.mappingFunc.push_back([&pipelineHandler, &movBuffer](int a, int b) {
		pipelineHandler->UpdateConstantBuffer("velocityBuffer", &movBuffer, sizeof(moveMentStruct));
	});
	updateParticleJob.vertexCount = 1;
	int updateParticleJobID = Renderer->AddRenderJob(updateParticleJob, SE::Graphics::RenderGroup::PRE_PASS_0);

	//Render particle job pipeline
	Pipeline RPP;
	RPP.IAStage.topology = PrimitiveTopology::POINT_LIST;
	RPP.IAStage.inputLayout = "ParticleVS.hlsl";
	RPP.IAStage.vertexBuffer = "OutStreamBuffer2";
	RPP.VSStage.shader = "ParticleVS.hlsl";
	RPP.GSStage.shader = "ParticleGS.hlsl";
	RPP.PSStage.shader = "ParticlePS.hlsl";
	RPP.PSStage.textures[0] = "fire_texture.sei";
	RPP.PSStage.textureBindings[0] = "fireTex";
	RPP.PSStage.textureCount = 1;
	RPP.PSStage.samplers[0] = "ParticleSampler";
	RPP.PSStage.samplerCount = 1;
	RPP.OMStage.blendState = "ParticleBlend";
	RPP.OMStage.renderTargets[0] = "backbuffer";
	RPP.OMStage.depthStencilView = "backbuffer";
	RPP.OMStage.depthStencilState = "noDepth";
	RPP.OMStage.renderTargetCount = 1;
	RenderJob renderParticleJob;
	renderParticleJob.pipeline = RPP;

	//Particle constant buffer to get the right and up vector
	struct PCB {
		XMFLOAT3 upVector;
		float pad;
		XMFLOAT3 eyePosition;
		float pad2;
	};
	bool RandVelocity = false;

	PCB constantBuffer;
	constantBuffer.eyePosition = eyePos;
	constantBuffer.upVector = upVec;

	renderParticleJob.mappingFunc.push_back([&renderParticleJob, &Renderer, &pipelineHandler, &cameraMatrix, &constantBuffer](int a, int b) {
		pipelineHandler->UpdateConstantBuffer("OncePerFrame", &cameraMatrix, sizeof(XMFLOAT4X4));
		pipelineHandler->UpdateConstantBuffer("ParticleConstantBuffer", &constantBuffer, sizeof(PCB));
	});
	int RPPID = Renderer->AddRenderJob(renderParticleJob, SE::Graphics::RenderGroup::RENDER_PASS_5);
	 
	window->MapActionButton(Window::KeyEscape, Window::KeyEscape);
	float value = 0;
	while (!window->ButtonPressed(Window::KeyEscape))
	{
		time.Tick();

		engine->BeginFrame();
		//First window, main particle attributes
		ImGui::Begin("Particle Attributes");//Putting each separate window withing a Begin/End() section
		ImGui::SliderFloat3("Gravity", &movBuffer.gravity[0], -1.0f, 1.0f);
		ImGui::SliderFloat("Gravity Scalar", &movBuffer.gravityValue, 0.0f, 1.0f);
		ImGui::SliderFloat("Radial Acceleration", &movBuffer.radialValue, -20.0f, 20.0f);
		ImGui::SliderFloat("Tangential Acceleration", &movBuffer.tangentValue, -20.0f, 20.0f);
		ImGui::SliderFloat("Speed", &movBuffer.speed, 0.00100f, 0.0100f, "%.5f");
		ImGui::SliderFloat("Emit Rate", &movBuffer.emitRate, 0.00050, 0.0500, "%.5f");
		//ImGui::ColorEdit3("StartColor", &movBuffer.color[0]);
		ImGui::InputFloat("LifeTime", &movBuffer.lifeTime);
		ImGui::CheckboxFlags("Enable Gravity", &movBuffer.gravityCheck, 1);
		ImGui::CheckboxFlags("Circular", &movBuffer.circular, 1);
		ImGui::Checkbox("Random direction", &RandVelocity);
		ImGui::End();

		ImGui::Begin("Emit Attributes");
		ImGui::SliderFloat("Direction X", &movBuffer.vel.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Direction Y", &movBuffer.vel.y, -1.0f, 1.0f);
		ImGui::SliderFloat2("Direction angle X", &velocityRangeX[0], -1.0, 1.0f);
		ImGui::SliderFloat2("Direction angle Y", &velocityRangeY[0], -1.0, 1.0f);
		ImGui::SliderFloat2("Emit position", &movBuffer.emitPos[0], -1.0f, 1.0f);
		ImGui::SliderFloat2("Emit range X", &emitRangeX[0], -1.0f, 1.0f);
		ImGui::SliderFloat2("Emit range Y", &emitRangeY[0], -1.0f, 1.0f);
		ImGui::End();
		
		float randEmitX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (emitRangeX[1] - emitRangeX[0]) - emitRangeX[0];
		float randEmitY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (emitRangeY[1] - emitRangeY[0]) - emitRangeY[0];
		movBuffer.emitRange[0] = randEmitX;
		movBuffer.emitRange[1] = randEmitY;

		if (RandVelocity)
		{
			float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (velocityRangeX[1] - velocityRangeX[0]) + velocityRangeX[0];
			float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (velocityRangeY[1] - velocityRangeY[0]) + velocityRangeY[0];
		/*	float r = static_cast <float> (rand() - 1) / static_cast <float> (RAND_MAX/2.0f) - 1;
			float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/2.0f) - 1;*/
			movBuffer.vel.x = r1;
			movBuffer.vel.y = r2;
		}
		
		//** swapping renderjobs for particle outstream
		std::swap(updateParticleJob.pipeline.SOStage.streamOutTarget, updateParticleJob.pipeline.IAStage.vertexBuffer);
		renderParticleJob.pipeline.IAStage.vertexBuffer = updateParticleJob.pipeline.SOStage.streamOutTarget;
		Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
		Renderer->ChangeRenderJob(RPPID, renderParticleJob);
		//*****
		engine->EndFrame();
		updateParticleJob.vertexCount = 0;
		Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
	}

	engine->Release();
	delete engine;
	return 0;
}
moveMentStruct createParticleBuffer()
{
	XMFLOAT2 velocity = { 0.0f, 1.0f };
	float emitPosition[3] = { 0.0f, 0.0f, 5.0f };
	moveMentStruct movBuffer;
	movBuffer.vel = velocity;
	movBuffer.emitPos[0] = emitPosition[0];
	movBuffer.emitPos[1] = emitPosition[1];
	movBuffer.emitPos[2] = emitPosition[2];
	movBuffer.emitRange[0] = 0;
	movBuffer.emitRange[1] = 0;
	movBuffer.emitRange[2] = 0;
	movBuffer.gravity[0] = 0.0f;
	movBuffer.gravity[1] = 0.0f;
	movBuffer.gravity[2] = 0.0f;
	movBuffer.gravityValue = 0.0f;
	movBuffer.speed = 0.005;
	movBuffer.emitRate = 0.01;
	movBuffer.lifeTime = 5.0f;
	movBuffer.color[0] = 0.35f;
	movBuffer.color[1] = 0.2f;
	movBuffer.color[2] = 0.45f;
	movBuffer.circular = 0;
	movBuffer.tangentValue = 0.0f;
	movBuffer.radialValue = 0.0f;

	return movBuffer;
}