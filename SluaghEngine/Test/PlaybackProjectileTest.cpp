#include "PlaybackProjectileTest.h"
#include <Core\IEngine.h>
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
#include <Gameplay\Game.h>
#include <Imgui\imgui.h>
#include <Utilz\PathParsing.h>

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

#undef PlaySound

SE::Test::PlaybackProjectileTest::PlaybackProjectileTest()
{

}

SE::Test::PlaybackProjectileTest::~PlaybackProjectileTest()
{

}

bool SE::Test::PlaybackProjectileTest::Run(SE::DevConsole::IConsole* console)
{
	StartProfile;

	auto lam = [](std::string file, Window::WindowState state)
	{
		using namespace DirectX;
		auto engine = Core::CreateEngine();


		Core::IEngine::InitializationInfo info;

		info.subSystems.optionsHandler = Core::CreateOptionsHandler();
		info.subSystems.optionsHandler->Initialize("Config.ini");

		float width = info.subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 800);
		float height = info.subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 600);

		Window::InitializationInfo winInfo;
		winInfo.file = file;
		winInfo.winState = state;
		winInfo.height = height;
		winInfo.width = width;
		winInfo.windowTitle = "Playback";
		winInfo.fullScreen = false;

		info.subSystems.window = Window::CreateNewWindow();
		info.subSystems.window->Initialize(winInfo);
		Gameplay::Game game;
		engine->Init(info);
		game.Initiate(engine);
		auto managers = engine->GetManagers();
		auto subSystem = engine->GetSubsystems();

		/*Core::IMaterialManager::CreateInfo floorInfo;
		Utilz::GUID material = Utilz::GUID("Cube.mat");
		Utilz::GUID shader = Utilz::GUID("SimpleNormMapPS.hlsl");
		floorInfo.shader = shader;
		floorInfo.materialFile = material;
		auto floor = managers.entityManager->Create();
		managers.materialManager->Create(floor, floorInfo);*/
		const int numberOfBlocks = 25 * 25;
		SE::Core::Entity entities[numberOfBlocks];
		SE::Core::Entity arrows[numberOfBlocks];
		SE::Core::Entity floor[numberOfBlocks];

		Core::IMaterialManager::CreateInfo arrowInfo;
		Utilz::GUID material = Utilz::GUID("Cube.mat");
		Utilz::GUID shader = Utilz::GUID("SimpleNormMapPS.hlsl");
		arrowInfo.shader = shader;
		arrowInfo.materialFile = material;

		for (int i = 0; i < numberOfBlocks; i++)
		{
			entities[i] = managers.entityManager->Create();
			arrows[i] = managers.entityManager->Create();
			floor[i] = managers.entityManager->Create();
			managers.materialManager->Create(arrows[i], arrowInfo);
		}
		/*managers.transformManager->Create(floor);
		managers.transformManager->SetPosition(floor, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));*/

		for (int i = 0; i < numberOfBlocks; i++)
		{
			managers.transformManager->Create(entities[i]);
			managers.transformManager->Create(arrows[i]);
			managers.transformManager->Create(floor[i]);
		}


		/*managers.renderableManager->CreateRenderableObject(floor, { "Placeholder_Floor.mesh" });
		managers.renderableManager->ToggleRenderableObject(floor, true);

		managers.collisionManager->CreateBoundingHierarchy(floor, Utilz::GUID("Placeholder_Floor.mesh"));*/


#pragma region AudioData
		managers.audioManager->SetCameraEnt(managers.cameraManager->GetActive());
		auto soundEnt = managers.entityManager->Create();
		managers.transformManager->Create(soundEnt, DirectX::XMFLOAT3(22.5, 1.0, 22.5));
		Core::IAudioManager::CreateInfo ausioInfo1;
		ausioInfo1.soundFile = Utilz::GUID("BLoop.wav");
		ausioInfo1.soundType = Audio::SoundIndexName::StereoPanLoopSound;
		managers.audioManager->Create(soundEnt, ausioInfo1);
		Core::IAudioManager::CreateInfo ausioInfo2;
		ausioInfo2.soundFile = Utilz::GUID("BLoop2.wav");
		ausioInfo2.soundType = Audio::SoundIndexName::StereoPanLoopSound;
		managers.audioManager->Create(soundEnt, ausioInfo2);

		managers.audioManager->PlaySound(soundEnt, Utilz::GUID("BLoop.wav"));
