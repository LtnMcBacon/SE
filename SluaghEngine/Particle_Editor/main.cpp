#include <Core\IEngine.h>
#include <Imgui\imgui.h>
#include <Utilz\Timer.h>
#include <Graphics\ParticleSystemJob.h>
#include <Particle_Editor\ParticleEmitter.h>
#include <string>
#include <filesystem>
#include <fstream>
namespace FS = std::experimental::filesystem;
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
	RotLeft,
	RotRight,
	RotUp,
	RotDown
};
// Variables for randomization of particle attributes
float velocityRangeX[2] = { -1.0f , 1.0f };
float velocityRangeY[2] = { -1.0f , 1.0f };
float velocityRangeZ[2] = { 0.0f , 0.0f };
float emitRangeX[2] = { 0.0f, 0.0f };
float emitRangeY[2] = { 0.0f, 0.0f };
float emitRangeZ[2] = { 0.0f, 0.0f };
float startPos[3] = { 0, 0, 0 };
float endPos[3] = { 0, 0, 0 };
bool RandVelocity = false;
bool imported = false;

Utilz::GUID importedTexture;
struct ParticleDataStruct {
	float vel[3];
	float pad;
	float emitPos[3];
	float pad1;
	float emitRange[3];
	float pad5;
	float gravity[3];
	float pad4;
	float startPos[3];
	float pad6;
	float endPos[3];
	float pad7;
	float speed;
	float emitRate;
	float lifeTime;
	float tangentValue;
	float radialValue;
	float gravityValue;
	float pSize;
	float dt;
	unsigned int circular;
	unsigned int gravityCheck;
	unsigned int emit;
	unsigned int particlePath;
	unsigned int bloomCheck;
};
ParticleDataStruct createParticleBuffer();
void exportParticleInfo(Utilz::GUID texName, ParticleDataStruct pInfo, char fileName[], bool randomVelocity, XMFLOAT2 velocityArr[], XMFLOAT2 emitArr[]);
void ImportParticleSystem(char fileName[], RenderJob &renderJob, ParticleDataStruct &cbuffer);
int main()
{
	Particle emitter;
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
	int changedTexture = 0;
	char particleName[100] = "";
	char loadSystem[100] = "";


	
	auto camera = engine->GetManagers().entityManager->Create();
	Core::ICameraManager::CreateInfo info;
	info.aspectRatio = 16.0f / 9.0f;
	info.farPlance = 100.0f;
	info.nearPlane = 0.01f;
	info.posistion = XMFLOAT3{ 0.0f, 1.0f, -5.0f };
	info.rotation = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	engine->GetManagers().cameraManager->Create(camera, info);
	engine->GetManagers().cameraManager->SetActive(camera);

//	engine->GetManagers().transformManager->SetForward(camera, XMVECTOR{0, 0, 1.0f});
//	engine->GetManagers().transformManager->SetRotation(camera, 0, 0, 0);
//	engine->GetManagers().transformManager->SetPosition(camera, { 0.0f, 1.0f, -5.0f });


	subSystem.window->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	subSystem.window->MapActionButton(ActionButton::Hide, Window::KeyO);
	subSystem.window->MapActionButton(ActionButton::Show, Window::KeyK);
	subSystem.window->MapActionButton(ActionButton::Up, Window::KeyW);
	subSystem.window->MapActionButton(ActionButton::Down, Window::KeyS);
	subSystem.window->MapActionButton(ActionButton::Left, Window::KeyA);
	subSystem.window->MapActionButton(ActionButton::Right, Window::KeyD);
	subSystem.window->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
	subSystem.window->MapActionButton(ActionButton::RotLeft, Window::KeyLeft);
	subSystem.window->MapActionButton(ActionButton::RotRight, Window::KeyRight);
	subSystem.window->MapActionButton(ActionButton::RotUp, Window::KeyUp);
	subSystem.window->MapActionButton(ActionButton::RotDown, Window::KeyDown);

	subSystem.window->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
	subSystem.window->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

	
	bool exportWindow = false;
	subSystem.devConsole->Toggle();
	ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());
	srand(time.GetDelta());

	char tempText[100] = "";//Used for finding texture

	ResourceHandle->LoadResource("ParticleGS.hlsl", [&pipelineHandler](auto guid, void* data, size_t size) {
		pipelineHandler->CreateGeometryShader(guid, data, size);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	ResourceHandle->LoadResource("ParticleVS.hlsl", [&pipelineHandler](auto guid, void* data, size_t size) {
		pipelineHandler->CreateVertexShader(guid ,data, size);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	ResourceHandle->LoadResource("ParticlePS.hlsl", [&pipelineHandler](auto guid,void* data, size_t size) {
		pipelineHandler->CreatePixelShader(guid, data, size);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	ResourceHandle->LoadResource("ParticleGSUpdate.hlsl", [&pipelineHandler](auto guid, void* data, size_t size) {
		pipelineHandler->CreateGeometryShaderStreamOut(guid, data, size);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	ResourceHandle->LoadResource("galaxy_texture.sei", [&pipelineHandler](auto guid, void* data, size_t size) {
		Graphics::TextureDesc texDesc;
		texDesc = *(TextureDesc*)data;
		data = (char*)data + sizeof(TextureDesc);
		pipelineHandler->CreateTexture(guid, data, texDesc.width, texDesc.height);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});

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

	Particle p;
	p.opacity = 1.0f;
	//Pipeline for the update geometry shader
	pipelineHandler->CreateBuffer("OutStreamBuffer1", nullptr, 0, sizeof(Particle), 10000, BufferFlags::BIND_VERTEX | BufferFlags::BIND_STREAMOUT);
	pipelineHandler->CreateBuffer("OutStreamBuffer2", &p, 1, sizeof(Particle), 10000, BufferFlags::BIND_VERTEX | BufferFlags::BIND_STREAMOUT);
	pipeline.IAStage.vertexBuffer = "OutStreamBuffer1";
	pipeline.IAStage.topology = PrimitiveTopology::POINT_LIST;
	pipeline.IAStage.inputLayout = "ParticleVS.hlsl";
	pipeline.VSStage.shader = "ParticleVS.hlsl";
	pipeline.GSStage.shader = "ParticleGSUpdate.hlsl";
	pipeline.SOStage.streamOutTarget = "OutStreamBuffer2";
	
	pipelineHandler->CreateDepthStencilState("noDepth", {false, false, ComparisonOperation::NO_COMPARISON });
	XMFLOAT3 lookAt = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 eyePos = { 0.0f, 1.0f, -5.0f };
	XMFLOAT3 upVec = { 0.0f, 1.0f, 0.0f };
	XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&lookAt), XMLoadFloat3(&upVec));

	XMFLOAT4X4 cameraMatrix;
	XMMATRIX cameraM = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, (float)window->Width() / window->Height(), 0.01f, 100.0f));
	XMMATRIX viewProj = viewMatrix * cameraM;
	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view, viewMatrix);
	DirectX::XMStoreFloat4x4(&cameraMatrix, viewProj);

	
	//Constant buffer for the geometry shader that updates the particles, also connected to ImGui

	ParticleDataStruct movBuffer = createParticleBuffer();

	//Particle constant buffer to get the right and up vector
	struct PCB {
		XMFLOAT3 upVector;
		float pad;
		XMFLOAT3 eyePosition;
		float pad2;
	};
	PCB constantBuffer;
	constantBuffer.eyePosition = eyePos;
	constantBuffer.upVector = upVec;

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());
	identity._41 = 0.0f;
	identity._42 = 2.0f;
	identity._43 = 0.0f;
	auto wMatrix = XMLoadFloat4x4(&identity);
	wMatrix = XMMatrixTranspose(wMatrix);
	XMStoreFloat4x4(&identity, wMatrix);

	RenderJob updateParticleJob;
	updateParticleJob.pipeline = pipeline;
	updateParticleJob.mappingFunc.push_back([&pipelineHandler, &movBuffer, &identity, &constantBuffer](int a, int b) {
		pipelineHandler->UpdateConstantBuffer("velocityBuffer", &movBuffer, sizeof(ParticleDataStruct));
		pipelineHandler->UpdateConstantBuffer("ParticleTransform", &identity, sizeof(XMFLOAT4X4));
		pipelineHandler->UpdateConstantBuffer("CamBuffer", &constantBuffer, sizeof(PCB));
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
	RPP.PSStage.textures[0] = "galaxy_texture.sei";
	RPP.PSStage.textureBindings[0] = "particleTex";
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



	struct CameraMatrices {
		XMFLOAT4X4 view;
		XMFLOAT4X4 viewProj;
	};
	CameraMatrices cameraMatrices;
	cameraMatrices.view = view;
	cameraMatrices.viewProj = cameraMatrix;

	renderParticleJob.mappingFunc.push_back([&renderParticleJob, &Renderer, &pipelineHandler, &cameraMatrices, &constantBuffer](int a, int b) {
		pipelineHandler->UpdateConstantBuffer("OncePerFrame", &cameraMatrices, sizeof(CameraMatrices));
		
		pipelineHandler->UpdateConstantBuffer("CameraPos", &constantBuffer, sizeof(PCB));
	});
	int RPPID = Renderer->AddRenderJob(renderParticleJob, SE::Graphics::RenderGroup::RENDER_PASS_5);
	 
	window->MapActionButton(Window::KeyEscape, Window::KeyEscape);
	float value = 0;
	Utilz::GUID texName = renderParticleJob.pipeline.PSStage.textures[0];//Default texture for exporting
	while (!window->ButtonPressed(Window::KeyEscape))
	{
		time.Tick();
		float dt = time.GetDelta<std::ratio<1, 1>>();
		constantBuffer.eyePosition = engine->GetManagers().transformManager->GetPosition(camera);
		XMFLOAT4X4 view = engine->GetManagers().cameraManager->GetView(camera);
		XMMATRIX tViewProj = XMLoadFloat4x4(&engine->GetManagers().cameraManager->GetViewProjection(camera));
		tViewProj = XMMatrixTranspose(tViewProj);
		 XMStoreFloat4x4(&cameraMatrices.viewProj, tViewProj);
		XMFLOAT3 eyePos = engine->GetManagers().transformManager->GetPosition(camera);
		constantBuffer.eyePosition = eyePos;
		constantBuffer.upVector = XMFLOAT3(view._12, view._22, view._32);
		//Camera movement
		if (subSystem.window->ButtonDown(ActionButton::Up))
			engine->GetManagers().transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Down))
			engine->GetManagers().transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -1.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Right))
			engine->GetManagers().transformManager->Move(camera, DirectX::XMFLOAT3{ 1.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Left))
			engine->GetManagers().transformManager->Move(camera, DirectX::XMFLOAT3{ -1.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Rise))
			engine->GetManagers().transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, -1.01f*dt, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Sink))
			engine->GetManagers().transformManager->Move(camera, DirectX::XMFLOAT3{ 0.0f, 1.01f*dt, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::RotLeft))
			engine->GetManagers().transformManager->Rotate(camera, 0.0f, 0.4f * dt, 0.0f);
		if (subSystem.window->ButtonDown(ActionButton::RotRight))
			engine->GetManagers().transformManager->Rotate(camera, 0.0f, -0.4f * dt, 0.0f);
		if (subSystem.window->ButtonDown(ActionButton::RotUp))
			engine->GetManagers().transformManager->Rotate(camera, -0.4f * dt, 0.0f, 0.0f);
		if (subSystem.window->ButtonDown(ActionButton::RotDown))
			engine->GetManagers().transformManager->Rotate(camera, 0.4f * dt, 0.0f, 0.0f);


		if (changedTexture || imported)
		{
			if (!imported)
			{
				ResourceHandle->LoadResource(tempText, [&pipelineHandler](auto guid, void* data, size_t size) {
					Graphics::TextureDesc texDesc;
					texDesc = *(TextureDesc*)data;
					data = (char*)data + sizeof(TextureDesc);
					pipelineHandler->CreateTexture(guid, data, texDesc.width, texDesc.height);
					return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
				});
				renderParticleJob.pipeline.PSStage.textures[0] = tempText;
				texName = tempText;

			}
			else
			{
				ResourceHandle->LoadResource(importedTexture, [&pipelineHandler](auto guid, void* data, size_t size) {
					Graphics::TextureDesc texDesc;
					texDesc = *(TextureDesc*)data;
					data = (char*)data + sizeof(TextureDesc);
					pipelineHandler->CreateTexture(guid, data, texDesc.width, texDesc.height);
					return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
				});
				texName = importedTexture;
				renderParticleJob.pipeline.PSStage.textures[0] = importedTexture;
				imported = false;
			}
			//renderParticleJob.pipeline.PSStage.textures[0] = Utilz::GUID(21045996);
			
		
			changedTexture = 0;
		}
		engine->BeginFrame();
		//Putting each separate window withing a Begin/End() section
		//Window for emit specific properties
		ImGui::Begin("Camera");
		//ImGui::BeginMenuBar();

		//ImGui::EndMenuBar();
		
	
		if (ImGui::Button("Reset Camera"))
		{
			
			engine->GetManagers().transformManager->SetRotation(camera, 0.0f, 0.0f, 0.0f);
			engine->GetManagers().transformManager->SetPosition(camera, { 0.0f, 1.0f, -5.0f });
		}
		ImGui::End();

		ImGui::Begin("Emit Attributes");
		ImGui::SliderFloat("Direction X", &movBuffer.vel[0], -1.0f, 1.0f);
		ImGui::SliderFloat("Direction Y", &movBuffer.vel[1], -1.0f, 1.0f);
		ImGui::SliderFloat("Direction Z", &movBuffer.vel[2], -1.0f, 1.0f);
		ImGui::SliderFloat2("Direction angle X", &velocityRangeX[0], -1.0, 1.0f);
		ImGui::SliderFloat2("Direction angle Y", &velocityRangeY[0], -1.0, 1.0f);
		ImGui::SliderFloat2("Direction angle Z", &velocityRangeZ[0], -1.0, 1.0f);
		ImGui::InputFloat2("Emit range X", &emitRangeX[0]);
		ImGui::InputFloat2("Emit range Y", &emitRangeY[0]);
		ImGui::InputFloat2("Emit range Z", &emitRangeZ[0]);
		ImGui::CheckboxFlags("Particle path", &movBuffer.particlePath, 1);
	//	ImGui::SliderFloat("Emit X", &movBuffer.emitPos[0], -5.0, 5.0f);
	//	ImGui::InputFloat3("Startpos", startPos);
		ImGui::InputFloat3("Endpos", endPos);
		if (ImGui::Button("Reset Sliders"))
		{
			for (size_t i = 0; i < 2; i++)
			{
				emitRangeX[i] = 0.0f;
				emitRangeY[i] = 0.0f;
				emitRangeZ[i] = 0.0f;
				velocityRangeX[i] = 0.0f;
				velocityRangeY[i] = 0.0f;
				velocityRangeZ[i] = 0.0f;
			}
		}
		if (ImGui::Button("Start/Stop Emit")) {
			movBuffer.emit ^= 1;
		}		
		ImGui::End();
	
		float randEmitX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (emitRangeX[1]/*Max*/ - emitRangeX[0]/*Min*/) + emitRangeX[0];
		float randEmitY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (emitRangeY[1] - emitRangeY[0]) + emitRangeY[0];
		float randEmitZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (emitRangeZ[1] - emitRangeZ[0]) + emitRangeZ[0];

		movBuffer.emitRange[0] = randEmitX;
		movBuffer.emitRange[1] = randEmitY;
		movBuffer.emitRange[2] = randEmitZ;
		
		if (RandVelocity && !movBuffer.particlePath)
		{
			float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (velocityRangeX[1]/*Max*/ - velocityRangeX[0]/*Min*/) + velocityRangeX[0];
			float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (velocityRangeY[1] - velocityRangeY[0]) + velocityRangeY[0];
			float r3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (velocityRangeZ[1] - velocityRangeZ[0]) + velocityRangeZ[0];
			movBuffer.vel[0] = r1;
			movBuffer.vel[1] = r2;
			movBuffer.vel[2] = r3;
		}
		if (movBuffer.particlePath)
		{
			for (size_t i = 0; i < 3; i++)
			{
				movBuffer.startPos[i] = startPos[i];
				movBuffer.endPos[i] = endPos[i];
			}
		}
		
		ImGui::Begin("Particle Attributes");
		ImGui::SliderFloat3("Gravity", &movBuffer.gravity[0], -1.0f, 1.0f);
		ImGui::SliderFloat("Gravity Scalar", &movBuffer.gravityValue, 0.0f, 10.0f);
		ImGui::SliderFloat("Radial Acceleration", &movBuffer.radialValue, -10.0f, 10.0f);
		ImGui::SliderFloat("Tangential Acceleration", &movBuffer.tangentValue, -10.0f, 10.0f);
		ImGui::SliderFloat("Speed", &movBuffer.speed, 0.1f, 5.0f);
		ImGui::InputFloat("Emit Rate", &movBuffer.emitRate);
		if (movBuffer.emitRate < 0)
			movBuffer.emitRate = 0;
		ImGui::InputFloat("LifeTime", &movBuffer.lifeTime);
		if (movBuffer.lifeTime < 0)
			movBuffer.lifeTime = 0;
		if (movBuffer.lifeTime > 30)
			movBuffer.lifeTime = 30;
		ImGui::InputFloat("Particle Size", &movBuffer.pSize);
		ImGui::CheckboxFlags("Enable Gravity", &movBuffer.gravityCheck, 1);
		ImGui::CheckboxFlags("Circular", &movBuffer.circular, 1);
		ImGui::Checkbox("Random direction", &RandVelocity);
		ImGui::CheckboxFlags("Bloom", &movBuffer.bloomCheck, 1);
		if (movBuffer.bloomCheck == 0)
		{
			int a = 0;
		}
		ImGui::InputText("Texture file", tempText, 100);
		if (ImGui::Button("Reset Sliders"))
		{
			movBuffer.radialValue = 0;
			movBuffer.tangentValue = 0;
			for (size_t i = 0; i < 3; i++)
			{
				movBuffer.gravity[i] = 0.0f;
			}
		}
		

		if (ImGui::Button("Change Texture")) changedTexture ^= 1;
		
		if (ImGui::Button("Export/Import Settings")) exportWindow ^= 1;
		ImGui::End();

		if (exportWindow)//Exporting window
		{
			XMFLOAT2 velocityRange[3];
			XMFLOAT2 emitRange[3];
			//XMFLOAT2 .x value is Max, .y is Min

			//Fill the XMFLOAT2's with the data to export
			velocityRange[0].x = velocityRangeX[1];
			velocityRange[0].y = velocityRangeX[0];
			velocityRange[1].x = velocityRangeY[1];
			velocityRange[1].y = velocityRangeY[0];
			velocityRange[2].x = velocityRangeZ[1];
			velocityRange[2].y = velocityRangeZ[0];

			emitRange[0].x = emitRangeX[1];
			emitRange[0].y = emitRangeX[0];
			emitRange[1].x = emitRangeY[1];
			emitRange[1].y = emitRangeY[0];
			emitRange[2].x = emitRangeZ[1];
			emitRange[2].y = emitRangeZ[0];

			ImGui::Begin("Export/Import Window");
			ImGui::InputText("Export file", particleName, 100);
			if (ImGui::Button("Export"))
				exportParticleInfo(texName, movBuffer, particleName, RandVelocity, velocityRange, emitRange);
			ImGui::InputText("Load file", loadSystem, 100);
			if (ImGui::Button("Import")) {
				ImportParticleSystem(loadSystem, renderParticleJob, movBuffer);
				imported = true;
			}
			
			ImGui::End();
		}
		//float dt = time.GetDelta<std::ratio<1, 1>>();
		//float dt = time.GetDelta();
		//dt = 10;
		movBuffer.dt = dt;
		//** swapping renderjobs for particle outstream
		std::swap(updateParticleJob.pipeline.SOStage.streamOutTarget, updateParticleJob.pipeline.IAStage.vertexBuffer);
		renderParticleJob.pipeline.IAStage.vertexBuffer = updateParticleJob.pipeline.SOStage.streamOutTarget;
		Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
		Renderer->ChangeRenderJob(RPPID, renderParticleJob);
		updateParticleJob.vertexCount = 0;
		Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
		//*****
		engine->EndFrame();
	}

	engine->Release();

	delete engine;
	return 0;
}
ParticleDataStruct createParticleBuffer()
{
	float velocity[3] = { 0.0f, 1.0f , 0.0f };
	float emitPosition[3] = { 0.0f, 0.0f, 0.0f };
	ParticleDataStruct movBuffer;
	movBuffer.vel[0] = velocity[0];
	movBuffer.vel[1] = velocity[1];
	movBuffer.vel[2] = velocity[2];
	movBuffer.emitPos[0] = emitPosition[0];
	movBuffer.emitPos[1] = emitPosition[1];
	movBuffer.emitPos[2] = emitPosition[2];
	movBuffer.gravityCheck = 0;
	for (size_t i = 0; i < 3; i++)
	{
		movBuffer.emitRange[i] = 0;
		movBuffer.gravity[i] = 0;
		movBuffer.startPos[i] = 0;
		movBuffer.endPos[i] = 0;
	}
	movBuffer.gravityValue = 0.0f;
	movBuffer.pSize = 0.1f;
	movBuffer.dt = 0.01f;
	movBuffer.speed = 0.05f;
	movBuffer.emitRate = 0.01;
	movBuffer.lifeTime = 5.0f;
	movBuffer.circular = 0;
	movBuffer.tangentValue = 0.0f;
	movBuffer.radialValue = 0.0f;
	movBuffer.emit = 0;
	movBuffer.particlePath = 0;
	movBuffer.bloomCheck = 1;

	return movBuffer;
}
void exportParticleInfo(Utilz::GUID texName, ParticleDataStruct pInfo, char fileName[], bool randomVelocity, XMFLOAT2 velocityArr[], XMFLOAT2 emitArr[])
{
	FS::create_directory("Particle_Systems");
	std::string file = fileName;
	XMFLOAT2 velocityRange[3];
	XMFLOAT2 emitRange[3];

	for (size_t i = 0; i < 3; i++)
	{
		velocityRange[i] = velocityArr[i];
		emitRange[i] = emitArr[i];
		pInfo.emitPos[i] = 0;
	}

	file += ".pts";
	
	std::ofstream toFile;
	
	toFile.open("Particle_Systems/" + file, std::ios::out | std::ios::binary);
	toFile.write((char*)&randomVelocity, sizeof(bool));
	toFile.write((char*)&texName, sizeof(Utilz::GUID));
	toFile.write((char*)&pInfo, sizeof(ParticleDataStruct));
	toFile.write((char*)velocityRange, sizeof(XMFLOAT2) * 3);
	toFile.write((char*)emitRange, sizeof(XMFLOAT2) * 3);
	toFile.close();

}
void ImportParticleSystem(char fileName[], RenderJob &renderJob, ParticleDataStruct &cbuffer)
{

	std::string filePath = fileName;
	filePath += ".pts";
	std::ifstream loadFile;
	bool tempRandVeloctity = false;
	Utilz::GUID texName;
	ParticleDataStruct tempData;
	XMFLOAT2 velocityRange[3] = {};
	XMFLOAT2 emitRange[3] = {};

	loadFile.seekg(0, std::ios::beg);
	//char* memBlock = new char[loadFile.tellg()];
	char* randVel = new char[sizeof(bool)];
	char* charTexture = new char[sizeof(Utilz::GUID)];
	char* buffer = new char[sizeof(ParticleDataStruct)];
	char* velocityRangeBuf = new char[sizeof(XMFLOAT2) * 3];
	char* emitRangeBuf = new char[sizeof(XMFLOAT2) * 3];

	loadFile.open("Particle_Systems/" + filePath, std::ios::in | std::ios::binary);
	loadFile.read(randVel, sizeof(bool));
	loadFile.read(charTexture, sizeof(Utilz::GUID));
	loadFile.read(buffer, sizeof(ParticleDataStruct));
	loadFile.read(velocityRangeBuf, sizeof(XMFLOAT2) * 3);
	loadFile.read(emitRangeBuf, sizeof(XMFLOAT2) * 3);

	memcpy(&tempRandVeloctity, randVel, sizeof(bool));
	memcpy(&texName, charTexture, sizeof(Utilz::GUID));
	memcpy(&tempData, buffer, sizeof(ParticleDataStruct));
	memcpy(&velocityRange, velocityRangeBuf, sizeof(XMFLOAT2) * 3);
	memcpy(&emitRange, emitRangeBuf, sizeof(XMFLOAT2) * 3);

	tempData.emitPos[2] = 1.0f;
	//Setting the global variables
	RandVelocity = tempRandVeloctity;

	velocityRangeX[1] = velocityRange[0].x;
	velocityRangeX[0] = velocityRange[0].y;
	velocityRangeY[1] = velocityRange[1].x;
	velocityRangeY[0] = velocityRange[1].y;
	velocityRangeZ[1] = velocityRange[2].x;
	velocityRangeZ[0] = velocityRange[2].y;

	emitRangeX[1] = emitRange[0].x;
	emitRangeX[0] = emitRange[0].y;
	emitRangeY[1] = emitRange[1].x;
	emitRangeY[0] = emitRange[1].y;
	emitRangeZ[1] = emitRange[2].x;
	emitRangeZ[0] = emitRange[2].y;
	
	//Changing the texture from the loaded file
	importedTexture = texName;
	renderJob.pipeline.PSStage.textures[0] = texName;

	//Loading the the cbuffer for the UpdateParticleJob
	cbuffer = tempData;

	delete[] randVel;
	delete[] charTexture;
	delete[] buffer;
	delete[] velocityRangeBuf;
	delete[] emitRangeBuf;
}