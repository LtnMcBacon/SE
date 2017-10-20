#include <Core\Engine.h>
#include <Imgui/imgui.h>
#include <Utilz\Timer.h>
#include <Graphics\ParticleSystemJob.h>
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
	auto& Engine = Core::Engine::GetInstance();
	Engine.Init();
	auto window = Engine.GetWindow();
	auto Renderer = Engine.GetRenderer();
	Utilz::Timer time;
	auto ResourceHandle = Engine.GetResourceHandler();


	int vertexShaderHandle;
	int pixelShaderHandle;
	int geometryShaderHandle;
	
	ResourceHandle->LoadResource("ParticleGS.hlsl", [&geometryShaderHandle, &Renderer](auto guid, void* data, size_t size) {
		geometryShaderHandle = Renderer->CreateGeometryShader(data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	ResourceHandle->LoadResource("ParticleVS.hlsl", [&vertexShaderHandle, &Renderer](auto guid, void* data, size_t size) {
		vertexShaderHandle = Renderer->CreateVertexShader(data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	ResourceHandle->LoadResource("ParticlePS.hlsl", [&pixelShaderHandle, &Renderer](auto guid,void* data, size_t size) {
		pixelShaderHandle = Renderer->CreatePixelShader(data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	int vtxBufferHandle = Renderer->CreateDynamicVertexBuffer((sizeof(float) * 6) * 1000, sizeof(float) * 6);//Change the values here if the vertex buffer layout is changed
	DirectX::XMFLOAT4X4 identityMatrix;
	DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());

	Graphics::ParticleSystemJob particleJob = { vertexShaderHandle, geometryShaderHandle, pixelShaderHandle, vtxBufferHandle, identityMatrix, 4 };
	struct VS_INPUT {
		XMFLOAT3 pos;
		XMFLOAT3 col;
	};
	
	float pos1[3] = {0.0f, 0.0f, 0.0f};
	VS_INPUT particle[1];
	particle[0].pos = { 0 + pos1[0], 0 + pos1[1], 10 + pos1[2]};
	

	Renderer->UpdateDynamicVertexBuffer(vtxBufferHandle, particle, sizeof(VS_INPUT) * 4, sizeof(VS_INPUT));//Change the values here if vertex buffer layout is changed
	int particleJobID = Renderer->AddParticleSystemJob(particleJob);

	window->MapActionButton(Window::KeyEscape, Window::KeyEscape);
	float value = 0;
	while (!window->ButtonPressed(Window::KeyEscape))
	{
		time.Tick();
		Engine.BeginFrame();

		ImGui::Begin("TestWin");
		ImGui::SliderFloat("TestSlideX", &particle[0].pos.x, -2.0f, 1.0f);
		ImGui::SliderFloat("TestSlideY", &particle[0].pos.y, -2.0f, 1.0f);
		
		

		Renderer->UpdateDynamicVertexBuffer(vtxBufferHandle, particle, sizeof(VS_INPUT) * 4, sizeof(VS_INPUT));
		ImGui::End();
		Engine.Frame(time.GetDelta());
	}

	Engine.Release();
	return 0;
}