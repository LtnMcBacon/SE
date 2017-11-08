#include "RoomCreationTest.h"
#include <Core\IEngine.h>
#include <Gameplay/EnemyUnit.h>
#include <Gameplay/Room.h>
#include <chrono>
#include <Profiler.h>
#include "Gameplay/GameBlackboard.h"
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


SE::Test::RoomCreationTest::RoomCreationTest()
{

}

SE::Test::RoomCreationTest::~RoomCreationTest()
{

}

bool SE::Test::RoomCreationTest::Run(SE::DevConsole::IConsole* console)
{
	StartProfile;
	auto engine = Core::CreateEngine();
	Gameplay::Game game;
	engine->Init();
	game.Initiate(engine);
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();

	auto floor = managers.entityManager->Create();
	auto player = managers.entityManager->Create();
	const int numberOfBlocks = 25 * 25;
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

	auto Block = Utilz::GUID("HighWall.mesh");
	auto Arrow = Utilz::GUID("Placeholder_Arrow.mesh");




	/*Place out the level*/
	char mapRepresentation[25][25]; //=
	//{
	//	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	//};


	int numberOfEntitesPlaced = 0;

	Gameplay::Room* testRoom = new Gameplay::Room("");
	
	char map[25][25];
	testRoom->GetMap(map);

	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			if (map[x][y])
			{
				managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Block });
				managers.renderableManager->ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
				managers.transformManager->SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x + 0.5f, 0.5f, y + 0.5f));
				numberOfEntitesPlaced++;
			}
			else
			{
				managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Arrow });
				managers.renderableManager->ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
				float xMagnitude = 0.0f;
				float zMagnitude = 0.0f;
				pos enemyPos;
				enemyPos.x = x + 0.5f;
				enemyPos.y = y + 0.5f;
				testRoom->GetFlowFieldMap()->SampleFromMap(enemyPos, xMagnitude, zMagnitude);
				managers.transformManager->SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x+0.5f, 0.5f, y + 0.5f));
				managers.transformManager->SetScale(entities[numberOfEntitesPlaced], 0.25f);
				if (xMagnitude == 1.0f)
				{
					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
				}
				else if (xMagnitude == -1.0f)
				{
					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PIDIV2, 0.0f);
				}
				else if (zMagnitude == 1.0f)
				{
					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI, 0.0f);
				}
				else if (zMagnitude == -1.0f)
				{
					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, 0.0f, 0.0f);

				}
				else if (xMagnitude == 0.707f)
				{
					if (zMagnitude == 0.707f)
					{
						managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
					}
					else if (zMagnitude == -0.707f)
					{
						managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
					}
				}
				else if (xMagnitude == -0.707f)
				{
					if (zMagnitude == 0.707f)
					{
						managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0f);
					}
					else if (zMagnitude == -0.707f)
					{
						managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PIDIV4, 0.0f);
					}
				}
				numberOfEntitesPlaced++;
			}
		}
	}
	
	Gameplay::EnemyFactory eFactory;
	auto enemyGUID = Utilz::GUID("FlowFieldEnemy.SEC");
	eFactory.LoadEnemyIntoMemory(enemyGUID);
	Gameplay::GameBlackboard blackBoard;

	testRoom->GetMap(mapRepresentation);
	blackBoard.roomFlowField = testRoom->GetFlowFieldMap();
	for (int i = 0; i < 100; i++)
	{
		pos enemyPos;
		do
		{
			enemyPos.x = subSystem.window->GetRand() % 25;
			enemyPos.y = subSystem.window->GetRand() % 25;
		} while (mapRepresentation[int(enemyPos.x)][int(enemyPos.y)]);

		Gameplay::EnemyUnit* enemy = eFactory.CreateEnemy(enemyGUID, &blackBoard);
		enemy->SetXPosition(enemyPos.x + .5f);
		enemy->SetYPosition(enemyPos.y + .5f);

		//new Gameplay::EnemyUnit(testRoom->GetFlowFieldMap(), enemyPos.x + .5f, enemyPos.y + .5f, 10.0f);
		managers.renderableManager->CreateRenderableObject(enemy->GetEntity(), { Block });
		managers.renderableManager->ToggleRenderableObject(enemy->GetEntity(), true);
		managers.transformManager->SetRotation(enemy->GetEntity(), -DirectX::XM_PIDIV2, 0, 0);
		managers.transformManager->SetScale(enemy->GetEntity(), 0.5f);
		testRoom->AddEnemyToRoom(enemy);
	}

	subSystem.window->MapActionButton(0, Window::KeyEscape);
	subSystem.window->MapActionButton(1, Window::Key1);
	subSystem.window->MapActionButton(2, Window::Key2);
	pos playerPos;
	playerPos.x = 1.5f;
	playerPos.y = 1.5f;

	bool stepping = false;
	bool running = true;
	/*unsigned char counter = 0;*/
	float dt = 1.0f / 60.0f;
	while (running)
	{
		/*Only thing needed right now*/
		blackBoard.deltaTime = dt;

		if (subSystem.window->ButtonPressed(0))
			running = false;
		if (subSystem.window->ButtonPressed(1))
		{
			stepping = !stepping;
		}

		if (stepping)
		{
			if (subSystem.window->ButtonDown(2))
			{
				testRoom->Update(dt, playerPos.x, playerPos.y);
			}
		}
		else {
			testRoom->Update(dt, playerPos.x, playerPos.y);
		}
		engine->BeginFrame();
		engine->EndFrame();
	}

	delete testRoom;

	game.Shutdown();
	engine->Release(); delete engine;

	ProfileReturnConst(true)
}
