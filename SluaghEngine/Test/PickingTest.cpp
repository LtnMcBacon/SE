#include "PickingTest.h"
#include <Core\Engine.h>
#include <Core\CollisionManager.h>

bool SE::Test::PickingTest::Run(SE::Utilz::IConsoleBackend * console)
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
	auto& cm = e.GetCollisionManager();
	
	auto floor = em.Create();
	auto object = em.Create();

	rm.CreateRenderableObject(floor, Utilz::GUID("Placeholder_Floor.obj"));
	rm.ToggleRenderableObject(floor, true);

	cm.CreateBoundingHierarchy(floor, Utilz::GUID("Placeholder_Floor.obj"));
	//auto test = cm.PickEntity(floor, )
	

	return false;
}
