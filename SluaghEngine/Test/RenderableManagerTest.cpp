#include "RenderableManagerTest.h"
#include <Core\Engine.h>
#include <Profiler.h>

#include <Utilz\Timer.h>

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
	Sink
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

	auto handle = e.GetWindow();

	tm.Create(level);
	tm.Create(mainC);
	tm.SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tm.SetRotation(mainC, 0.0f, 3.14f, 0.0f);
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

	handle->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
	handle->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

	/*rm.CreateRenderableObject(level, Utilz::GUID("Placeholder_level.obj"));
	rm.ToggleRenderableObject(level, true);*/

	Core::MaterialManager::CreateInfo info;
	Utilz::GUID textures[] = { Utilz::GUID("TestNormal.sei"), Utilz::GUID("purewhite.sei") };
	Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex"), Utilz::GUID("diffuseTexSec") };
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.shaderResourceNames = resourceNames;
	info.textureFileNames = textures;
	info.textureCount = 2;

	mm.Create(mainC, info);

	rm.CreateRenderableObject(mainC, Utilz::GUID("MCModell.mesh"));
	rm.ToggleRenderableObject(mainC, true);

	

	e.GetWindow()->MapActionButton(0, Window::KeyEscape);

	bool running = true;
	Utilz::Timer timer;

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


		//tm.Rotate(mainC, 0.0f, 0.0f, 0.01f);
		//tm.Move(mainC, { 0.01f, 0.0f, 0.0f });
		e.Frame(dt);
	}



	e.Release();
	ProfileReturnConst(true);
}
