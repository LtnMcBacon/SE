#include "RenderableManagerTest.h"
#include <Core\Engine.h>

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
	auto& e = Core::Engine::GetInstance();
	auto& info = Core::Engine::InitializationInfo();
	auto re = e.Init(info);
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		return false;
	}

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();
	auto& level = em.Create();
	auto& mainC = em.Create();
	tm.Create(level);
	tm.Create(mainC);
	tm.SetPosition(mainC, DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));

	Core::RenderableManager::CreateRenderObjectInfo eInfo;
	eInfo.meshGUID = Utilz::GUID("Placeholder_level.obj");
	rm.CreateRenderableObject(level, eInfo);
	rm.ToggleRenderableObject(level, true);

	eInfo.meshGUID = Utilz::GUID("Placeholder_MC.obj");
	rm.CreateRenderableObject(mainC, eInfo);
	rm.ToggleRenderableObject(mainC, true);


	while (e.GetWindow()->HandleMSG())
	{
		tm.Move(mainC, DirectX::XMFLOAT3(0.01f, 0.0f, 0.0f));
		e.Frame();
	}
	


	e.Release();
	return true;
}
