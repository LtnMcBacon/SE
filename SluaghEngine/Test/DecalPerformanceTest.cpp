#include "DecalPerformanceTest.h"
#include <Core/IEngine.h>
#include <Imgui/imgui.h>
SE::Test::DecalPerformanceTest::DecalPerformanceTest()
{
}

SE::Test::DecalPerformanceTest::~DecalPerformanceTest()
{
}

bool SE::Test::DecalPerformanceTest::Run(DevConsole::IConsole* console)
{
	auto engine = Core::CreateEngine();
	auto r = engine->Init();
	if (r < 0)
	{
		delete engine;
		return false;
	}

	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());

	auto em = managers.entityManager;
	auto rm = managers.renderableManager;
	auto cm = managers.cameraManager;
	auto dm = managers.decalManager;
	auto tm = managers.transformManager;
	auto window = subSystem.window;
	auto mm = managers.materialManager;
	auto lm = managers.lightManager;


	struct
	{
		float x, y, z;
	} vertices[4];

	vertices[0].x = -100.0f; vertices[0].y = -0.01f; vertices[0].z = -100.0f;
	vertices[1].x = -100.0f; vertices[1].y = -0.01f; vertices[1].z = 100.0f;
	vertices[2].x = 100.0f; vertices[2].y = -0.01f; vertices[2].z = -100.0f;
	vertices[3].x = 100.0f; vertices[3].y = -0.01f; vertices[3].z = 100.0f;

	subSystem.renderer->GetPipelineHandler()->CreateVertexBuffer("DummyPlane", vertices, 4, sizeof(float) * 3, false);
	const std::string dummyShader = "cbuffer OncePerFrame : register(b1)\
	{\
		float4x4 View;\
		float4x4 ViewProj;\
	};\
	\
	struct VS_IN\
	{\
		float3 Pos : POSITION;\
	};\
	\
	struct VS_OUT\
	{\
		float4 Pos : SV_Position;\
	};\
	\
	VS_OUT VS_main(VS_IN input)\
	{\
		VS_OUT output = (VS_OUT)0;\
		output.Pos = mul(float4(input.Pos, 1.0f), ViewProj);\
		return output;\
	}";
	subSystem.renderer->GetPipelineHandler()->CreateVertexShaderFromSource("DummyPlaneVS.hlsl", dummyShader, "VS_main", "vs_5_0");
	Graphics::RenderJob dummyJob;
	dummyJob.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_STRIP;
	dummyJob.pipeline.IAStage.inputLayout = "DummyPlaneVS.hlsl";
	dummyJob.pipeline.IAStage.vertexBuffer = "DummyPlane";
	dummyJob.pipeline.VSStage.shader = "DummyPlaneVS.hlsl";
	dummyJob.pipeline.OMStage.renderTargetCount = 1;
	dummyJob.pipeline.OMStage.renderTargets[0] = "backbuffer";
	dummyJob.pipeline.OMStage.depthStencilView = "backbuffer";
	dummyJob.vertexCount = 4;
	uint32_t dummyBoxJobID = subSystem.renderer->AddRenderJob(dummyJob, Graphics::RenderGroup::RENDER_PASS_5);



	Core::Entity camera = em->Create();
	Core::ICameraManager::CreateInfo cmci;
	cmci.aspectRatio = 1280.0f / 720.0f;
	tm->Create(camera, { 0, 1.5f, -10 }, {DirectX::XM_PIDIV2 / 2, 0.0f, 0.0f});
	cm->Create(camera, cmci);
	cm->SetActive(camera);

	//Core::IMaterialManager::CreateInfo mInfo;
	//auto material = Utilz::GUID("MCModell.mat");
	//auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	//mInfo.shader = shader;
	//mInfo.materialFile = material;
	//Core::Entity box = em->Create();
	//tm->Create(box, { 0,0,5 }, { 0,0,0 }, { 8,8,1 });
	//mm->Create(box, mInfo);

	//Core::IRenderableManager::CreateInfo rmci;
	//rmci.meshGUID = "Placeholder_Block.mesh";
	//rmci.transparent = false;
	//rmci.wireframe = false;
	//rm->CreateRenderableObject(box, rmci);
	//rm->ToggleRenderableObject(box, true);

	
	auto l = em->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 5.0f, 2.0f };
	d.color = { 1, 1,1 };
	lm->Create(l, d);
	lm->ToggleLight(l, true);

	window->MapActionButton(Window::KeyEscape, Window::KeyEscape);

	window->MapActionButton(Window::KeyW, Window::KeyW);
	window->MapActionButton(Window::KeyA, Window::KeyA);
	window->MapActionButton(Window::KeyS, Window::KeyS);
	window->MapActionButton(Window::KeyD, Window::KeyD);
	window->MapActionButton(Window::KeyLeft, Window::KeyLeft);
	window->MapActionButton(Window::KeyRight, Window::KeyRight);
	window->MapActionButton(Window::KeyO, Window::KeyO);
	window->MapActionButton(Window::KeyL, Window::KeyL);
	window->MapActionButton(Window::KeyK, Window::KeyK);

	window->MapActionButton(Window::KeyY, Window::KeyY);
	window->MapActionButton(Window::KeyT, Window::KeyT);

	std::vector<Core::Entity> decals;


	bool running = true;
	float dt = 1 / 60.0f * 5.0f;
	
	while (running)
	{
		engine->BeginFrame();
		if (window->ButtonPressed(Window::KeyEscape))
			running = false;
		auto forward = tm->GetForward(camera);
		auto right = tm->GetRight(camera);
		if (window->ButtonDown(Window::KeyW))
			tm->Move(camera, { forward.x * dt, forward.y * dt, forward.z * dt, 0.0f });
		if (window->ButtonDown(Window::KeyS))
			tm->Move(camera, { -forward.x * dt, -forward.y * dt, -forward.z * dt, 0.0f });
		if (window->ButtonDown(Window::KeyD))
			tm->Move(camera, { right.x * dt, right.y * dt, right.z * dt, 0.0f });
		if (window->ButtonDown(Window::KeyA))
			tm->Move(camera, { -right.x * dt, -right.y * dt, -right.z * dt, 0.0f });
		if (window->ButtonDown(Window::KeyLeft))
			tm->Rotate(camera, 0.0f, -3.14 / 10.0f* dt, 0.0f);
		if (window->ButtonDown(Window::KeyRight))
			tm->Rotate(camera, 0.0f, 3.14 / 10.0f* dt, 0.0f);

		Utilz::TimeMap map;
		subSystem.renderer->GetProfilingInformation(map);

		if(ImGui::SliderInt("decals", &nrOfDecals, 0, 8192))
		{
			for (auto e : decals)
				em->DestroyNow(e);
			decals.clear();
			for(int i = 0; i < nrOfDecals; i++)
			{
				Core::Entity e = em->Create();
				tm->Create(e, { (float)(i % 32), 0.0f, (float)((i / 32) % 32) },{DirectX::XM_PIDIV2, 0.0f, 0.0f}, {1.0f, 1.0f, 0.05f});
				Core::DecalCreateInfo ci;
				ci.opacity = 1.0f;
				ci.ambiance = 1.0f;
				ci.textureName = "AimDecal.png";
				dm->Create(e, ci);
				dm->ToggleVisible(e, true);
				decals.push_back(e);
			}
		}
		int i = 0;
		static float timers[20] = {};
		static float averages[20] = {};
		static size_t frames = 0;
		for(auto& m : map)
		{
			++frames;
			
			std::string timestring;
			
			ImGui::TextUnformatted(m.first); ImGui::SameLine(0, 10); ImGui::TextUnformatted(std::to_string(m.second).c_str()); ImGui::SameLine(); ImGui::TextUnformatted("ms");
			ImGui::SameLine(0, 10); ImGui::TextUnformatted("Average: "); ImGui::SameLine(0, 10); ImGui::TextUnformatted(std::to_string(averages[i]).c_str()); ImGui::TextUnformatted("ms");
			timers[i] += m.second;
			if(frames > 100)
			{
				averages[i] = timers[i] / frames;
				frames = 0;
				timers[i] = 0;
			}
			++i;
		}
		
	
		engine->EndFrame();
	}
	engine->Release();
	delete engine;
	return true;
}

void SE::Test::DecalPerformanceTest::MakeDecal()
{
	for(auto& e : decals)
	{
		
	}
}
