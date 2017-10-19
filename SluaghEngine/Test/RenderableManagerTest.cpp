#include "RenderableManagerTest.h"
#include <Core\Engine.h>
#include <Profiler.h>
#include <Utilz\CpuTimeCluster.h>
#include <Utilz\Timer.h>
#include <Imgui\imgui.h>

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


SE::Test::RenderableManagerTest::RenderableManagerTest()
{
}


SE::Test::RenderableManagerTest::~RenderableManagerTest()
{
}

bool SE::Test::RenderableManagerTest::Run(Utilz::IConsoleBackend * console)
{
	StartProfile;

	Utilz::CPUTimeCluster timers;

	timers.Start("Init");
	auto& e = Core::Engine::GetInstance();
	auto re = e.Init(Core::Engine::InitializationInfo());
	e.GetWindow();
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		ProfileReturnConst(false);
	}

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();
	auto& cm = e.GetCameraManager();
	auto& am = e.GetAnimationManager();
	auto& mm = e.GetMaterialManager();
	auto& level = em.Create();
	auto& mainC = em.Create();
	auto& camera = em.Create();
	auto& lm = e.GetLightManager();

	auto handle = e.GetWindow();

	tm.Create(level);

	cm.Bind(camera);
	cm.SetActive(camera);
	tm.SetRotation(camera, 0.0f, 0.0f, 0.0f);
	tm.SetPosition(camera, { 0.0f, 1.0f, -5.0f });

	handle->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	handle->MapActionButton(ActionButton::Hide, Window::KeyO);
	handle->MapActionButton(ActionButton::Show, Window::KeyK);
	handle->MapActionButton(ActionButton::Up, Window::KeyW);
	handle->MapActionButton(ActionButton::Down, Window::KeyS);
	handle->MapActionButton(ActionButton::Left, Window::KeyA);
	handle->MapActionButton(ActionButton::Right, Window::KeyD);
	handle->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
	handle->MapActionButton(ActionButton::TL, Window::KeyLeft);

	handle->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
	handle->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

	/*rm.CreateRenderableObject(level, Utilz::GUID("Placeholder_level.obj"));
	rm.ToggleRenderableObject(level, true);*/

	tm.Create(mainC);
	tm.SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tm.SetRotation(mainC, 0.0f, 3.14f, 0.0f);

	Core::MaterialManager::CreateInfo info;
	auto material = Utilz::GUID("lambert2_MCModell.mat");
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	info.shader = shader;
	info.materialFile = material;

	mm.Create(mainC, info, true);

	rm.CreateRenderableObject(mainC, Utilz::GUID("MCModell.mesh"), true);
	rm.ToggleRenderableObject(mainC, true);

	auto& l = em.Create();
	Core::AddLightInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 5.0f, -5.0f };
	d.color = { 1, 1,1 };
	lm.AddLight(l, d);
	lm.ToggleLight(l, true);

	e.GetWindow()->MapActionButton(0, Window::KeyEscape);

	bool running = true;



	Utilz::Timer timer;
	timers.Stop("Init");
	timers.Start("Running");
	e.GetDevConsole().Show();

	auto e1 = em.Create();
	em.Destroy(e1);

	auto e2 = em.Create();
	em.Destroy(e2);

	auto e3 = em.Create();
	em.Destroy(e3);
	while (running)
	{
		if (e.GetWindow()->ButtonPressed(0))
			running = false;

		timer.Tick();
		float dt = (float)timer.GetDelta();

		if (handle->ButtonDown(ActionButton::Up))
			tm.Move(camera, DirectX::XMFLOAT3 { 0.0f, 0.0f, 0.01f*dt });
		if (handle->ButtonDown(ActionButton::Down))
			tm.Move(camera, DirectX::XMFLOAT3 { 0.0f, 0.0f, -0.01f*dt });
		if (handle->ButtonDown(ActionButton::Right))
			tm.Move(camera, DirectX::XMFLOAT3 { 0.01f*dt, 0.0f, 0.0f });
		if (handle->ButtonDown(ActionButton::Left))
			tm.Move(camera, DirectX::XMFLOAT3 { -0.01f*dt, 0.0f, 0.0f });
		if (handle->ButtonDown(ActionButton::Rise))
			tm.Move(camera, DirectX::XMFLOAT3 { 0.0f, -0.01f*dt, 0.0f });
		if (handle->ButtonDown(ActionButton::Sink))
			tm.Move(camera, DirectX::XMFLOAT3 { 0.0f, 0.01f*dt, 0.0f });
		if (handle->ButtonDown(ActionButton::TL))
			tm.Rotate(camera, 0.0f, 0.01f, 0.0f);
		tm.Rotate(mainC, 0.0f, 0.01f, 0.0f);
		//tm.Move(mainC, { 0.01f, 0.0f, 0.0f });

		e.BeginFrame();

		if (ImGui::Button("CreateBakedTest", { 100, 100 }))
		{
			if (!em.Alive(e1))
			{
				e1 = em.Create();
				tm.Create(e1);
				tm.SetPosition(e1, DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));
				rm.CreateRenderableObject(e1, Utilz::GUID("bakedTest.mesh"), true);
				rm.ToggleRenderableObject(e1, true);
			}		
		}
		if (ImGui::Button("DestroyBakedTest", { 100, 100 }))
		{
			em.Destroy(e1);
		}
		if (ImGui::Button("CreateBakedTest2", { 100, 100 }))
		{
			if (!em.Alive(e2))
			{
				e2 = em.Create();
				tm.Create(e2);
				tm.SetPosition(e2, DirectX::XMFLOAT3(-2.0f, -2.0f, 0.0f));
				rm.CreateRenderableObject(e2, Utilz::GUID("bakedTest2.mesh"), true);
				rm.ToggleRenderableObject(e2, true);
			}
		}
		if (ImGui::Button("DestroyBakedTest2", { 100, 100 }))
		{
			em.Destroy(e2);
		}

		if (ImGui::Button("CreateBakedTest3", { 100, 100 }))
		{
			if (!em.Alive(e3))
			{
				e3 = em.Create();
				tm.Create(e3);
				tm.SetPosition(e3, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f));
				rm.CreateRenderableObject(e3, Utilz::GUID("bakedTest3.mesh"), true);
				rm.ToggleRenderableObject(e3, true);
			}
		}
		if (ImGui::Button("DestroyBakedTest3", { 100, 100 }))
		{
			em.Destroy(e3);
		}
		e.Frame(dt);
	}

	timers.Stop("Running");

	Utilz::TimeMap times;
	timers.GetMap(times);
	e.GetProfilingInformation(times);
	for(auto& t: times)
		console->Print("%s: %f\n", t.first.str, t.second);





	e.Release();
	ProfileReturnConst(true);
}
