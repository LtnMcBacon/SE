#include "RoomCreationTest.h"
#include <Core\Engine.h>
#include <Gameplay/EnemyUnit.h>
#include <Gameplay/Room.h>
#include <chrono>

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

bool SE::Test::RoomCreationTest::Run(SE::Utilz::IConsoleBackend* console)
{
	auto& e = Core::Engine::GetInstance();
	auto& info = Core::Engine::InitializationInfo();
	auto re = e.Init(info);
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		return false;
	}

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();

	auto floor = em.Create();
	auto player = em.Create();
	const int numberOfBlocks = 25 * 25;
	SE::Core::Entity entities[numberOfBlocks];

	for (int i = 0; i < numberOfBlocks; i++)
		entities[i] = em.Create();

	tm.Create(player);
	tm.SetPosition(player, DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));
	tm.SetScale(player, 1.5f);
	tm.Create(floor);
	tm.SetPosition(floor, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));

	for (int i = 0; i < numberOfBlocks; i++)
		tm.Create(entities[i]);



	rm.CreateRenderableObject(floor, Utilz::GUID("Placeholder_Floor.obj"));
	rm.ToggleRenderableObject(floor, true);

	rm.CreateRenderableObject(player, Utilz::GUID("Placeholder_Arrow.obj"));
	rm.ToggleRenderableObject(player, true);
	tm.SetRotation(player, -DirectX::XM_PIDIV2, 0, 0);

	auto Block = Utilz::GUID("Placeholder_Block.obj");
	auto Arrow = Utilz::GUID("Placeholder_Arrow.obj");




	/*Place out the level*/
	char mapRepresentation[25][25] =
	{
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
	int numberOfEntitesPlaced = 0;

	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			if (mapRepresentation[x][y])
			{
				rm.CreateRenderableObject(entities[numberOfEntitesPlaced], Block);
				rm.ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
				tm.SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x + 0.5f, 0.5f, y + 0.5f));
				numberOfEntitesPlaced++;
			}
			else
			{

			}
		}
	}
	Gameplay::Room* testRoom = new Gameplay::Room(mapRepresentation);
	
	for (int i = 0; i < 10; i++)
	{
		pos enemyPos;
		do
		{
			enemyPos.x = rand() % 25;
			enemyPos.y = rand() % 25;
		} while (mapRepresentation[int(enemyPos.x)][int(enemyPos.y)]);
		Gameplay::EnemyUnit* enemy = new Gameplay::EnemyUnit(testRoom->GetFlowFieldMap(), enemyPos.x + .5f, enemyPos.y + .5f, 10.0f);
		rm.CreateRenderableObject(enemy->GetEntity(), Utilz::GUID("Placeholder_Arrow.obj"));
		rm.ToggleRenderableObject(enemy->GetEntity(), true);
		tm.SetRotation(enemy->GetEntity(), -DirectX::XM_PIDIV2, 0, 0);
		testRoom->AddEnemyToRoom(enemy);
	}



	e.GetWindow()->MapActionButton(0, Window::KeyEscape);

	pos playerPos;
	playerPos.x = 1.5f;
	playerPos.y = 1.5f;

	bool running = true;
	unsigned char counter = 0;
	float dt = 1.0f / 60.0f;
	while (running)
	{

		if (e.GetWindow()->ButtonPressed(0))
			running = false;
		testRoom->Update(dt, playerPos.x, playerPos.y);


		e.Frame();
	}

	delete testRoom;

	e.Release();

	return true;
}
