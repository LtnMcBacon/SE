#include "DebugRenderTest.h"
#include <Core\Engine.h>
#include <Utilz/Timer.h>
#include <Imgui/imgui.h>
SE::Test::DebugRenderManagerTest::DebugRenderManagerTest()
{
}

SE::Test::DebugRenderManagerTest::~DebugRenderManagerTest()
{
}

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
	FrameTime,
	Jiggle,
	RemoveStuff,
	Console
};
bool SE::Test::DebugRenderManagerTest::Run(Utilz::IConsoleBackend * console)
{
	auto& engine = Core::Engine::GetInstance();
	engine.Init(Core::Engine::InitializationInfo());
	auto& em = engine.GetEntityManager();
	auto& mm = engine.GetMaterialManager();
	auto& tm = engine.GetTransformManager();
	auto& cm = engine.GetCameraManager();
	auto& rm = engine.GetRenderableManager();
	auto& drm = engine.GetDebugRenderManager();
	auto& colm = engine.GetCollisionManager();
	Core::Entity entity = em.Create();
	const int numEnts = 600;
	Core::Entity ents[numEnts];
	Core::MaterialManager::CreateInfo info;
	Utilz::GUID material = Utilz::GUID("MCModell.mat");
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.materialFile = material;
	for (int i = 0; i < numEnts; i++)
	{
		ents[i] = em.Create();
		mm.Create(ents[i], info);
		tm.Create(ents[i], { (float)(i*3.0f),0.0f,(float)((i * 3) % 2) }, { 0.0f,3.14f,0.0f }, { 5.02f,5.02f,5.02f });
		//tm.Create(ents[i]);
		rm.CreateRenderableObject(ents[i], Utilz::GUID("Placeholder_Block.mesh"));
		rm.ToggleRenderableObject(ents[i], true);
		
		drm.ToggleDebugRendering(ents[i], true);
		drm.DrawCross(ents[i], 1.0f);

	}
	for (int i = 10; i < 20; i++)
	{
		drm.ToggleDebugRendering(ents[i], false);
	}
	for (int i = 0; i < 200; i++)
	{
		drm.DrawCross(ents[i], 1.0f);
	}
	
	for (int i = 7; i < 14; i++)
	{
		drm.ToggleDebugRendering(ents[i], false);
	}
	rm.ToggleRenderableObject(ents[2], false);
	rm.ToggleRenderableObject(ents[2], true);
	const auto camera = em.Create();
	cm.Bind(camera);
	cm.SetActive(camera);
	tm.SetRotation(camera, 0.9f, 0.0f, 0.0f);
	tm.SetPosition(camera, { 0.0f, 10.0f, -20.0f });


	auto w = engine.GetWindow();


	w->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	w->MapActionButton(ActionButton::Hide, Window::KeyO);
	w->MapActionButton(ActionButton::Show, Window::KeyK);
	w->MapActionButton(ActionButton::Up, Window::KeyW);
	w->MapActionButton(ActionButton::Down, Window::KeyS);
	w->MapActionButton(ActionButton::Left, Window::KeyA);
	w->MapActionButton(ActionButton::Right, Window::KeyD);
	w->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
	w->MapActionButton(ActionButton::FrameTime, Window::KeyF);
	w->MapActionButton(ActionButton::Jiggle, Window::KeyJ);
	w->MapActionButton(RemoveStuff, Window::KeyR);
	w->MapActionButton(Console, Window::KeyC);
	

	
	Utilz::Timer timer;
	auto& oh = engine.GetOptionHandler();

	uint32_t removeIndex = 0;
	bool showConsole = false;
	bool running = true;
	while (running)
	{
		engine.BeginFrame();
		
		timer.Tick();
		float dt = timer.GetDelta();
		if (w->ButtonPressed(ActionButton::Exit))
			running = false;


		if (w->ButtonDown(ActionButton::Up))

			tm.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.11f*dt });
		if (w->ButtonDown(ActionButton::Down))
			tm.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.11f*dt });
		if (w->ButtonDown(ActionButton::Right))
			tm.Move(camera, DirectX::XMFLOAT3{ 0.11f*dt, 0.0f, 0.0f });
		if (w->ButtonDown(ActionButton::Left))
			tm.Move(camera, DirectX::XMFLOAT3{ -0.11f*dt, 0.0f, 0.0f });


		if (w->ButtonPressed(ActionButton::Jiggle))
			tm.Move(ents[3], DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f });

		if (removeIndex < numEnts && w->ButtonDown(RemoveStuff))
			em.Destroy(ents[removeIndex++]);


		if (w->ButtonPressed(ActionButton::Console))
		{
			showConsole = !showConsole;
		}

		if (showConsole)
		{
			ImGui::Text("I am the console.\n");
		}


		if (w->ButtonPressed(ActionButton::Hide))
		{
			for (int i = 0; i< numEnts; i++)
			{
				rm.ToggleRenderableObject(ents[i], false);
			}
		}
		if (w->ButtonPressed(ActionButton::Show))
		{
			for (int i = 0; i< numEnts; i++)
			{
				rm.ToggleRenderableObject(ents[i], true);
			}
		}
		if (w->ButtonPressed(ActionButton::FrameTime))
		{
			console->Print("Frametime: %f ms\n", timer.GetDelta());
		}
		engine.Frame(0.01f);
	}


	engine.Release();

	return true;
}
