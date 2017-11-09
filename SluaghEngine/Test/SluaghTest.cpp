#include "SlaughTest.h"
#include <Core/IEngine.h>
#include <Gameplay/EnemyUnit.h>
#include <Gameplay/Room.h>
#include <Gameplay/ProjectileData.h>
#include <Gameplay/ProjectileManager.h>
#include <chrono>
#include <Gameplay/PlayerUnit.h>
#include <Profiler.h>
#include <Utilz\Tools.h>
#include "Gameplay/GameBlackboard.h"
#include "Gameplay/EnemyFactory.h"
#include "Gameplay/PlayerPossibleActions.h"
#include "Gameplay/Game.h"
#include "../Gameplay/CoreInit.h"

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


SE::Test::SlaughTest::SlaughTest()
{
}

SE::Test::SlaughTest::~SlaughTest()
{
}

bool SE::Test::SlaughTest::Run(SE::DevConsole::IConsole* console)
{
	StartProfile;

	using namespace DirectX;

	Core::IEngine::InitializationInfo info;

	info.subSystems.optionsHandler = Core::CreateOptionsHandler();
	info.subSystems.optionsHandler->Initialize("Config.ini");

	float width = info.subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 800);
	float height = info.subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 600);

	Window::InitializationInfo winInfo;
	winInfo.winState = Window::WindowState::Playback;
	winInfo.height = height;
	winInfo.width = width;
	winInfo.windowTitle = "Recording";
	winInfo.fullScreen = false;

	info.subSystems.window = Window::CreateNewWindow();
	info.subSystems.window->Initialize(winInfo);

	auto engine = Core::CreateEngine();
	engine->Init(info);
	Gameplay::CoreInit::Init(engine);
	
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();

	uint32_t nrOfRooms;
	Utilz::GUID* RoomArr;

	subSystem.resourceHandler->LoadResource("RoomGeneration.txt", [&nrOfRooms, &RoomArr](auto GUID, auto data, auto size)
	{
		nrOfRooms = *(uint32_t *)data;
		RoomArr = new Utilz::GUID[nrOfRooms];
		memcpy(RoomArr, (char*)data + sizeof(uint32_t), sizeof(Utilz::GUID) * nrOfRooms);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});

	int random = subSystem.window->GetRand() % nrOfRooms;

	Gameplay::Room* testRoom = new Gameplay::Room(RoomArr[random]);

	Gameplay::PlayerUnit* player = nullptr;
	char map[25][25];
	testRoom->GetMap(map);
	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			if (map[x][y] == 32)
			{
				float rotation = ceilf((testRoom->FloorCheck(x, y) * (180 / 3.1416) - 270) - 0.5f);
				int xOffset = 0, yOffset = 0;
				if (rotation == 0)
				{
					yOffset = 1;
				}
				else if (rotation == 90)
				{
					xOffset = 1;
				}
				else if (rotation == 180)
				{
					yOffset = -1;
				}
				else if (rotation == 270)
				{
					xOffset = -1;
				}
				player = new Gameplay::PlayerUnit(nullptr, nullptr, x + (0.5f + xOffset), y + (0.5f + yOffset), map);
				player->SetZPosition(1.5f);
				break;
			}
		}
	}

	managers.transformManager->SetScale(player->GetEntity(), 1.f);
	//managers.renderableManager->CreateRenderableObject(player->GetEntity(), { "MCModell.mesh" });

	//managers.renderableManager->ToggleRenderableObject(player->GetEntity(), true);
	managers.transformManager->SetRotation(player->GetEntity(), 0, 0, 0);


	//Create a default light

	auto dummy = Gameplay::CoreInit::managers.entityManager->Create();
	Gameplay::CoreInit::managers.transformManager->Create(dummy, { 12.5, 3, 12.5 });
	Gameplay::CoreInit::managers.renderableManager->CreateRenderableObject(dummy, { "Placeholder_Block.mesh" });
	Gameplay::CoreInit::managers.renderableManager->ToggleRenderableObject(dummy, true);

	SE::Core::ILightManager::CreateInfo lightInfo;
	lightInfo.pos = { 0.0f, 0.0f, 0.0f };
	lightInfo.color = { 1.0f, 0.5f, .75f };
	lightInfo.radius = 10000.0f;

	Gameplay::CoreInit::managers.lightManager->Create(dummy, lightInfo);
	Gameplay::CoreInit::managers.lightManager->ToggleLight(dummy, true);

	SE::Core::Entity camera = managers.entityManager->Create();

	Core::ICameraManager::CreateInfo cInfo;
	cInfo.aspectRatio = (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800) / (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 640);

	managers.cameraManager->Create(camera, cInfo);
	managers.cameraManager->SetActive(camera);

	float cameraRotationX = DirectX::XM_PI / 3;
	float cameraRotationY = DirectX::XM_PI / 3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	player->UpdatePlayerRotation(cameraRotationX, cameraRotationY);
	managers.transformManager->BindChild(player->GetEntity(), camera, false, true);
	managers.transformManager->Move(camera, -5 * cameraTranslation);
	managers.transformManager->SetRotation(camera, cameraRotationX, cameraRotationY, 0);

	SE::Gameplay::BehaviourPointers temp;
	temp.currentRoom = &testRoom;
	temp.player = player;

	SE::Gameplay::ProjectileManager* projectileManager = new SE::Gameplay::ProjectileManager(temp);


	Gameplay::EnemyFactory eFactory;
	Gameplay::EnemyCreationStruct eStruct;
	Gameplay::GameBlackboard blackBoard;
	blackBoard.roomFlowField = testRoom->GetFlowFieldMap();
	
	const int enemiesSize = 2;
	Gameplay::EnemyUnit** enemies;

	enemies = new Gameplay::EnemyUnit*[enemiesSize];

	for (int i = 0; i < enemiesSize; i++)
	{
		pos enemyPos;
		do
		{
			enemyPos.x = subSystem.window->GetRand() % 25;
			enemyPos.y = subSystem.window->GetRand() % 25;
		} while (map[int(enemyPos.x)][int(enemyPos.y)]);

		Gameplay::EnemyCreationData data;
		data.type = Gameplay::EnemyType::ENEMY_TYPE_GLAISTIG;
		data.startX = enemyPos.x;
		data.startY = enemyPos.y;
		data.useVariation = true;
		eStruct.information.push_back(data);
	}

	

	eFactory.CreateEnemies(eStruct, &blackBoard, enemies);
	
	for (int i = 0; i < enemiesSize; i++)
	{
		testRoom->AddEnemyToRoom(enemies[i]);
	}
	delete[] enemies;
	subSystem.window->MapActionButton(0, Window::KeyEscape);
	subSystem.window->MapActionButton(1, Window::Key1);
	subSystem.window->MapActionButton(2, Window::Key2);

	enum MoveDir
	{
		UP = 3,
		RIGHT = 4,
		DOWN = 5,
		LEFT = 6,
		RIGHT_MOUSE = 7,
		SPACE = 8,
		CONSOLE = 9
	};


	subSystem.window->MapActionButton(UP, Window::KeyW);
	subSystem.window->MapActionButton(RIGHT, Window::KeyD);
	subSystem.window->MapActionButton(DOWN, Window::KeyS);
	subSystem.window->MapActionButton(LEFT, Window::KeyA);
	subSystem.window->MapActionButton(RIGHT_MOUSE, Window::MouseRight);
	subSystem.window->MapActionButton(SPACE, Window::KeySpace);
	subSystem.window->MapActionButton(CONSOLE, Window::Key1);

	pos playerPos;
	playerPos.x = 1.5f;
	playerPos.y = 1.5f;

	std::vector<Gameplay::ProjectileData> newProjectiles;
	
	bool stepping = false;
	bool running = true;



	auto SpawnEnemy = [&testRoom, &eFactory, &blackBoard, &subSystem]() mutable -> void
	{
		if(testRoom->NumberOfEnemiesInRoom() < 2)
		{
			char map[25][25];
			testRoom->GetMap(map);
			Gameplay::EnemyCreationStruct eStruct;
			blackBoard.roomFlowField = testRoom->GetFlowFieldMap();

			const int enemiesSize = 1;
			Gameplay::EnemyUnit** enemies;

			enemies = new Gameplay::EnemyUnit*[enemiesSize];

			for (int i = 0; i < enemiesSize; i++)
			{
				pos enemyPos;
				do
				{
					enemyPos.x = subSystem.window->GetRand() % 25;
					enemyPos.y = subSystem.window->GetRand() % 25;
				} while (map[int(enemyPos.x)][int(enemyPos.y)]);

				Gameplay::EnemyCreationData data;
				data.type = Gameplay::EnemyType::ENEMY_TYPE_GLAISTIG;
				data.startX = enemyPos.x;
				data.startY = enemyPos.y;
				data.useVariation = true;
				eStruct.information.push_back(data);
			}



			eFactory.CreateEnemies(eStruct, &blackBoard, enemies);

			for (int i = 0; i < enemiesSize; i++)
			{
				testRoom->AddEnemyToRoom(enemies[i]);
			}
			delete[] enemies;
		}
	};
	blackBoard.currentRoom = testRoom;
	float dt = 1 / 60.0f;//subSystem.window->GetDelta();
	while (running)
	{
		dt = subSystem.window->GetDelta();
		SpawnEnemy();
		newProjectiles.clear();

		Gameplay::PlayerUnit::MovementInput input(false, false, false, false, false, 0.0f, 0.0f);
		Gameplay::PlayerUnit::ActionInput actionInput(false, false, false);
		float movX = 0.0f;
		float movY = 0.0f;



		int mX = 0;
		int mY = 0;
		DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR rayD;

		subSystem.window->GetMousePos(mX, mY);
		managers.cameraManager->WorldSpaceRayFromScreenPos(mX, mY, width, height, rayO, rayD);

		float distance = XMVectorGetY(rayO) / -XMVectorGetY(rayD);

		auto clickPos = rayO + rayD*distance;

		input.mousePosX = DirectX::XMVectorGetX(clickPos);
		input.mousePosY = DirectX::XMVectorGetZ(clickPos);

		if (subSystem.window->ButtonDown(MoveDir::UP))
		{
			input.upButton = true;
			movY += 1.0f;
		}
		if (subSystem.window->ButtonDown(MoveDir::DOWN))
		{
			input.downButton = true;
			movY -= 1.0f;
		}
		if (subSystem.window->ButtonDown(MoveDir::RIGHT))
		{
			input.rightButton = true;
			movX += 1.0f;
		}
		if (subSystem.window->ButtonDown(MoveDir::LEFT))
		{
			input.leftButton = true;
			movX -= 1.0f;
		}

		if (subSystem.window->ButtonDown(MoveDir::RIGHT_MOUSE))
		{
			actionInput.actionButton = true;
		}
		
		SE::Gameplay::PlayerMovementActions playerMovement;
		SE::Gameplay::PlayerAttackActions playerAttack = SE::Gameplay::PlayerAttackActions::PLAYER_ATTACK_NONE;
		

		float totMov = abs(movX) + abs(movY);
		if (totMov != 0.f)
		{
			movX /= totMov;
			movY /= totMov;
		}
		player->Update(dt * 5, input,  newProjectiles, actionInput);

		projectileManager->AddProjectiles(newProjectiles);

		projectileManager->UpdateProjectilePositions(dt);
		testRoom->CheckProjectileCollision(projectileManager->GetAllProjectiles());
		projectileManager->UpdateProjectileActions(dt);


		playerPos.x = player->GetXPosition();
		playerPos.y = player->GetYPosition();

		/*Only thing needed right now*/
		blackBoard.deltaTime = dt;
		blackBoard.playerPositionX = playerPos.x;
		blackBoard.playerPositionY = playerPos.y;


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
		else
		{
			testRoom->Update(dt, playerPos.x, playerPos.y);
		}
		projectileManager->AddProjectiles(blackBoard.enemyProjectiles);
		blackBoard.enemyProjectiles.clear();
		engine->BeginFrame();

		engine->EndFrame();


	}
	

	delete projectileManager;

	delete testRoom;
	delete player;
	delete RoomArr;


	engine->Release(); delete engine;


	ProfileReturnConst(true)
}
