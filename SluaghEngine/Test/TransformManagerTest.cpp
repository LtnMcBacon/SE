#include "TransformManagerTest.h"
#include <Core/Engine.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

using namespace SE::Core;
using namespace SE::Test;

static bool CompareFloat(float a, float b)
{
	return std::abs(a - b) < 0.00001f;
}

static bool CompareFloat3(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	return CompareFloat(a.x, b.x) && CompareFloat(a.y, b.y) && CompareFloat(a.z, b.z);
}

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
	Entity entities[2048];

	///////////// Initialization and Expansion /////////////
	for(int i = 0; i < 2048; i++)
	{
		entities[i] = em.Create();
		tm.Create(entities[i], {1.0f, 1.0f, 1.0f}, {2.0f, 2.0f, 2.0f}, 2.0f);
	}
	for(int i = 0; i < 2048; i++)
	{
		if (!CompareFloat3(tm.GetPosition(entities[i]), { 1.0f,1.0f,1.0f }))
			return false;
		if (!CompareFloat3(tm.GetRotation(entities[i]), { 2.0f,2.0f,2.0f }))
			return false;
		if (!CompareFloat(tm.GetScale(entities[i]), 2.0f))
			return false;

	}
	///////////////// Absolute transforms ////////////////////////
	for(int i = 0; i < 2048; i++)
	{
		tm.SetPosition(entities[i], { (float)i,1.0f,1.0f });
		tm.SetScale(entities[i], 2.0f);
		tm.SetRotation(entities[i], 1.0f, 1.0f, 1.0f);
	}
	for(int i = 0; i < 2048; i++)
	{
		DirectX::XMFLOAT3 pos = tm.GetPosition(entities[i]);
		DirectX::XMFLOAT3 rot = tm.GetRotation(entities[i]);
		float scale = tm.GetScale(entities[i]);

		if(!CompareFloat3(pos, {(float)i, 1.0f, 1.0f}))
		{
			return false;
		}
		if(!CompareFloat3(rot, {1.0f,1.0f,1.0f}))
		{
			return false;
		}
		if(!CompareFloat(scale, 2.0f))
		{
			return false;
		}

	}

	///////////// Relative Transforms //////////
	for (int i = 0; i < 2048; i++)
	{
		tm.SetPosition(entities[i], { 0.0f, 0.0f, 0.0f });
		tm.SetRotation(entities[i], 0.0f, 0.0f, 0.0f);
		tm.SetScale(entities[i], 1.0f);
	}
	for(int i = 0; i < 2048; i++)
	{
		tm.Move(entities[i], { 0.0f, 3.0f, 0.0f });
		tm.Rotate(entities[i], 3.0f, 0.0f, 0.0f);
		tm.Scale(entities[i], 3.0f);
	}
	for (int i = 0; i < 2048; i++)
	{
		DirectX::XMFLOAT3 pos = tm.GetPosition(entities[i]);
		DirectX::XMFLOAT3 rot = tm.GetRotation(entities[i]);
		float scale = tm.GetScale(entities[i]);

		if (!CompareFloat3(pos, {0.0f,3.0f,0.0f}))
		{
			return false;
		}
		if (!CompareFloat3(rot, {3.0f,0.0f,0.0f}))
		{
			return false;
		}
		if (!CompareFloat(3.0f, scale))
		{
			return false;
		}
	}
	////////////////

	engine.Release();
	return true;
}