#pragma endregion AudioData


#pragma region LightDataSet
		Core::Entity light[5];
		for (int i = 0; i < 5; i++)
		{
			light[i] = managers.entityManager->Create();
		}

		Core::ILightManager::CreateInfo data;
		//Light 1
		data.color = DirectX::XMFLOAT3(0.3, 0.1, 0.1);
		data.pos = DirectX::XMFLOAT3(22.5, 1.0, 22.5);
		data.radius = 10.0f;
		managers.lightManager->Create(light[0], data);
		managers.lightManager->ToggleLight(light[0], true);

		//Light 2
		data.color = DirectX::XMFLOAT3(0.1, 0.3, 0.1);
		data.pos = DirectX::XMFLOAT3(2.5, 1.0, 22.5);
		data.radius = 10.0;
		managers.lightManager->Create(light[1], data);
		managers.lightManager->ToggleLight(light[1], true);

		//Light 3
		data.color = DirectX::XMFLOAT3(0.1, 0.1, 0.3);
		data.pos = DirectX::XMFLOAT3(22.5, 1.0, 2.5);
		data.radius = 10.0;
		managers.lightManager->Create(light[2], data);
		managers.lightManager->ToggleLight(light[2], true);

		//Light 4
		data.color = DirectX::XMFLOAT3(0.3, 0.1, 0.1);
		data.pos = DirectX::XMFLOAT3(2.5, 1.0, 2.5);
		data.radius = 10.0;
		managers.lightManager->Create(light[3], data);
		managers.lightManager->ToggleLight(light[3], true);

		//Light 5
		data.color = DirectX::XMFLOAT3(0.1, 0.1, 0.1);
		data.pos = DirectX::XMFLOAT3(12.5, 40.0, 12.5);
		data.radius = 150.0;
		managers.lightManager->Create(light[4], data);
		managers.lightManager->ToggleLight(light[4], true);
