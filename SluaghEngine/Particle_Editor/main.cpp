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

	ParticleEmitter::Particle p[2];
	p[0].pos = { 0.0f, 0.0f, 10.0f };
	p[0].age = 0;
	p[0].size = { 0.5f, 0.5f };
	p[0].color = { 0.2f, 0.35f, 0.5f };
	p[0].type = 0;
	p[0].velocity = { 0, 0, 0 };
	//p[0].opacity = 1.0f;

	p[1].pos = { 0.0f, 0.0f, 10.0f };
	p[1].age = 0;
	p[1].size = { 0.5f, 0.5f };
	p[1].color = { 0.2f, 0.35f, 0.5f };
	//p[0].opacity = 1.0f;
	p[1].type = 0;
	p[1].velocity = { 0, 0, 0 };

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

	XMFLOAT2 velocity = { 0.0f, 1.0f };
	XMFLOAT3 emitPosition = { 0.0f, 0.0f, 5.0f };
	//Constant buffer for the geometry shader that updates the particles, also connected to ImGui
	struct moveMentStruct {
		XMFLOAT2 vel;
		XMFLOAT2 pad;
		XMFLOAT3 emitPos;
		float pad1;
		float color[3];
		float pad2;
		float speed;
		float emitRate;
		float lifeTime;
		float tangentValue;
		float radialValue;
		unsigned int circular;
	};
	moveMentStruct movBuffer;
	movBuffer.vel = velocity;
	movBuffer.emitPos = emitPosition;
	movBuffer.speed = 0.005;
	movBuffer.emitRate = 0.01;
	movBuffer.lifeTime = 5.0f;
	movBuffer.color[0] = 0.35f;
	movBuffer.color[1] = 0.2f;
	movBuffer.color[2] = 0.45f;
	movBuffer.circular = 0;
	movBuffer.tangentValue = 0.0f;
	movBuffer.radialValue = 0.0f;

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
		
		ImGui::Begin("Particle Attributes");
		ImGui::SliderFloat("Velocity X", &movBuffer.vel.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Velocity Y", &movBuffer.vel.y, -1.0f, 1.0f);
		ImGui::SliderFloat("Radial Acceleration", &movBuffer.radialValue, -50.0f, 50.0f);
		ImGui::SliderFloat("Tangential Acceleration", &movBuffer.tangentValue, -50.0f, 50.0f);
		ImGui::SliderFloat("Emit position X", &movBuffer.emitPos.x, -10.0f, 10.0f);
		ImGui::SliderFloat("Speed", &movBuffer.speed, 0.00100f, 0.00900, "%.5f");
		ImGui::SliderFloat("Emit Rate", &movBuffer.emitRate, 0.00050, 1.0000, "%.5f");
		ImGui::ColorEdit3("StartColor", &movBuffer.color[0]);
		ImGui::InputFloat("LifeTime", &movBuffer.lifeTime);
		ImGui::CheckboxFlags("Circular", &movBuffer.circular, 1);
		ImGui::Checkbox("Random velocity", &RandVelocity);

		if (RandVelocity)
		{
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/2.0f) - 1;
			float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/2.0f) - 1;
			float r3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.00900f);
			movBuffer.vel.x = r;
			movBuffer.vel.y = r2;
			movBuffer.speed = r3;
		}
		
		//** swapping renderjobs for particle outstream
		std::swap(updateParticleJob.pipeline.SOStage.streamOutTarget, updateParticleJob.pipeline.IAStage.vertexBuffer);
		renderParticleJob.pipeline.IAStage.vertexBuffer = updateParticleJob.pipeline.SOStage.streamOutTarget;
		Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
		Renderer->ChangeRenderJob(RPPID, renderParticleJob);
		//*****
		ImGui::End();
		engine->EndFrame();
		updateParticleJob.vertexCount = 0;
		Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
	}

	engine->Release();
	delete engine;
	return 0;
}
void loadResources()
{


}