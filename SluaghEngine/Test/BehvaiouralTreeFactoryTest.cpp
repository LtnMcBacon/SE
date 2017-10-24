#include "BehaviouralTreeFactoryTest.h"
#include <Gameplay/BehaviouralTreeFactory.h>
#include <Profiler.h>
#include <Utilz/GUID.h>
#include "Core/IEngine.h"
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyBlackboard.h"
#include "Gameplay/BehaviouralTree.h"
#include <Gameplay\Game.h>
using namespace SE;
using namespace Test;


BehaviouralTreeFactoryTest::BehaviouralTreeFactoryTest()
{
}

BehaviouralTreeFactoryTest::~BehaviouralTreeFactoryTest()
{

}

bool BehaviouralTreeFactoryTest::Run(SE::DevConsole::IConsole* console)
{
	StartProfile;
	bool passing = true;
	auto engine = Core::CreateEngine();
	Gameplay::Game game;
	engine->Init();
	game.Initiate(engine);
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	
	Gameplay::BehaviouralTreeFactory factory;
	passing &= factory.LoadTree(Utilz::GUID("data.SEFBT"));
	
	passing &= factory.LoadTree(Utilz::GUID("data.SEFBT"));

	passing &= factory.LoadTree(Utilz::GUID("FlowFieldMovement.SEFBT"));

	Gameplay::GameBlackboard gameBlackboard;
	Gameplay::EnemyBlackboard enemyBlackboard;

	auto test = factory.BuildBehaviouralTree(Utilz::GUID("data.SEFBT"), &gameBlackboard, &enemyBlackboard);
	if (test == nullptr)
	{
		passing &= false;
	}
	else
	{
		delete test;
	}

	test = factory.BuildBehaviouralTree(Utilz::GUID("FlowFieldMovement.SEFBT"), &gameBlackboard, &enemyBlackboard);
	if (test == nullptr)
	{
		passing &= false;
	}
	else
	{
		delete test;
	}

	test = factory.BuildBehaviouralTree(Utilz::GUID("Wrong.SEFBT"), &gameBlackboard, &enemyBlackboard);
	if(test)
	{
		passing &= false;
	}

	game.Shutdown();
	engine->Release(); delete engine;

	ProfileReturnConst(passing);
}