#pragma endregion LightDataSet

		auto Block = SE::Utilz::GUID{ "HighWall.mesh" };
		auto Passage = SE::Utilz::GUID{ "HighWall_Passage.mesh" };
		auto OneSide = SE::Utilz::GUID{ "HighWall_OneSide.mesh" };
		auto Corner = SE::Utilz::GUID{ "HighWall_Corner.mesh" };
		auto Top = SE::Utilz::GUID{ "HighWall_Top.mesh" };
		auto ThreeSides = SE::Utilz::GUID{ "HighWall_ThreeSides.mesh" };
		auto Arrow = SE::Utilz::GUID{ "Placeholder_Arrow.mesh" };
		auto Door = SE::Utilz::GUID{ "Door.mesh" };
		auto Floor = SE::Utilz::GUID{ "floorTest.mesh" };

		int numberOfEntitesPlaced = 0;
		int numberOfArrows = 0;
		int numberOfFloor = 0;
		uint32_t nrOfRooms;
		Utilz::GUID* RoomArr;

		subSystem.resourceHandler->LoadResource("RoomGeneration.txt", [&nrOfRooms, &RoomArr](auto GUID, auto data, auto size)
		{
			nrOfRooms = *(uint32_t *)data;
			RoomArr = new Utilz::GUID[nrOfRooms];
			memcpy(RoomArr, (char*)data + sizeof(uint32_t), sizeof(Utilz::GUID) * nrOfRooms);
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		});

		int random = subSystem.window->GetRand() % nrOfRooms;

		Gameplay::Room* testRoom = new Gameplay::Room(RoomArr[random]);

		Gameplay::PlayerUnit* player = nullptr;
		for (int x = 0; x < 25; x++)
		{
			for (int y = 0; y < 25; y++)
			{
				if (testRoom->tileValues[x][y] == 1)
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
					player = new Gameplay::PlayerUnit(nullptr, nullptr, x + (0.5f + xOffset), y + (0.5f + yOffset), testRoom->tileValues);

					break;
				}
			}
		}
		if (player == nullptr)
		{
			delete testRoom;
			delete player;
			delete[] RoomArr;
			game.Shutdown();
			engine->Release();
			delete engine;
			ProfileReturnConst(false);
		}


		Core::IMaterialManager::CreateInfo playerInfo;
		material = Utilz::GUID("MCModell.mat");
		shader = Utilz::GUID("SimpleLightPS.hlsl");
		playerInfo.shader = shader;
		playerInfo.materialFile = material;

		managers.materialManager->Create(player->GetEntity(), playerInfo);
		managers.transformManager->SetScale(player->GetEntity(), 1.f);
		managers.renderableManager->CreateRenderableObject(player->GetEntity(), { Utilz::GUID("MCModell.mesh") });

		managers.renderableManager->ToggleRenderableObject(player->GetEntity(), true);
		managers.transformManager->SetRotation(player->GetEntity(), 0, 0, 0);

		SE::Core::Entity camera = managers.cameraManager->GetActive();
		Core::ICameraManager::CreateInfo cInfo;
		cInfo.aspectRatio = (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800) / (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 640);

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

		Core::IMaterialManager::CreateInfo cubeInfo;
		material = Utilz::GUID("Cube.mat");
		cubeInfo.materialFile = material;

		

		for (int x = 0; x < 25; x++)
		{
			for (int y = 0; y < 25; y++)
			{
				if (testRoom->tileValues[x][y] == 10)
				{

					int right = testRoom->tileValues[x + 1][y];
					int left = testRoom->tileValues[x - 1][y];
					int up = testRoom->tileValues[x][y + 1];
					int down = testRoom->tileValues[x][y - 1];
					
					if (x == 24 || y == 24 || x == 0 || y == 0)
					{
						if ((x == 0 && y == 0) || (x == 24 && y == 0) || (x == 0 && y == 24) || (x == 24 && y == 24))
						{
							managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Top });
						}
						else if ((x == 24 && y != 0))
						{
							if (testRoom->tileValues[x - 1][y] == 10)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Top });
							}
							else if (testRoom->tileValues[x][y - 1] == 1 || testRoom->tileValues[x][y - 1] == 2)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, 0.0, 0.0);
							}
							else if (testRoom->tileValues[x][y + 1] == 1 || testRoom->tileValues[x][y + 1] == 2)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PIDIV2, 0.0);
							}
							else
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { OneSide });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PI, 0.0);
							}
						}
						else if ((y == 24 && x != 0))
						{
							if (testRoom->tileValues[x][y - 1] == 10)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Top });
							}
							else if (testRoom->tileValues[x - 1][y] == 1 || testRoom->tileValues[x - 1][y] == 2)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, 0.0, 0.0);
							}
							else if (testRoom->tileValues[x + 1][y] == 1 || testRoom->tileValues[x + 1][y] == 2)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, -DirectX::XM_PIDIV2, 0.0);
							}
							else
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { OneSide });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PIDIV2, 0.0);
							}
						}
						else if ((x == 0 && y != 24))
						{
							if (testRoom->tileValues[x + 1][y] == 10)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Top });
							}
							else if (testRoom->tileValues[x][y - 1] == 1 || testRoom->tileValues[x][y - 1] == 2)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, -DirectX::XM_PIDIV2, 0.0);
							}
							else if (testRoom->tileValues[x][y + 1] == 1 || testRoom->tileValues[x][y + 1] == 2)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PI, 0.0);
							}
							else
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { OneSide });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, 0.0, 0.0);
							}
						}
						else if ((y == 0 && x != 24))
						{
							if (testRoom->tileValues[x][y + 1] == 10)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Top });
							}
							else if (testRoom->tileValues[x - 1][y] == 1 || testRoom->tileValues[x - 1][y] == 2)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PIDIV2, 0.0);
							}
							else if (testRoom->tileValues[x + 1][y] == 1 || testRoom->tileValues[x + 1][y] == 2)
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PI, 0.0);
							}
							else
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { OneSide });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, -DirectX::XM_PIDIV2, 0.0);
							}
						}
					}
					else if (right + left + up + down == 40)
					{
						continue;
					}
					else
					{
						int side = 1;
						if (right == 10)
							side = side * 5;
						if (left == 10)
							side = side * 2;
						if (up == 10)
							side = side * 3;
						if (down == 10)
							side = side * 7;
						//    3
						//  2/#/5
						//    7

						switch (side) {
							case 1:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Block });
								break;
							}
							case 2:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { ThreeSides });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PI, 0.0);
								break;
							}
							case 3:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { ThreeSides });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, -DirectX::XM_PIDIV2, 0.0);
								break;
							}
							case 5:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { ThreeSides });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, 0.0, 0.0);
								break;
							}
							case 7:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { ThreeSides });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PIDIV2, 0.0);
								break;
							}
							case 10:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Passage });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PIDIV2, 0.0);
								break;
							}
							case 21:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Passage });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, 0.0, 0.0);
								break;
							}
							case 6:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, -DirectX::XM_PIDIV2, 0.0);
								break;
							}
							case 15:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, 0.0, 0.0);
								break;
							}
							case 35:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PIDIV2, 0.0);
								break;
							}
							case 14:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Corner });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PI, 0.0);
								break;
							}
							case 30:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { OneSide });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PIDIV2, 0.0);
								break;
							}
							case 105:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { OneSide });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, DirectX::XM_PI, 0.0);
								break;
							}
							case 70:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { OneSide });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, -DirectX::XM_PIDIV2, 0.0);
								break;
							}
							case 42:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { OneSide });
								managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0, 0.0, 0.0);
								break;
							}
							default:
							{
								managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Block });
								break;
							}
						}
							
					}
					
					
					if (!(x == 24 || y == 24) && (right == 0 || up == 0 || testRoom->tileValues[x + 1][y + 1] == 0))
					{
						shader = Utilz::GUID("SimpleNormTransPS.hlsl");
						managers.renderableManager->ToggleTransparency(entities[numberOfEntitesPlaced], true);
					}
					else
						shader = Utilz::GUID("SimpleNormMapPS.hlsl");
									
					cubeInfo.shader = shader;
					managers.materialManager->Create(entities[numberOfEntitesPlaced], cubeInfo);					
					managers.renderableManager->ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
					managers.transformManager->SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x + 0.5f, 1.0f, y + 0.5f));
					managers.transformManager->SetScale(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(1.0, 1.0, 1.0));
					numberOfEntitesPlaced++;
				}
				else if (testRoom->tileValues[x][y] == 2 || testRoom->tileValues[x][y] == 1)
				{
					managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Door });
					if (!(x == 24 || y == 24) && (testRoom->tileValues[x + 1][y] == 0 || testRoom->tileValues[x][y + 1] == 0 || testRoom->tileValues[x + 1][y + 1] == 0))
					{
						shader = Utilz::GUID("SimpleNormTransPS.hlsl");
						managers.renderableManager->ToggleTransparency(entities[numberOfEntitesPlaced], true);
					}
					else
						shader = Utilz::GUID("SimpleNormMapPS.hlsl");
					cubeInfo.shader = shader;
					managers.materialManager->Create(entities[numberOfEntitesPlaced], cubeInfo);
					managers.renderableManager->ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
					managers.transformManager->SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x + 0.5f, 0.5f, y + 0.5f));

					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, testRoom->FloorCheck(x, y), 0.0f);
					numberOfEntitesPlaced++;

					managers.renderableManager->CreateRenderableObject(floor[numberOfFloor], { Floor });
					shader = Utilz::GUID("SimpleNormMapPS.hlsl");
					cubeInfo.shader = shader;
					managers.materialManager->Create(floor[numberOfFloor], cubeInfo);
					managers.renderableManager->ToggleRenderableObject(floor[numberOfFloor], true);
					managers.transformManager->SetPosition(floor[numberOfFloor], DirectX::XMFLOAT3(x + 0.5f, 0.0f, y + 0.5f));

					numberOfFloor++;
				}
				else if (testRoom->tileValues[x][y] == 0)
				{
					managers.renderableManager->CreateRenderableObject(floor[numberOfFloor], { Floor });
					shader = Utilz::GUID("SimpleNormMapPS.hlsl");
					cubeInfo.shader = shader;
					managers.materialManager->Create(floor[numberOfFloor], cubeInfo);
					managers.renderableManager->ToggleRenderableObject(floor[numberOfFloor], true);
					managers.transformManager->SetPosition(floor[numberOfFloor], DirectX::XMFLOAT3(x + 0.5f, 0.0f, y + 0.5f));

					numberOfFloor++;
#pragma region Arrows



					managers.renderableManager->CreateRenderableObject(arrows[numberOfArrows], { Arrow });
					managers.renderableManager->ToggleRenderableObject(arrows[numberOfArrows], true);
					float xMagnitude = 0.0f;
					float zMagnitude = 0.0f;
					pos enemyPos;
					enemyPos.x = x + 0.5f;
					enemyPos.y = y + 0.5f;
					testRoom->GetFlowFieldMap()->SampleFromMap(enemyPos, xMagnitude, zMagnitude);
					managers.transformManager->SetPosition(arrows[numberOfArrows], DirectX::XMFLOAT3(x + 0.5f, 0.5f, y + 0.5f));
					managers.transformManager->SetScale(arrows[numberOfArrows], 0.25f);
					if (xMagnitude == 1.0f)
					{
						managers.transformManager->SetRotation(arrows[numberOfArrows], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
					}
					else if (xMagnitude == -1.0f)
					{
						managers.transformManager->SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PIDIV2, 0.0f);
					}
					else if (zMagnitude == 1.0f)
					{
						managers.transformManager->SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PI, 0.0f);
					}
					else if (zMagnitude == -1.0f)
					{
						managers.transformManager->SetRotation(arrows[numberOfArrows], 0.0f, 0.0f, 0.0f);

					}
					else if (xMagnitude == 0.707f)
					{
						if (zMagnitude == 0.707f)
						{
							managers.transformManager->SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
						}
						else if (zMagnitude == -0.707f)
						{
							managers.transformManager->SetRotation(arrows[numberOfArrows], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
						}
					}
					else if (xMagnitude == -0.707f)
					{
						if (zMagnitude == 0.707f)
						{
							managers.transformManager->SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0f);
						}
						else if (zMagnitude == -0.707f)
						{
							managers.transformManager->SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PIDIV4, 0.0f);
						}
					}

					numberOfArrows++;
