#include "TransformManagerTest.h"
#include <Core/Engine.h>
#include <fstream>
#include <map>
#include <Profiler.h>

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
	StartProfile;

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
		tm.Create(entities[i], {1.0f, 1.0f, 1.0f}, {2.0f, 2.0f, 2.0f}, { 2.0f, 2.0f, 2.0f });
	}
	for(int i = 0; i < 2048; i++)
	{
		if (!CompareFloat3(tm.GetPosition(entities[i]), { 1.0f,1.0f,1.0f }))
			ProfileReturnConst(false);
		if (!CompareFloat3(tm.GetRotation(entities[i]), { 2.0f,2.0f,2.0f }))
			ProfileReturnConst(false);
		if (!CompareFloat(tm.GetScale(entities[i]).x, 2.0f))
			ProfileReturnConst(false);

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
		float scale = tm.GetScale(entities[i]).x;

		if(!CompareFloat3(pos, {(float)i, 1.0f, 1.0f}))
		{
			ProfileReturnConst(false);
		}
		if(!CompareFloat3(rot, {1.0f,1.0f,1.0f}))
		{
			ProfileReturnConst(false);
		}
		if(!CompareFloat(scale, 2.0f))
		{
			ProfileReturnConst(false);
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
		tm.Move(entities[i], DirectX::XMFLOAT3{ 0.0f, 3.0f, 0.0f });
		tm.Rotate(entities[i], 3.0f, 0.0f, 0.0f);
		tm.Scale(entities[i], 3.0f);
	}
	for (int i = 0; i < 2048; i++)
	{
		DirectX::XMFLOAT3 pos = tm.GetPosition(entities[i]);
		DirectX::XMFLOAT3 rot = tm.GetRotation(entities[i]);
		float scale = tm.GetScale(entities[i]).x;

		if (!CompareFloat3(pos, {0.0f,3.0f,0.0f}))
		{
			ProfileReturnConst(false);
		}
		if (!CompareFloat3(rot, {3.0f,0.0f,0.0f}))
		{
			ProfileReturnConst(false);
		}
		if (!CompareFloat(3.0f, scale))
		{
			ProfileReturnConst(false);
		}
	}
	////////////////Garbage collection///////////////////

	for(int i = 0; i < 2048; i++)
	{
		tm.SetPosition(entities[i], { 5.0f,5.0f,5.0f });
		tm.SetRotation(entities[i], 5.0f, 5.0f, 5.0f);
		tm.SetScale(entities[i], 5.0f);
	}
	const uint32_t toRemove = 128;
	for(int i = 0; i < 2048; i++)
	{
		//Simulate entities being destroyed in arbitrary order
		std::swap(entities[rand() % 2048], entities[rand() % 2048]);
	}
	for(int i = 0; i < toRemove; i++)
	{
		tm.SetScale(entities[i], 1.0f);
		tm.SetPosition(entities[i], { 1.0f,1.0f,1.0f });
		tm.SetRotation(entities[i], 1.0f, 1.0f, 1.0f);
		em.Destroy(entities[i]);
	}
	uint32_t callsToGC = 0;
	uint32_t nonEffectiveCalls = 0;
	struct Hist
	{
		uint32_t successes = 0;
		uint32_t fails = 0;
	};
	std::map<uint32_t, Hist> histogram;
	while(tm.ActiveTransforms() > 2048 - toRemove + 1)
	{
		if (tm.GarbageCollection() == 0)
		{
			++nonEffectiveCalls;
			histogram[tm.ActiveTransforms()].fails++;
		}
		else
		{
			histogram[tm.ActiveTransforms()].successes++;
		}
		++callsToGC;
		
	}
	console->Print("%d calls to GarbageCollector to clean up %d entities\n", callsToGC, toRemove);
	console->Print("%d calls found no garbage.\n", nonEffectiveCalls);
	console->Print("Remaining transforms: %d\n", tm.ActiveTransforms());
	console->Print("See transformGarbageHist.txt for histogram.\n");
	std::ofstream fout("transformGarbageHist.txt");

	fout << "Transforms left, successful garbage collection, no garbage collected\n";
	for(auto& h : histogram)
	{
		fout << h.first << ", " << h.second.successes << ", " << h.second.fails << "\n";
	}
	//Make sure the garbage collection didnt mess anything up
	
	for(int i = toRemove; i < 2048; i++)
	{
		if (!CompareFloat3(tm.GetPosition(entities[i]), { 5.0f,5.0f,5.0f }))
		{
			DirectX::XMFLOAT3 p = tm.GetPosition(entities[i]);
			console->Print("Got %f, %f, %f\n", p.x, p.y, p.z);
			ProfileReturnConst(false);
		}
		if (!CompareFloat3(tm.GetRotation(entities[i]), { 5.0f,5.0f,5.0f }))
			ProfileReturnConst(false);
		if (!CompareFloat(tm.GetScale(entities[i]).x, 5.0f))
			ProfileReturnConst(false);
	}

	auto& parent = em.Create();
	auto& child = em.Create();
	tm.Create(parent, { 0.0f, 0.0f, 0.0f });
	tm.Create(child, { 1.0f, 0.0f, 0.0f });
	tm.Frame();
	tm.BindChild(parent, child);
	tm.Move(parent, DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	tm.Frame();
	engine.Release();
	ProfileReturnConst(true);
}
