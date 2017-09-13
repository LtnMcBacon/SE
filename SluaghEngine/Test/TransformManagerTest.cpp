#include "TransformManagerTest.h"
#include <Core/Engine.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

using namespace SE::Core;
using namespace SE::Test;

TransformManagerTest::TransformManagerTest()
{
}

TransformManagerTest::~TransformManagerTest()
{
}

bool TransformManagerTest::Run(SE::Utilz::IConsoleBackend* console)
{
	Engine& engine = Engine::GetInstance();
	Engine::InitializationInfo info;

	engine.Init(info);
	EntityManager& em = engine.GetEntityManager();
	TransformManager& tm = engine.GetTransformManager();
	Entity entities[64];
	for(int i = 0; i < 64; i++)
	{
		entities[i] = em.Create();
		tm.Create(entities[i], { (float)i ,0.0f, 0.0f });
	}
	DirectX::XMFLOAT3 direction = { 0.0f, 1.0f, 0.0f };
	for(int i = 0; i < 32; i++)
	{
		tm.Move(entities[i], direction);
	}
	for(int i = 0; i < 64; i++)
	{
		DirectX::XMFLOAT3 pos = tm.GetPosition(entities[i]);
		console->Print("%f, %f, %f\n", pos.x, pos.y, pos.z);
	}



	engine.Release();
	return true;
}
