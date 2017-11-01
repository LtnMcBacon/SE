#include <Core\Engine.h>
#include <Imgui/imgui.h>
#include <Utilz\Timer.h>
#include <Graphics\ParticleSystemJob.h>
#include "ParticleEmitter.h"

using namespace SE;
using namespace DirectX;
using namespace Graphics;

#ifdef _DEBUG
#pragma comment(lib, "CoreD.lib")
#else
#pragma comment(lib, "Core.lib")
#endif
int main()
{
	ParticleEmitter emitter;
	auto& Engine = Core::Engine::GetInstance();
	Engine.Init();
	auto window = Engine.GetWindow();
	auto Renderer = Engine.GetRenderer();
	auto pipelineHandler = Renderer->GetPipelineHandler();
	Pipeline pipeline;
	Utilz::Timer time;
	auto ResourceHandle = Engine.GetResourceHandler();
	
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
	p[0].pos = { 5.0f, 0.0f, 10.0f };
	p[0].age = 0;
	p[0].size = { 0.5f, 0.5f };
	p[0].color = { 0.2f, 0.35f, 0.5f };
	//p[0].opacity = 1.0f;
	p[0].type = 0;
	p[0].velocity = { 0, 0, 0 };

	p[1].pos = { 4.0f, 0.0f, 10.0f };
	p[1].age = 0;
	p[1].size = { 0.5f, 0.5f };
	p[1].color = { 0.2f, 0.35f, 0.5f };
	//p[1].opacity = 1.0f;
	p[1].type = 0;
	p[1].velocity = { 0, 0, 0 };

	pipelineHandler->CreateBuffer("OutStreamBuffer1", nullptr, 0, sizeof(ParticleEmitter::Particle), 5000, BufferFlags::BIND_VERTEX | BufferFlags::BIND_STREAMOUT);
	pipelineHandler->CreateBuffer("OutStreamBuffer2", p, 2, sizeof(ParticleEmitter::Particle), 5000, BufferFlags::BIND_VERTEX | BufferFlags::BIND_STREAMOUT);
	pipeline.IAStage.vertexBuffer = "OutStreamBuffer1";
	pipeline.IAStage.topology = PrimitiveTopology::POINT_LIST;
	pipeline.IAStage.inputLayout = "ParticleVS.hlsl";
	pipeline.VSStage.shader = "ParticleVS.hlsl";
	pipeline.VSStage.constantBuffers[0] = "OncePerObject";
	pipeline.VSStage.constantBufferCount = 1;
	pipeline.GSStage.shader = "ParticleGSUpdate.hlsl";
	pipeline.GSStage.constantBuffers[0] = "ParticleInfo";
	pipeline.GSStage.constantBuffers[1] = "velocityBuffer";
	pipeline.GSStage.constantBufferCount = 2;
	pipeline.SOStage.streamOutTarget = "OutStreamBuffer2";
	
	pipelineHandler->CreateDepthStencilState("noDepth", {false, false, ComparisonOperation::NO_COMPARISON});

	XMFLOAT4X4 cameraMatrix;
	XMMATRIX camera = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, (float)window->Width() / window->Height(), 0.01f, 100.0f));
	DirectX::XMStoreFloat4x4(&cameraMatrix, camera);

	XMFLOAT2 velocity = { 0.0f, 1.0f };
	XMFLOAT3 emitPosition = { 0.0f, 0.0f, 0.0f };
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
	movBuffer.pad2 = 0;


	RenderJob updateParticleJob;
	updateParticleJob.pipeline = pipeline;
	updateParticleJob.mappingFunc = [&pipelineHandler, &movBuffer](int a, int b) {
		pipelineHandler->UpdateConstantBuffer("velocityBuffer", &movBuffer, sizeof(moveMentStruct));
	};
	updateParticleJob.vertexCount = 1;
	int updateParticleJobID = Renderer->AddRenderJob(updateParticleJob);

	//Render particle job pipeline
	Pipeline RPP;
	RPP.IAStage.topology = PrimitiveTopology::POINT_LIST;
	RPP.IAStage.inputLayout = "ParticleVS.hlsl";
	RPP.IAStage.vertexBuffer = "OutStreamBuffer2";
	RPP.VSStage.shader = "ParticleVS.hlsl";
	RPP.VSStage.constantBuffers[0] = "OncePerObject";
	RPP.VSStage.constantBufferCount = 1;
	RPP.GSStage.constantBuffers[0] = "OncePerFrame";
	RPP.GSStage.constantBuffers[1] = "ParticleConstantBuffer";
	RPP.GSStage.constantBufferCount = 2;
	RPP.GSStage.shader = "ParticleGS.hlsl";
	RPP.PSStage.shader = "ParticlePS.hlsl";
	RPP.OMStage.renderTargets[0] = "backbuffer";
	RPP.OMStage.depthStencilView = "backbuffer";
	RPP.OMStage.depthStencilState = "noDepth";
	RPP.OMStage.renderTargetCount = 1;
	RenderJob renderParticleJob;
	renderParticleJob.pipeline = RPP;

	XMVECTOR eyePosVec;
	XMVECTOR lookAtVec;
	XMVECTOR upDirVec;
	XMFLOAT3 eyePos = { 0.0f, 0.0f, -5.0f };
	XMFLOAT3 lookAt = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 upVec = { 0.0f, 1.0f, 0.0f };
	XMMATRIX camView = XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&lookAt), XMLoadFloat3(&upVec)));

	renderParticleJob.mappingFunc = [&renderParticleJob, &Renderer, &pipelineHandler, &cameraMatrix, &camView](int a, int b) {
	/*	XMFLOAT4X4 identity;
		XMStoreFloat4x4(&identity, XMMatrixIdentity());*/
		pipelineHandler->UpdateConstantBuffer("OncePerFrame", &cameraMatrix, sizeof(XMFLOAT4X4));
		pipelineHandler->UpdateConstantBuffer("ParticleConstantBuffer", &camView, sizeof(XMFLOAT4X4));
	};
	int RPPID = Renderer->AddRenderJob(renderParticleJob);

	DirectX::XMFLOAT4X4 identityMatrix;
	DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
	 
	window->MapActionButton(Window::KeyEscape, Window::KeyEscape);
	float value = 0;
	while (!window->ButtonPressed(Window::KeyEscape))
	{
		time.Tick();
		Engine.BeginFrame();

		ImGui::Begin("TestWin");
		ImGui::SliderFloat("Velocity X", &movBuffer.vel.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Velocity Y", &movBuffer.vel.y, -1.0f, 1.0f);
		ImGui::SliderFloat("Emit position X", &movBuffer.emitPos.x, -10.0f, 10.0f);
		ImGui::SliderFloat("Speed", &movBuffer.speed, 0.001, 0.09);
		ImGui::SliderFloat("Emit Rate", &movBuffer.emitRate, 0.00500, 0.10000, "%.5f");
		//ImGui::SliderFloat("Lifetime", &movBuffer.lifeTime, 0.5, 10.0f);
		ImGui::ColorEdit3("StartColor", &movBuffer.color[0]);
		ImGui::InputFloat("LifeTime", &movBuffer.lifeTime);
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/2.0f) - 1;
		float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/2.0f) - 1;
		movBuffer.vel.x = r;
		movBuffer.vel.y = r2;
		
		//** swapping renderjobs for particle outstream
		std::swap(updateParticleJob.pipeline.SOStage.streamOutTarget, updateParticleJob.pipeline.IAStage.vertexBuffer);
		renderParticleJob.pipeline.IAStage.vertexBuffer = updateParticleJob.pipeline.SOStage.streamOutTarget;
		Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
		Renderer->ChangeRenderJob(RPPID, renderParticleJob);
		//*****
		ImGui::End();
		Engine.Frame(time.GetDelta());
		updateParticleJob.vertexCount = 0;
		Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
	}

	Engine.Release();
	return 0;
}
void loadResources()
{


}