#pragma endregion Arrows
				}
			}
		}
		Gameplay::EnemyFactory eFactory;
		Gameplay::EnemyCreationStruct eStruct;
		Gameplay::GameBlackboard blackBoard;
		blackBoard.roomFlowField = testRoom->GetFlowFieldMap();

		const int enemiesSize = 100;
		Gameplay::EnemyUnit** enemies;

		enemies = new Gameplay::EnemyUnit*[enemiesSize];

		for (int i = 0; i < enemiesSize; i++)
		{
			pos enemyPos;
			do
			{
				enemyPos.x = subSystem.window->GetRand() % 25;
				enemyPos.y = subSystem.window->GetRand() % 25;
			} while (testRoom->tileValues[int(enemyPos.x)][int(enemyPos.y)]);

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
			CONSOLE = 9,
			FULLSCREEN = 10,
			SIZE19 = 11,
			SIZE12 = 12,
			LEFT_MOUSE = 13
		};


		subSystem.window->MapActionButton(UP, Window::KeyW);
		subSystem.window->MapActionButton(RIGHT, Window::KeyD);
		subSystem.window->MapActionButton(DOWN, Window::KeyS);
		subSystem.window->MapActionButton(LEFT, Window::KeyA);
		subSystem.window->MapActionButton(RIGHT_MOUSE, Window::MouseRight);
		subSystem.window->MapActionButton(LEFT_MOUSE, Window::MouseLeft);
		subSystem.window->MapActionButton(SPACE, Window::KeySpace);
		subSystem.window->MapActionButton(CONSOLE, Window::Key1);
		subSystem.window->MapActionButton(FULLSCREEN, Window::KeyT);
		subSystem.window->MapActionButton(SIZE19, Window::KeyY);
		subSystem.window->MapActionButton(SIZE12, Window::KeyU);

		pos playerPos;
		playerPos.x = 1.5f;
		playerPos.y = 1.5f;

		std::vector<Gameplay::ProjectileData> newProjectiles;

		/*DirectX::XMFLOAT3 tPos = managers.transformManager->GetPosition(floor);
		DirectX::XMFLOAT3 tRot = managers.transformManager->GetRotation(floor);
		DirectX::XMFLOAT3 tScale = managers.transformManager->GetScale(floor);
		DirectX::XMMATRIX worldM = { tScale.x, 0, 0, 0,
			0, tScale.y, 0, 0,
			0, 0, tScale.z, 0,
			tPos.x, tPos.y, tPos.z, 1.0f };*/

		bool stepping = false;
		bool running = true;
		subSystem.window->UpdateTime();
		subSystem.window->UpdateTime();
		float audioTime = 0.0;
		int audio = 0;
		bool change = false;
		while (running)
		{
			float dt = subSystem.window->GetDelta();
			audioTime += dt;
			if (audioTime > 20.0)
			{
				audio = (audio + 1) % 2;
				if (audio == 0)
				{
					managers.audioManager->StopSound(soundEnt, Utilz::GUID("BLoop2.wav"));
					managers.audioManager->PlaySound(soundEnt, Utilz::GUID("BLoop.wav"));
				}
				else
				{
					managers.audioManager->StopSound(soundEnt, Utilz::GUID("BLoop.wav"));
					managers.audioManager->PlaySound(soundEnt, Utilz::GUID("BLoop2.wav"));
				}
				audioTime = 0.0;
			}
			if (subSystem.window->ButtonPressed(MoveDir::FULLSCREEN))
			{
				change = true;
				bool fs = subSystem.optionsHandler->GetOptionBool("Window", "fullScreen", true);
				if (fs == true)
					subSystem.optionsHandler->SetOptionBool("Window", "fullScreen", false);
				else
					subSystem.optionsHandler->SetOptionBool("Window", "fullScreen", true);
			}
			if (subSystem.window->ButtonPressed(MoveDir::SIZE19))
			{
				size_t height = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 1080);
				size_t width = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 1920);
				if (height != 1080 || width != 1920)
				{
					subSystem.optionsHandler->SetOptionUnsignedInt("Window", "height", 1080);
					subSystem.optionsHandler->SetOptionUnsignedInt("Window", "width", 1920);
					change = true;
				}
			}
			if (subSystem.window->ButtonPressed(MoveDir::SIZE12))
			{
				size_t height = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 1080);
				size_t width = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 1920);
				if (height != 720 || width != 1280)
				{
					subSystem.optionsHandler->SetOptionUnsignedInt("Window", "height", 720);
					subSystem.optionsHandler->SetOptionUnsignedInt("Window", "width", 1280);
					change = true;
				}
			}
			if (change == true)
			{
				subSystem.optionsHandler->Trigger();
			}
			newProjectiles.clear();

			Gameplay::PlayerUnit::MovementInput input(false, false, false, false, false, 0.0f, 0.0f);
			float movX = 0.0f;
			float movY = 0.0f;

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
				input.mouseRightDown = true;
			}
			if (subSystem.window->ButtonPressed(MoveDir::CONSOLE))
			{
				//e.GetDevConsole().Toggle();
			}

			int mX = 0;
			int mY = 0;
			subSystem.window->GetMousePos(mX, mY);

			DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
			DirectX::XMVECTOR rayD;
			managers.cameraManager->WorldSpaceRayFromScreenPos(mX, mY, subSystem.window->Width(), subSystem.window->Height(), rayO, rayD);


			float distance = XMVectorGetY(rayO) / -XMVectorGetY(rayD);

			auto clickPos = rayO + rayD*distance;

			input.mousePosX = DirectX::XMVectorGetX(clickPos);
			input.mousePosY = DirectX::XMVectorGetZ(clickPos);

			float totMov = abs(movX) + abs(movY);
			if (totMov != 0.f)
			{
				movX /= totMov;
				movY /= totMov;
			}

			Gameplay::PlayerUnit::ActionInput actionInput(false, false);
			if (subSystem.window->ButtonDown(MoveDir::SPACE))
			{
				actionInput.skill1Button = true;
			}
			/*Only thing needed right now*/
			blackBoard.deltaTime = dt;

			int arrowIndex = 0;
			for (int x = 0; x < 25; x++)
			{
				for (int y = 0; y < 25; y++)
				{
					if (testRoom->tileValues[x][y])
					{

					}
					else
					{

						float xMagnitude = 0.0f;
						float zMagnitude = 0.0f;
						pos enemyPos;
						enemyPos.x = x + 0.5f;
						enemyPos.y = y + 0.5f;
						testRoom->GetFlowFieldMap()->SampleFromMap(enemyPos, xMagnitude, zMagnitude);

						if (xMagnitude == 1.0f)
						{
							managers.transformManager->SetRotation(arrows[arrowIndex], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
						}
						else if (xMagnitude == -1.0f)
						{
							managers.transformManager->SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PIDIV2, 0.0f);
						}
						else if (zMagnitude == 1.0f)
						{
							managers.transformManager->SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PI, 0.0f);
						}
						else if (zMagnitude == -1.0f)
						{
							managers.transformManager->SetRotation(arrows[arrowIndex], 0.0f, 0.0f, 0.0f);

						}
						else if (xMagnitude == 0.707f)
						{
							if (zMagnitude == 0.707f)
							{
								managers.transformManager->SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
							}
							else if (zMagnitude == -0.707f)
							{
								managers.transformManager->SetRotation(arrows[arrowIndex], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
							}
						}
						else if (xMagnitude == -0.707f)
						{
							if (zMagnitude == 0.707f)
							{
								managers.transformManager->SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0f);
							}
							else if (zMagnitude == -0.707f)
							{
								managers.transformManager->SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PIDIV4, 0.0f);
							}
						}

						arrowIndex++;
					}

				}

			}

			player->UpdateMovement(dt * 5, input);
			player->UpdateActions(dt, newProjectiles, actionInput);

			projectileManager->AddProjectiles(newProjectiles);

			projectileManager->UpdateProjectilePositions(dt);
			testRoom->CheckProjectileCollision(projectileManager->GetAllProjectiles());
			projectileManager->UpdateProjectileActions(dt);


			playerPos.x = player->GetXPosition();
			playerPos.y = player->GetYPosition();

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


			/*	Utilz::TimeMap times;
			e.GetProfilingInformation(times);
			for (auto& t : times)
			console->Print("%s: %f\n", t.first.str, t.second);*/
		}

		delete projectileManager;
		delete testRoom;
		delete player;
		delete[] RoomArr;




		game.Shutdown();
		engine->Release();
		delete engine;


	};

	ThereBePizza:
	using namespace DirectX;
	auto engine = Core::CreateEngine();
	Core::IEngine::InitializationInfo info;

	info.subSystems.optionsHandler = Core::CreateOptionsHandler();
	info.subSystems.optionsHandler->Initialize("Config.ini");

	float width = info.subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 800);
	float height = info.subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 600);

	Window::InitializationInfo winInfo;
	winInfo.winState = Window::WindowState::Regular;
	winInfo.height = height;
	winInfo.width = width;
	winInfo.windowTitle = "Playback";
	winInfo.fullScreen = false;

	info.subSystems.window = Window::CreateNewWindow();
	info.subSystems.window->Initialize(winInfo);
	engine->Init(info);
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();


	ImGui::SetCurrentContext(static_cast<ImGuiContext*>(subSystem.devConsole->GetContext()));

	enum MoveDir
	{
		ESCAPE,
	};

	subSystem.window->MapActionButton(ESCAPE, Window::KeyEscape);

	std::vector<Utilz::File> files;
	Utilz::get_all_files_names_within_folder("Recordings", files);
	std::vector<const char*> fileNames;

	for (auto& fi : files)
	{
		fileNames.push_back(fi.name.c_str());
	}
	bool play = false;
	bool record = false;
	bool running = true;
	int current = 0;
	while (running)
	{
		if (subSystem.window->ButtonPressed(ESCAPE))
			running = false;
		engine->BeginFrame();
		if (ImGui::Button("Delete") && files.size() > 0)
		{
			std::remove(files[current].fullPath.c_str());
			for (int ite = current; ite < files.size() - 1; ite++)
			{
				files[ite] = files[ite + 1];
			}
			files.pop_back();
			fileNames.clear();
			for (auto& fi : files)
			{
				fileNames.push_back(fi.name.c_str());
			}
			if (current = files.size() && current != 0)
			{
				current--;
			}
		}
		ImGui::ListBox("PlaybackFiles", &current, fileNames.data(), fileNames.size());
		ImGui::LabelText("NrOfFiles", "%d",fileNames.size());
		if (ImGui::Button("Play") && files.size() > 0)
		{
			play = true;
			running = false;
		}
		if (ImGui::Button("Record"))
		{
			record = true;
			running = false;
		}		
		engine->EndFrame();
	}
	engine->Release();
	delete engine;
	if (play == true)
	{
		lam(files[current].fullPath, Window::WindowState::Playback);
		goto ThereBePizza;
	}
	if (record == true)
	{
		lam("", Window::WindowState::Record);
		goto ThereBePizza;
	}
	

	ProfileReturnConst(true)
}