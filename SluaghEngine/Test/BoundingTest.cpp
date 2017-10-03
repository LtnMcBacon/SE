#include "BoundingTest.h"
#include <Core\Engine.h>
//#include <Core\TransformManager.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

SE::Test::BoundingTest::BoundingTest()
{
}


SE::Test::BoundingTest::~BoundingTest()
{
}


static bool test = false;
void Collide1(const SE::Core::Entity& hit, const SE::Core::Entity& hitter)
{
	test = true;
}
void Collide2(const SE::Core::Entity& hit, const SE::Core::Entity& hitter)
{
	test = false;
}


bool SE::Test::BoundingTest::Run(Utilz::IConsoleBackend * console)
{
	auto& e = Core::Engine::GetInstance();
	e.Init(Core::Engine::InitializationInfo());
	auto& em = e.GetEntityManager();
	auto& col = e.GetCollisionManager();
	auto& tm = e.GetTransformManager();
	auto& ent = em.Create();
	tm.Create(ent);
	col.CreateBoundingHierarchy(ent, "Placeholder_Block.mesh");
	

	auto& ent2 = em.Create();
	tm.Create(ent2);
	col.CreateBoundingHierarchy(ent2, "Placeholder_Block.mesh");

	test = false;

	col.BindOnCollideWithAny(ent, Core::CollisionManager::CollideCallbackDelegate::Make<&Collide1>());

	e.Frame(0.0f);

	if (!test)
	{
		console->Print("Did not collide in check 1. They should!\n");
		return false;
	}
	test = true;

	col.BindOnCollideWithAny(ent, Core::CollisionManager::CollideCallbackDelegate::Make<&Collide2>());

	tm.SetPosition(ent2, { 100.0f, 100.0f, 100.0f });
	e.Frame(0.0f);
	if (!test)
	{
		console->Print("Collided in check 2. Should not collide!\n");
		return false;
	}
	
	test = false;
	col.BindOnCollideWithAny(ent, Core::CollisionManager::CollideCallbackDelegate::Make<&Collide1>());

	tm.SetPosition(ent, { 100.0f, 100.0f, 100.0f });
	e.Frame(0.0f);
	if (!test)
	{
		console->Print("Did not collide in check 3. They should!\n");
		return false;
	}
	test = true;
	col.BindOnCollideWithAny(ent, Core::CollisionManager::CollideCallbackDelegate::Make<&Collide2>());

	tm.SetPosition(ent, { 0.0f, 100.0f, 100.0f });
	e.Frame(0.0f);
	if (!test)
	{
		console->Print("Collided in check 4. Should not collide!\n");
		return false;
	}

	e.Release();
	return true;
}
