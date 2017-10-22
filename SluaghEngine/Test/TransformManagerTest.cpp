#include "TransformManagerTest.h"
#include <Core/IEngine.h>
#include <fstream>
#include <map>
#include <Profiler.h>
#include <Utilz\CPUTimeCluster.h>

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

	Core::IEngine::ManagerWrapper managers;
	managers.entityManager = Core::CreateEntityManager();
	managers.transformManager = Core::CreateTransformManager({ managers.entityManager });
	Entity entities[2048];

	///////////// Initialization and Expansion /////////////
	for(int i = 0; i < 2048; i++)
	{
		entities[i] = managers.entityManager->Create();
		managers.transformManager->Create(entities[i], {1.0f, 1.0f, 1.0f}, {2.0f, 2.0f, 2.0f}, { 2.0f, 2.0f, 2.0f });
	}
	for(int i = 0; i < 2048; i++)
	{
		if (!CompareFloat3(managers.transformManager->GetPosition(entities[i]), { 1.0f,1.0f,1.0f }))
			ProfileReturnConst(false);
		if (!CompareFloat3(managers.transformManager->GetRotation(entities[i]), { 2.0f,2.0f,2.0f }))
			ProfileReturnConst(false);
		if (!CompareFloat(managers.transformManager->GetScale(entities[i]).x, 2.0f))
			ProfileReturnConst(false);

	}
	///////////////// Absolute transforms ////////////////////////
	for(int i = 0; i < 2048; i++)
	{
		managers.transformManager->SetPosition(entities[i], { (float)i,1.0f,1.0f });
		managers.transformManager->SetScale(entities[i], 2.0f);
		managers.transformManager->SetRotation(entities[i], 1.0f, 1.0f, 1.0f);
	}
	for(int i = 0; i < 2048; i++)
	{
		DirectX::XMFLOAT3 pos = managers.transformManager->GetPosition(entities[i]);
		DirectX::XMFLOAT3 rot = managers.transformManager->GetRotation(entities[i]);
		float scale = managers.transformManager->GetScale(entities[i]).x;

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
		managers.transformManager->SetPosition(entities[i], { 0.0f, 0.0f, 0.0f });
		managers.transformManager->SetRotation(entities[i], 0.0f, 0.0f, 0.0f);
		managers.transformManager->SetScale(entities[i], 1.0f);
	}
	for(int i = 0; i < 2048; i++)
	{
		managers.transformManager->Move(entities[i], DirectX::XMFLOAT3{ 0.0f, 3.0f, 0.0f });
		managers.transformManager->Rotate(entities[i], 3.0f, 0.0f, 0.0f);
		managers.transformManager->Scale(entities[i], 3.0f);
	}
	for (int i = 0; i < 2048; i++)
	{
		DirectX::XMFLOAT3 pos = managers.transformManager->GetPosition(entities[i]);
		DirectX::XMFLOAT3 rot = managers.transformManager->GetRotation(entities[i]);
		float scale = managers.transformManager->GetScale(entities[i]).x;

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
		managers.transformManager->SetPosition(entities[i], { 5.0f,5.0f,5.0f });
		managers.transformManager->SetRotation(entities[i], 5.0f, 5.0f, 5.0f);
		managers.transformManager->SetScale(entities[i], 5.0f);
	}
	const uint32_t toRemove = 128;
	for(int i = 0; i < 2048; i++)
	{
		//Simulate entities being destroyed in arbitrary order
		std::swap(entities[rand() % 2048], entities[rand() % 2048]);
	}
	for(int i = 0; i < toRemove; i++)
	{
		managers.transformManager->SetScale(entities[i], 1.0f);
		managers.transformManager->SetPosition(entities[i], { 1.0f,1.0f,1.0f });
		managers.transformManager->SetRotation(entities[i], 1.0f, 1.0f, 1.0f);
		managers.entityManager->Destroy(entities[i]);
	}
	uint32_t callsToGC = 0;
	uint32_t nonEffectiveCalls = 0;
	struct Hist
	{
		uint32_t successes = 0;
		uint32_t fails = 0;
	};
	std::map<uint32_t, Hist> histogram;

	Utilz::CPUTimeCluster timer;
	while(managers.transformManager->ActiveTransforms() > 2048 - toRemove + 1)
	{
		managers.transformManager->Frame(&timer);
		++callsToGC;
		
	}
	console->Print("%d calls to GarbageCollector to clean up %d entities\n", callsToGC, toRemove);
	console->Print("%d calls found no garbage.\n", nonEffectiveCalls);
	console->Print("Remaining transforms: %d\n", managers.transformManager->ActiveTransforms());
	console->Print("See transformGarbageHist.txt for histogrmanagers.animationManager->\n");
	std::ofstream fout("transformGarbageHist.txt");

	fout << "Transforms left, successful garbage collection, no garbage collected\n";
	for(auto& h : histogram)
	{
		fout << h.first << ", " << h.second.successes << ", " << h.second.fails << "\n";
	}
	//Make sure the garbage collection didnt mess anything up
	
	for(int i = toRemove; i < 2048; i++)
	{
		if (!CompareFloat3(managers.transformManager->GetPosition(entities[i]), { 5.0f,5.0f,5.0f }))
		{
			DirectX::XMFLOAT3 p = managers.transformManager->GetPosition(entities[i]);
			console->Print("Got %f, %f, %f\n", p.x, p.y, p.z);
			ProfileReturnConst(false);
		}
		if (!CompareFloat3(managers.transformManager->GetRotation(entities[i]), { 5.0f,5.0f,5.0f }))
			ProfileReturnConst(false);
		if (!CompareFloat(managers.transformManager->GetScale(entities[i]).x, 5.0f))
			ProfileReturnConst(false);
	}

	auto& parent = managers.entityManager->Create();
	auto& child = managers.entityManager->Create();
	managers.transformManager->Create(parent, { 0.0f, 0.0f, 0.0f });
	managers.transformManager->Create(child, { 1.0f, 0.0f, 0.0f });
	managers.transformManager->Frame(&timer);
	managers.transformManager->BindChild(parent, child);
	managers.transformManager->Move(parent, DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	managers.transformManager->Frame(&timer);
	

	delete managers.transformManager;
	delete managers.entityManager;

	ProfileReturnConst(true);
}
