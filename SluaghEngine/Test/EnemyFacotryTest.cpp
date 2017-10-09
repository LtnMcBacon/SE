#include "EnemyFactoryTest.h"
#include "Gameplay/EnemyUnit.h"
#include "Gameplay/EnemyFactory.h"
#include <Profiler.h>
#include <Utilz/GUID.h>
#include "Core/Engine.h"
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyBlackboard.h"

using namespace SE;
using namespace Test;


EnemyFactoryTest::EnemyFactoryTest()
{
}

EnemyFactoryTest::~EnemyFactoryTest()
{

}

bool EnemyFactoryTest::Run(SE::Utilz::IConsoleBackend* console)
{
	StartProfile;
	bool passing = true;
	auto& engine = Core::Engine::GetInstance();
	engine.Init(Core::Engine::InitializationInfo());
	
	Gameplay::EnemyFactory enemyFactory;

	auto result = enemyFactory.LoadEnemyIntoMemory(Utilz::GUID("Test.SEC"));
	if(!result)
	{
		passing &= false;
	}

	Gameplay::GameBlackboard blackBoard;

	Gameplay::EnemyUnit* testEnemy = enemyFactory.CreateEnemy(Utilz::GUID("Test.SEC"), &blackBoard);
	if (!testEnemy)
		passing &= false;
	else
		delete testEnemy;

	Gameplay::EnemyUnit** testEnemies = new Gameplay::EnemyUnit*[20];
	Gameplay::EnemyFactory::EnemyToCreateDescription desc[2];
	desc[0].GUID = Utilz::GUID("Test.SEC");
	desc[0].useVariation = true;
	desc[0].nrOfEnemiesWithThisDescription = 10;

	desc[1].GUID = Utilz::GUID("Error.SEC");
	desc[1].useVariation = true;
	desc[1].nrOfEnemiesWithThisDescription = 10;

	enemyFactory.CreateEnemies(desc, &blackBoard, testEnemies, 20);

	for(int i = 0; i < 10; i++)
	{
		if (!testEnemies[i])
			passing &= false;
		else
		{
			delete testEnemies[i];
		}
	}
	for(int i = 10; i < 20; i++)
	{
		if (testEnemies[i])
		{
			passing &= false;
			delete testEnemies[i];
		}
	}

	delete[] testEnemies;

	Core::Engine::GetInstance().Release();

	ProfileReturnConst(passing);
}