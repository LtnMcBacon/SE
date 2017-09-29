#include "InstancingTest.h"
#include <Core/Engine.h>
#include <Utilz/Timer.h>
SE::Test::InstancingTest::InstancingTest()
{
}

SE::Test::InstancingTest::~InstancingTest()
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
	FrameTime
};
bool SE::Test::InstancingTest::Run(Utilz::IConsoleBackend* console)
{
	auto& engine = Core::Engine::GetInstance();
	engine.Init(Core::Engine::InitializationInfo());
	auto& em = engine.GetEntityManager();
	auto& mm = engine.GetMaterialManager();
	auto& tm = engine.GetTransformManager();
	auto& cm = engine.GetCameraManager();
	auto& rm = engine.GetRenderableManager();

	Core::Entity entity = em.Create();
	const int numEnts = 600;
	Core::Entity ents[numEnts];
	Core::MaterialManager::CreateInfo info;
	Utilz::GUID textures[] = { Utilz::GUID("dummytex.sei"), Utilz::GUID("purewhite.sei") };
	Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex"), Utilz::GUID("diffuseTexSec") };
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.shaderResourceNames = resourceNames;
	info.textureFileNames = textures;
	info.textureCount = 2;
	for (int i = 0; i < numEnts; i++)
	{
		ents[i] = em.Create();
		mm.Create(ents[i], info);
		tm.Create(ents[i], { (float)(i*3.0f),0.0f,(float)((i * 3) % 2) },{0.0f,0.0f,0.0f},{0.02f,0.02f,0.02f});
		//tm.Create(ents[i]);
		rm.CreateRenderableObject(ents[i], Utilz::GUID("Placeholder_level.obj"));
		rm.ToggleRenderableObject(ents[i], true);

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

	bool running = true;
	Utilz::Timer timer;
	auto& oh = engine.GetOptionHandler();

	int full = oh.GetOption("Window", "fullScreen", 0);
	while (running)
	{
		timer.Tick();
		float dt = timer.GetDeltaMilliseconds();
		if (w->ButtonPressed(ActionButton::Exit))
			running = false;


		if (w->ButtonDown(ActionButton::Up))
			tm.Move(camera, { 0.0f, 0.0f, 0.11f*dt });
		if (w->ButtonDown(ActionButton::Down))
			tm.Move(camera, { 0.0f, 0.0f, -0.11f*dt });
		if (w->ButtonDown(ActionButton::Right))
			tm.Move(camera, { 0.11f*dt, 0.0f, 0.0f });
		if (w->ButtonDown(ActionButton::Left))
			tm.Move(camera, { -0.11f*dt, 0.0f, 0.0f });

		if(w->ButtonPressed(ActionButton::Hide))
		{
			for(int i = 0; i< numEnts; i++)
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
		if(w->ButtonPressed(ActionButton::FrameTime))
		{
			console->Print("Frametime: %f ms\n", 1.0f / timer.GetDeltaMilliseconds());
		}
		engine.Frame(0.01f);
	}


	engine.Release();

	return true;
}
