#include "RecordingProjectileTest.h"
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



SE::Test::RecordingProjectileTest::RecordingProjectileTest()
{

}

SE::Test::RecordingProjectileTest::~RecordingProjectileTest()
{

}

bool SE::Test::RecordingProjectileTest::Run(SE::DevConsole::IConsole* console)
{
	StartProfile;
	
	auto lam = [](Window::WindowState state)
	{


		using namespace DirectX;
		auto engine = Core::CreateEngine();


		Core::IEngine::InitializationInfo info;

		info.subSystems.optionsHandler = Core::CreateOptionsHandler();
		info.subSystems.optionsHandler->Initialize("Config.ini");

		float width = info.subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 800);
		float height = info.subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 600);

		Window::InitializationInfo winInfo;
		winInfo.winState = state;
		winInfo.height = height;
		winInfo.width = width;
		winInfo.windowTitle = "Recording";
		winInfo.fullScreen = false;

		info.subSystems.window = Window::CreateNewWindow();
		info.subSystems.window->Initialize(winInfo);
		Gameplay::Game game;
		engine->Init(info);
		game.Initiate(engine);
		auto managers = engine->GetManagers();
		auto subSystem = engine->GetSubsystems();







		Core::IMaterialManager::CreateInfo floorInfo;
		Utilz::GUID material = Utilz::GUID("Placeholder_Floor.mat");
		Utilz::GUID shader = Utilz::GUID("SimpleLightPS.hlsl");
		floorInfo.shader = shader;
		floorInfo.materialFile = material;
		auto floor = managers.entityManager->Create();
		managers.materialManager->Create(floor, floorInfo);
		const int numberOfBlocks = 25 * 25;
		SE::Core::Entity entities[numberOfBlocks];
		SE::Core::Entity arrows[numberOfBlocks];

		Core::IMaterialManager::CreateInfo cubeInfo;
		material = Utilz::GUID("Placeholder_Block.mat");
		shader = Utilz::GUID("SimpleLightPS.hlsl");
		cubeInfo.shader = shader;
		cubeInfo.materialFile = material;

		Core::IMaterialManager::CreateInfo arrowInfo;
		material = Utilz::GUID("Placeholder_Arrow.mat");
		shader = Utilz::GUID("SimpleLightPS.hlsl");
		arrowInfo.shader = shader;
		arrowInfo.materialFile = material;

		for (int i = 0; i < numberOfBlocks; i++)
		{
			entities[i] = managers.entityManager->Create();
			managers.materialManager->Create(entities[i], cubeInfo);
			arrows[i] = managers.entityManager->Create();
			managers.materialManager->Create(arrows[i], arrowInfo);
		}
		managers.transformManager->Create(floor);
		managers.transformManager->SetPosition(floor, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));

		for (int i = 0; i < numberOfBlocks; i++)
		{
			managers.transformManager->Create(entities[i]);
			managers.transformManager->Create(arrows[i]);
		}


		managers.renderableManager->CreateRenderableObject(floor, { "Placeholder_Floor.mesh" });
		managers.renderableManager->ToggleRenderableObject(floor, true);

		managers.collisionManager->CreateBoundingHierarchy(floor, Utilz::GUID("Placeholder_Floor.mesh"));


		auto Block = Utilz::GUID("Placeholder_Block.mesh");
		auto Arrow = Utilz::GUID("Placeholder_Arrow.mesh");


#pragma region AudioData
	auto soundEnt = managers.entityManager->Create();
	Core::IAudioManager::CreateInfo ausioInfo1;
	ausioInfo1.soundFile = Utilz::GUID("BLoop.wav");
	ausioInfo1.soundType = Audio::SoundIndexName::BakgroundSound;
	managers.audioManager->Create(soundEnt, ausioInfo1);
	/*Core::IAudioManager::CreateInfo ausioInfo2;
	ausioInfo2.soundFile = Utilz::GUID("BLoop2.wav");
	ausioInfo2.soundType = Audio::SoundIndexName::BakgroundSound;
	managers.audioManager->Create(soundEnt2, ausioInfo2);*/

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
		data.color = DirectX::XMFLOAT3(0.8, 0.1, 0.1);
		data.pos = DirectX::XMFLOAT3(22.5, 1.0, 22.5);
		data.radius = 10.0f;
		managers.lightManager->Create(light[0], data);
		managers.lightManager->ToggleLight(light[0], true);

		//Light 2
		data.color = DirectX::XMFLOAT3(0.1, 0.8, 0.1);
		data.pos = DirectX::XMFLOAT3(2.5, 1.0, 22.5);
		data.radius = 10.0;
		managers.lightManager->Create(light[1], data);
		managers.lightManager->ToggleLight(light[1], true);

		//Light 3
		data.color = DirectX::XMFLOAT3(0.1, 0.1, 0.8);
		data.pos = DirectX::XMFLOAT3(22.5, 1.0, 2.5);
		data.radius = 10.0;
		managers.lightManager->Create(light[2], data);
		managers.lightManager->ToggleLight(light[2], true);

		//Light 4
		data.color = DirectX::XMFLOAT3(0.8, 0.1, 0.1);
		data.pos = DirectX::XMFLOAT3(2.5, 1.0, 2.5);
		data.radius = 10.0;
		managers.lightManager->Create(light[3], data);
		managers.lightManager->ToggleLight(light[3], true);

		//Light 5
		data.color = DirectX::XMFLOAT3(0.8, 0.8, 0.8);
		data.pos = DirectX::XMFLOAT3(12.5, 40.0, 12.5);
		data.radius = 150.0;
		managers.lightManager->Create(light[4], data);
		managers.lightManager->ToggleLight(light[4], true);
