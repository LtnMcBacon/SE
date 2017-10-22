#include "EnemyMovementTest.h"
#include <Core\IEngine.h>
#include <Gameplay/Flowfield.h>
#include <Gameplay/EnemyUnit.h>
#include <Gameplay/GameBlackboard.h>
#include <chrono>
#include <Profiler.h>
#include "Gameplay/EnemyFactory.h"
#include <Gameplay\Game.h>
#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib, "gameplayD.lib")
#else
#pragma comment(lib, "gameplay.lib")
#endif


SE::Test::EnemyMovementTest::EnemyMovementTest()
{

}

SE::Test::EnemyMovementTest::~EnemyMovementTest()
{

}

bool SE::Test::EnemyMovementTest::Run(SE::DevConsole::IConsole* console)
{
	StartProfile;
	auto engine = Core::CreateEngine();
	engine->Init();
	Gameplay::Game game;
	game.Initiate(engine);
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();

	auto floor = managers.entityManager->Create();
	auto player = managers.entityManager->Create();
	const int numberOfBlocks = 25 * 25 * 3;
	SE::Core::Entity entities[numberOfBlocks];

	for (int i = 0; i < numberOfBlocks; i++)
		entities[i] = managers.entityManager->Create();

	managers.transformManager->Create(player);
	managers.transformManager->SetPosition(player, DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));
	managers.transformManager->SetScale(player, 1.5f);
	managers.transformManager->Create(floor);
	managers.transformManager->SetPosition(floor, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));

	for (int i = 0; i < numberOfBlocks; i++)
		managers.transformManager->Create(entities[i]);



	managers.renderableManager->CreateRenderableObject(floor, {"Placeholder_Floor.mesh"});
	managers.renderableManager->ToggleRenderableObject(floor, true);

	managers.renderableManager->CreateRenderableObject(player, { "Placeholder_Arrow.mesh" });
	managers.renderableManager->ToggleRenderableObject(player, true);
	managers.transformManager->SetRotation(player, -DirectX::XM_PIDIV2, 0, 0);

	auto Block = Utilz::GUID("Placeholder_Block.mesh");
	auto Arrow = Utilz::GUID("Placeholder_Arrow.mesh");



	/*Place out the level*/
	char mapRepresentation[25][25] =
	{
		{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
		{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 },
		{ 5, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 },
		{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 },
		{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 }
	};
	int numberOfEntitesPlaced = 0;
	pos playerPos;
	playerPos.x = 1.5f;
	playerPos.y = 1.5f;

	SE::Gameplay::FlowField flowField(mapRepresentation, 1.0f, playerPos, 0.0f, 0.0f);
	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			if (mapRepresentation[x][y])
			{
				for (int i = 0; i < mapRepresentation[x][y]; i++)
				{
					managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Block });
					managers.renderableManager->ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
					managers.transformManager->SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x + 0.5f, 0.5f + 1.f*i, y + 0.5f));
					numberOfEntitesPlaced++;
				}
				mapRepresentation[x][y] = 1.0f;
			}
			else
			{
				
			}
		}
	}
	Gameplay::EnemyUnit* enemies[10];
	Gameplay::EnemyFactory eFactory;
	auto enemyGUID = Utilz::GUID("FlowFieldEnemy.SEC");
	eFactory.LoadEnemyIntoMemory(enemyGUID);
	Gameplay::GameBlackboard blackBoard;
	blackBoard.roomFlowField = &flowField;
	Gameplay::EnemyFactory::EnemyToCreateDescription desc;
	desc.GUID = enemyGUID;
	desc.nrOfEnemiesWithThisDescription = 10;
	desc.useVariation = true;
	eFactory.CreateEnemies(&desc, &blackBoard, 10, enemies);
	for (int i = 0; i < 10; i++)
	{
		pos enemyPos;
		do
		{
			enemyPos.x = rand() % 25;
			enemyPos.y = rand() % 25;
		} while (mapRepresentation[int(enemyPos.x)][int(enemyPos.y)]);

		enemies[i]->SetXPosition(enemyPos.x + .5f);
		enemies[i]->SetYPosition(enemyPos.y + .5f);

		//new Gameplay::enemies[i]Unit(testRoom->GetFlowFieldMap(), enemies[i]Pos.x + .5f, enemies[i]Pos.y + .5f, 10.0f);
		managers.renderableManager->CreateRenderableObject(enemies[i]->GetEntity(), { Block });
		managers.renderableManager->ToggleRenderableObject(enemies[i]->GetEntity(), true);
		managers.transformManager->SetRotation(enemies[i]->GetEntity(), -DirectX::XM_PIDIV2, 0, 0);
		managers.transformManager->SetScale(enemies[i]->GetEntity(), 0.5f);
	}



	subSystem.window->MapActionButton(0, Window::KeyEscape);
	subSystem.window->MapActionButton(1, Window::Key1);

	bool running = true;
	//unsigned char counter = 0;
	float dt = 1.0f/60.0f;
	while (running)
	{

		/*Only thing needed right now*/
		blackBoard.deltaTime = dt;
		if (subSystem.window->ButtonPressed(0))
			running = false;
		if(subSystem.window->ButtonPressed(1))
		{
			do
			{
				playerPos.x = rand() % 25;
				playerPos.y = rand() % 25;
			} while (mapRepresentation[int(playerPos.x)][int(playerPos.y)]);
			managers.transformManager->SetPosition(player, { playerPos.x + 0.5f, 1.5f, playerPos.y + 0.5f });
			flowField.Update(playerPos);
		}
		for(int i = 0; i < 10; i++)
		{
			enemies[i]->Update(dt);
		}

			
		engine->Frame();
	}

	for (int i = 0; i < 10; i++)
		delete enemies[i];

	game.Shutdown();
	engine->Release(); delete engine;
	ProfileReturnConst(true)
}
