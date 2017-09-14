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
	auto& entity = em.Create();
	Core::RenderableManager::CreateRenderObjectInfo eInfo;
	eInfo.meshGUID = Utilz::GUID("test.objtest");
	rm.CreateRenderableObject(entity, eInfo);

	auto& entity2 = em.Create();
	rm.CreateRenderableObject(entity2, eInfo);

	e.Release();
	return false;
}