#pragma endregion LightDataSet

		/*Place out the level*/
		char mapRepresentation[25][25] =
		{
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
		};
		int numberOfEntitesPlaced = 0;
		int numberOfArrows = 0;
		Gameplay::Room* testRoom = new Gameplay::Room(mapRepresentation);

		Gameplay::PlayerUnit* player = new Gameplay::PlayerUnit(nullptr, nullptr, 1.5f, 1.5f, mapRepresentation);
		managers.transformManager->SetPosition(player->GetEntity(), DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));

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

		//SE::Core::Entity camera = managers.cameraManager->GetActive();
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
		managers.transformManager->BindChild(player->GetEntity(), camera, false);
		managers.transformManager->Move(camera, -5 * cameraTranslation);
		managers.transformManager->SetRotation(camera, cameraRotationX, cameraRotationY, 0);

		SE::Gameplay::BehaviourPointers temp;
		temp.currentRoom = &testRoom;
		temp.player = player;

		SE::Gameplay::ProjectileManager* projectileManager = new SE::Gameplay::ProjectileManager(temp);

		for (int x = 0; x < 25; x++)
		{
			for (int y = 0; y < 25; y++)
			{
				if (mapRepresentation[x][y])
				{
					managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Block });
					managers.renderableManager->ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
					managers.transformManager->SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x + 0.5f, 0.5f, y + 0.5f));
					numberOfEntitesPlaced++;
				}
				else
				{
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
				}
			}
		}
		Gameplay::EnemyFactory eFactory;
		auto enemyGUID = Utilz::GUID("FlowFieldEnemy.SEC");
		eFactory.LoadEnemyIntoMemory(enemyGUID);
		Gameplay::GameBlackboard blackBoard;
		blackBoard.roomFlowField = testRoom->GetFlowFieldMap();

		for (int i = 0; i < 100; i++)
		{
			pos enemyPos;
			do
			{
				enemyPos.x = rand() % 25;
				enemyPos.y = rand() % 25;
			} while (mapRepresentation[int(enemyPos.x)][int(enemyPos.y)]);

			Gameplay::EnemyUnit* enemy = eFactory.CreateEnemy(enemyGUID, &blackBoard);
			enemy->SetXPosition(enemyPos.x + .5f);
			enemy->SetYPosition(enemyPos.y + .5f);

			managers.transformManager->SetRotation(enemy->GetEntity(), -DirectX::XM_PIDIV2, 0, 0);
			managers.transformManager->SetScale(enemy->GetEntity(), 0.5f);
			testRoom->AddEnemyToRoom(enemy);
		}

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
		SIZE12 = 12
	};


	subSystem.window->MapActionButton(UP, Window::KeyW);
	subSystem.window->MapActionButton(RIGHT, Window::KeyD);
	subSystem.window->MapActionButton(DOWN, Window::KeyS);
	subSystem.window->MapActionButton(LEFT, Window::KeyA);
	subSystem.window->MapActionButton(RIGHT_MOUSE, Window::MouseRight);
	subSystem.window->MapActionButton(SPACE, Window::KeySpace);
	subSystem.window->MapActionButton(CONSOLE, Window::Key1);
	subSystem.window->MapActionButton(FULLSCREEN, Window::KeyT);
	subSystem.window->MapActionButton(SIZE19, Window::KeyY);
	subSystem.window->MapActionButton(SIZE12, Window::KeyU);
	
		pos playerPos;
		playerPos.x = 1.5f;
		playerPos.y = 1.5f;

		std::vector<Gameplay::ProjectileData> newProjectiles;

		DirectX::XMFLOAT3 tPos = managers.transformManager->GetPosition(floor);
		DirectX::XMFLOAT3 tRot = managers.transformManager->GetRotation(floor);
		DirectX::XMFLOAT3 tScale = managers.transformManager->GetScale(floor);
		DirectX::XMMATRIX worldM = { tScale.x, 0, 0, 0,
			0, tScale.y, 0, 0,
			0, 0, tScale.z, 0,
			tPos.x, tPos.y, tPos.z, 1.0f };

	bool stepping = false;
	bool running = true;
	subSystem.window->UpdateTime();
	float audioTime = 0.0;
	int audio = 0;
	bool change = false;
	while (running)
	{
		float dt = subSystem.window->GetDelta();
		audioTime += dt;
		/*if (audioTime > 60.0)
		{
			managers.audioManager->StopSound(soundEnt, Utilz::GUID("BLoop.wav"));
			audio = (audio + 1) % 2;
			managers.audioManager->PlaySound(soundEnt, Utilz::GUID("BLoop2.wav"));
			audioTime = 0.0;
		}*/
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
			managers.cameraManager->WorldSpaceRayFromScreenPos(mX, mY, width, height, rayO, rayD);


			//float distance = 0.0f;
			float distance = XMVectorGetY(rayO) / -XMVectorGetY(rayD);
			//bool pickTest = managers.collisionManager->PickEntity(floor, rayO, rayD, &distance);

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
					if (mapRepresentation[x][y])
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




		game.Shutdown();
		engine->Release();
		delete engine;


	};


	lam(Window::WindowState::Record);
	lam(Window::WindowState::Playback);

	ProfileReturnConst(true)
}