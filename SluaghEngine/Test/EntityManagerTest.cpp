#include "EntityManagerTest.h"
#include <Core\IEntityManager.h>
#include <Profiler.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

using namespace SE::Core;
using namespace SE::Test;
EntityManagerTest::EntityManagerTest()
{
}


EntityManagerTest::~EntityManagerTest()
{
}

bool EntityManagerTest::Run(SE::DevConsole::IConsole* console)
{
	StartProfile;
	auto em = Core::CreateEntityManager();

	Entity e[2048];

	for (int i = 0; i < 2048; i++)
	{
		e[i] = em->Create();
	}

	for (int i = 0; i < 1024; i++)
	{
		em->Destroy(e[i]);
	}
	for (int i = 0; i < 1024; i++)
	{
		if (em->Alive(e[i]))
			console->Print("%d: %u\n", i, e[i].id);
	}
	for (int i = 1024; i < 2048; i++)
	{
		if (!em->Alive(e[i]))
			console->Print("Dead: %u\n", e[i].id);
	}
	for (int i = 0; i < 1024; i++)
	{
		e[i] = em->Create();
	}

	for (int i = 0; i < 2048; i++)
	{
		if (!em->Alive(e[i]))
			console->Print("Is dead: %d\n", i);
	}
	for (int x = 0; x < 120; x++)
	{
		for (int i = 0; i < 2048; i++)
		{
			em->Destroy(e[i]);
		}
		for (int i = 0; i < 2048; i++)
		{
			e[i] = em->Create();
		}
	}

	for (int i = 0; i < 2048; i++)
	{
		for (int j = i + 1; j < 2048; j++)
		{
			if (e[i].id == e[j].id)
			{
				console->Print("Duplicate detected\n");
				return false;
			}
				
		}
	}
	delete em;
	ProfileReturnConst( true);
}
