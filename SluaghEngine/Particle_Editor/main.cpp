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
	struct VS_INPUT {
		XMFLOAT3 pos;
		XMFLOAT3 col;
	};
	ParticleEmitter::Particle p[2];
	p[0].pos = { 5.0f, 0.0f, 10.0f };
	p[0].age = 0;
	p[0].size = { 0.5f, 0.5f };
	p[0].type = 0;
	p[0].velocity = { 0, 0, 0 };

	p[1].pos = { 4.0f, 0.0f, 10.0f };
	p[1].age = 0;
	p[1].size = { 0.5f, 0.5f };
	p[1].type = 0;
	p[1].velocity = { 0, 0, 0 };

	pipelineHandler->CreateBuffer("OutStreamBuffer1", nullptr, 0, sizeof(ParticleEmitter::Particle), 1000, BufferFlags::BIND_VERTEX | BufferFlags::BIND_STREAMOUT);
	pipelineHandler->CreateBuffer("OutStreamBuffer2", p, 2, sizeof(ParticleEmitter::Particle), 1000, BufferFlags::BIND_VERTEX | BufferFlags::BIND_STREAMOUT);
	pipeline.IAStage.vertexBuffer = "OutStreamBuffer1";
	pipeline.IAStage.topology = PrimitiveTopology::POINT_LIST;
	pipeline.IAStage.inputLayout = "ParticleVS.hlsl";
	pipeline.VSStage.shader = "ParticleVS.hlsl";
	pipeline.VSStage.constantBuffers[0] = "OncePerObject";
	pipeline.VSStage.constantBufferCount = 1;
	pipeline.GSStage.shader = "ParticleGSUpdate.hlsl";
	pipeline.GSStage.constantBuffers[0] = "ParticleInfo";
	pipeline.GSStage.constantBufferCount = 1;
	pipeline.SOStage.streamOutTarget = "OutStreamBuffer2";
	
	pipelineHandler->CreateDepthStencilState("noDepth", {false, false, ComparisonOperation::NO_COMPARISON});

	XMFLOAT4X4 cameraMatrix;
	XMMATRIX camera = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, (float)window->Width() / window->Height(), 0.01f, 100.0f));
	XMStoreFloat4x4(&cameraMatrix, camera);

	RenderJob updateParticleJob;
	updateParticleJob.pipeline = pipeline;
	updateParticleJob.mappingFunc = [&pipelineHandler](int a, int b) {
		XMFLOAT4X4 identity;
		XMStoreFloat4x4(&identity, XMMatrixIdentity());
	//	pipelineHandler->UpdateConstantBuffer("OncePerObject", &identity, sizeof(XMFLOAT4X4));
	};
	updateParticleJob.vertexCount = 2;
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


	renderParticleJob.mappingFunc = [&renderParticleJob, &Renderer, &pipelineHandler, &cameraMatrix](int a, int b) {
		XMFLOAT4X4 identity;
		XMStoreFloat4x4(&identity, XMMatrixIdentity());
	//	pipelineHandler->UpdateConstantBuffer("OncePerObject", &identity, sizeof(XMFLOAT4X4));
		pipelineHandler->UpdateConstantBuffer("OncePerFrame", &cameraMatrix, sizeof(XMFLOAT4X4));
		pipelineHandler->UpdateConstantBuffer("ParticleConstantBuffer", &identity, sizeof(XMFLOAT4X4));
	};
	int RPPID = Renderer->AddRenderJob(renderParticleJob);


	
	DirectX::XMFLOAT4X4 identityMatrix;
	DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());

	
	float pos1[3] = {0.0f, 0.0f, 0.0f};
	VS_INPUT particle[1];
	particle[0].pos = { 0 + pos1[0], 0 + pos1[1], 10 + pos1[2]};

	window->MapActionButton(Window::KeyEscape, Window::KeyEscape);
	float value = 0;
	while (!window->ButtonPressed(Window::KeyEscape))
	{
		time.Tick();
		Engine.BeginFrame();

		ImGui::Begin("TestWin");
		ImGui::SliderFloat("TestSlideX", &particle[0].pos.x, -2.0f, 1.0f);
		ImGui::SliderFloat("TestSlideY", &particle[0].pos.y, -2.0f, 1.0f);
		
		
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