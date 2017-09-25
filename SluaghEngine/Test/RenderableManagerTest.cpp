#include "RenderableManagerTest.h"
#include <Core\Engine.h>
#include <Profiler.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif


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
	auto& info = Core::Engine::InitializationInfo();
	auto re = e.Init(info);
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
	auto& level = em.Create();
	auto& mainC = em.Create();
	auto& camera = em.Create();

	tm.Create(level);
	tm.Create(mainC);
	tm.SetPosition(mainC, DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));
	cm.Bind(camera);
	cm.SetActive(camera);
	tm.SetPosition(camera, { 0.0f, 1.0f, -2.0f });


	rm.CreateRenderableObject(level, Utilz::GUID("Placeholder_level.obj"));
	rm.ToggleRenderableObject(level, true);

	//rm.CreateRenderableObject(mainC, Utilz::GUID("Placeholder_MC.obj"));
	//rm.ToggleRenderableObject(mainC, true);
	//for (int i = 0; i < 10; i++)
	//{
	//	auto& en = em.Create();
	//	tm.Create(en);
	//	tm.SetPosition(en, DirectX::XMFLOAT3(rand() % 20, rand() % 20, rand() % 20));
	//	rm.CreateRenderableObject(en, Utilz::GUID("Placeholder_MC.obj"));
	//	rm.ToggleRenderableObject(en, true);
	//}




	e.GetWindow()->MapActionButton(0, Window::KeyEscape);

	bool running = true;
	while (running)
	{
		if (e.GetWindow()->ButtonPressed(0))
			running = false;
		tm.Move(mainC, DirectX::XMFLOAT3(0.01f, 0.0f, 0.0f));
		tm.Rotate(camera, 0.0f, 0.0f, 0.01f);
		e.Frame(0.01f);
	}
	


	e.Release();
	ProfileReturnConst(true);
}
