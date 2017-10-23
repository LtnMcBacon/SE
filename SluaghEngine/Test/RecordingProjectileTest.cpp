#include "RecordingProjectileTest.h"
#include <Core\Engine.h>
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

bool SE::Test::RecordingProjectileTest::Run(SE::Utilz::IConsoleBackend* console)
{
	StartProfile;
	

	using namespace DirectX;
	auto& e = Core::Engine::GetInstance();
	auto& info = Core::Engine::InitializationInfo();
	info.winInfo.winState = Window::WindowState::Record;
	info.winInfo.height = 720;
	info.winInfo.width = 1280;
	info.winInfo.windowTitle = "Recording";
	info.winInfo.fullScreen = false;
	auto re = e.Init(info);
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		ProfileReturnConst(false)
	}

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();
	auto& om = e.GetOptionHandler();
	auto& caM = e.GetCameraManager();
	auto& coM = e.GetCollisionManager();
	auto& mm = e.GetMaterialManager();
	auto& lm = e.GetLightManager();
	auto& am = e.GetAudioManager();
	auto win = e.GetWindow();


	Tools::Tools t;

	float width = om.GetOptionUnsignedInt("Window", "width", 800);
	float height = om.GetOptionUnsignedInt("Window", "height", 600);


	Core::MaterialManager::CreateInfo floorInfo;
	Utilz::GUID material = Utilz::GUID("Placeholder_Floor.mat");
	Utilz::GUID shader = Utilz::GUID("SimpleLightPS.hlsl");
	floorInfo.shader = shader;
	floorInfo.materialFile = material;
	auto floor = em.Create();
	mm.Create(floor, floorInfo);
	const int numberOfBlocks = 25 * 25;
	SE::Core::Entity entities[numberOfBlocks];
	SE::Core::Entity arrows[numberOfBlocks];

	Core::MaterialManager::CreateInfo cubeInfo;
	material = Utilz::GUID("Placeholder_Block.mat");
	shader = Utilz::GUID("SimpleLightPS.hlsl");
	cubeInfo.shader = shader;
	cubeInfo.materialFile = material;

	Core::MaterialManager::CreateInfo arrowInfo;
	material = Utilz::GUID("Placeholder_Arrow.mat");
	shader = Utilz::GUID("SimpleLightPS.hlsl");
	arrowInfo.shader = shader;
	arrowInfo.materialFile = material;

	for (int i = 0; i < numberOfBlocks; i++)
	{
		entities[i] = em.Create();
		mm.Create(entities[i], cubeInfo);
		arrows[i] = em.Create();
		mm.Create(arrows[i], arrowInfo);
	}
	tm.Create(floor);
	tm.SetPosition(floor, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));

	for (int i = 0; i < numberOfBlocks; i++)
	{
		tm.Create(entities[i]);
		tm.Create(arrows[i]);
	}


	rm.CreateRenderableObject(floor, Utilz::GUID("Placeholder_Floor.mesh"));
	rm.ToggleRenderableObject(floor, true);

	coM.CreateBoundingHierarchy(floor, Utilz::GUID("Placeholder_Floor.mesh"));


	auto Block = Utilz::GUID("Placeholder_Block.mesh");
	auto Arrow = Utilz::GUID("Placeholder_Arrow.mesh");


#pragma region AudioData
	am.LoadSound(Utilz::GUID("BLoop.wav"));

	int delay = 0;
	while (am.CheckIfLoaded(Utilz::GUID("BLoop.wav")) == 0 && delay < 10)
	{
		delay++;
	}

	auto soundEnt = em.Create();
	int soundID;
	soundID = am.CreateStream(soundEnt, Utilz::GUID("BLoop.wav"), Audio::SoundIndexName::BakgroundSound);
	if (soundID > -1)
		am.StreamSound(soundEnt, soundID);
#pragma endregion AudioData


	



#pragma region LightDataSet
	Core::Entity light[5];
	for (int i = 0; i < 5; i++)
	{
		light[i] = em.Create();
	}

	Core::AddLightInfo data;
	//Light 1
	data.color = DirectX::XMFLOAT3(0.8, 0.1, 0.1);
	data.pos = DirectX::XMFLOAT3(22.5, 1.0, 22.5);
	data.radius = 10.0f;
	lm.AddLight(light[0], data);
	lm.ToggleLight(light[0], true);

	//Light 2
	data.color = DirectX::XMFLOAT3(0.1, 0.8, 0.1);
	data.pos = DirectX::XMFLOAT3(2.5, 1.0, 22.5);
	data.radius = 10.0;
	lm.AddLight(light[1], data);
	lm.ToggleLight(light[1], true);

	//Light 3
	data.color = DirectX::XMFLOAT3(0.1, 0.1, 0.8);
	data.pos = DirectX::XMFLOAT3(22.5, 1.0, 2.5);
	data.radius = 10.0;
	lm.AddLight(light[2], data);
	lm.ToggleLight(light[2], true);

	//Light 4
	data.color = DirectX::XMFLOAT3(0.8, 0.1, 0.1);
	data.pos = DirectX::XMFLOAT3(2.5, 1.0, 2.5);
	data.radius = 10.0;
	lm.AddLight(light[3], data);
	lm.ToggleLight(light[3], true);

	//Light 5
	data.color = DirectX::XMFLOAT3(0.2, 0.2, 0.2);
	data.pos = DirectX::XMFLOAT3(80.0, 40.0, 0.0);
	data.radius = 150.0;
	lm.AddLight(light[4], data);
	lm.ToggleLight(light[4], true);
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
	tm.SetPosition(player->GetEntity(), DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));

	Core::MaterialManager::CreateInfo playerInfo;
	material = Utilz::GUID("MCModell.mat");
	shader = Utilz::GUID("SimpleLightPS.hlsl");
	playerInfo.shader = shader;
	playerInfo.materialFile = material;

	mm.Create(player->GetEntity(), playerInfo);
	tm.SetScale(player->GetEntity(), 1.f);
	rm.CreateRenderableObject(player->GetEntity(), Utilz::GUID("MCModell.mesh"));

	rm.ToggleRenderableObject(player->GetEntity(), true);
	tm.SetRotation(player->GetEntity(), 0, 0, 0);

	//SE::Core::Entity camera = caM.GetActive();
	SE::Core::Entity camera = SE::Core::Engine::GetInstance().GetEntityManager().Create();
	Core::CameraBindInfoStruct cInfo;
	cInfo.aspectRatio = (float)om.GetOptionUnsignedInt("Window", "width", 800) / (float)om.GetOptionUnsignedInt("Window", "height", 640);

	SE::Core::Engine::GetInstance().GetCameraManager().Bind(camera, cInfo);
	SE::Core::Engine::GetInstance().GetCameraManager().SetActive(camera);

	float cameraRotationX = DirectX::XM_PI / 3;
	float cameraRotationY = DirectX::XM_PI / 3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	player->UpdatePlayerRotation(cameraRotationX, cameraRotationY);
	SE::Core::Engine::GetInstance().GetTransformManager().BindChild(player->GetEntity(), camera, false);
	SE::Core::Engine::GetInstance().GetTransformManager().Move(camera, -5 * cameraTranslation);
	SE::Core::Engine::GetInstance().GetTransformManager().SetRotation(camera, cameraRotationX, cameraRotationY, 0);

	Gameplay::BehaviourPointers temp;
	temp.currentRoom = &testRoom;
	temp.player = player;

	SE::Gameplay::ProjectileManager* projectileManager = new SE::Gameplay::ProjectileManager(temp);

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
				rm.CreateRenderableObject(arrows[numberOfArrows], Arrow);
				rm.ToggleRenderableObject(arrows[numberOfArrows], true);
				float xMagnitude = 0.0f;
				float zMagnitude = 0.0f;
				pos enemyPos;
				enemyPos.x = x + 0.5f;
				enemyPos.y = y + 0.5f;
				testRoom->GetFlowFieldMap()->SampleFromMap(enemyPos, xMagnitude, zMagnitude);
				tm.SetPosition(arrows[numberOfArrows], DirectX::XMFLOAT3(x + 0.5f, 0.5f, y + 0.5f));
				tm.SetScale(arrows[numberOfArrows], 0.25f);
				if (xMagnitude == 1.0f)
				{
					tm.SetRotation(arrows[numberOfArrows], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
				}
				else if (xMagnitude == -1.0f)
				{
					tm.SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PIDIV2, 0.0f);
				}
				else if (zMagnitude == 1.0f)
				{
					tm.SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PI, 0.0f);
				}
				else if (zMagnitude == -1.0f)
				{
					tm.SetRotation(arrows[numberOfArrows], 0.0f, 0.0f, 0.0f);

				}
				else if (xMagnitude == 0.707f)
				{
					if (zMagnitude == 0.707f)
					{
						tm.SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
					}
					else if (zMagnitude == -0.707f)
					{
						tm.SetRotation(arrows[numberOfArrows], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
					}
				}
				else if (xMagnitude == -0.707f)
				{
					if (zMagnitude == 0.707f)
					{
						tm.SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0f);
					}
					else if (zMagnitude == -0.707f)
					{
						tm.SetRotation(arrows[numberOfArrows], 0.0f, DirectX::XM_PIDIV4, 0.0f);
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

		tm.SetRotation(enemy->GetEntity(), -DirectX::XM_PIDIV2, 0, 0);
		tm.SetScale(enemy->GetEntity(), 0.5f);
		testRoom->AddEnemyToRoom(enemy);
	}

	e.GetWindow()->MapActionButton(0, Window::KeyEscape);
	e.GetWindow()->MapActionButton(1, Window::Key1);
	e.GetWindow()->MapActionButton(2, Window::Key2);

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


	e.GetWindow()->MapActionButton(UP, Window::KeyW);
	e.GetWindow()->MapActionButton(RIGHT, Window::KeyD);
	e.GetWindow()->MapActionButton(DOWN, Window::KeyS);
	e.GetWindow()->MapActionButton(LEFT, Window::KeyA);
	e.GetWindow()->MapActionButton(RIGHT_MOUSE, Window::MouseRight);
	e.GetWindow()->MapActionButton(SPACE, Window::KeySpace);
	e.GetWindow()->MapActionButton(CONSOLE, Window::Key1);

	pos playerPos;
	playerPos.x = 1.5f;
	playerPos.y = 1.5f;

	std::vector<Gameplay::ProjectileData> newProjectiles;

	DirectX::XMFLOAT3 tPos = tm.GetPosition(floor);
	DirectX::XMFLOAT3 tRot = tm.GetRotation(floor);
	DirectX::XMFLOAT3 tScale = tm.GetScale(floor);
	DirectX::XMMATRIX worldM = { tScale.x, 0, 0, 0,
		0, tScale.y, 0, 0,
		0, 0, tScale.z, 0,
		tPos.x, tPos.y, tPos.z, 1.0f };

	bool stepping = false;
	bool running = true;
	win->UpdateTime();

	while (running)
	{
		win->UpdateTime();
		float dt = win->GetDelta();

		newProjectiles.clear();

		Gameplay::PlayerUnit::MovementInput input(false, false, false, false, false, 0.0f, 0.0f);
		float movX = 0.0f;
		float movY = 0.0f;

		if (win->ButtonDown(MoveDir::UP))
		{
			input.upButton = true;
			movY += 1.0f;
		}
		if (win->ButtonDown(MoveDir::DOWN))
		{
			input.downButton = true;
			movY -= 1.0f;
		}
		if (win->ButtonDown(MoveDir::RIGHT))
		{
			input.rightButton = true;
			movX += 1.0f;
		}
		if (win->ButtonDown(MoveDir::LEFT))
		{
			input.leftButton = true;
			movX -= 1.0f;
		}
		if (win->ButtonDown(MoveDir::RIGHT_MOUSE))
		{
			input.mouseRightDown = true;
		}
		if (win->ButtonPressed(MoveDir::CONSOLE))
		{
			e.GetDevConsole().Toggle();
		}

		int mX = 0;
		int mY = 0;
		win->GetMousePos(mX, mY);

		DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR rayD = t.rayToView(mX, mY, width, height);
		DirectX::XMFLOAT4X4 tempView = caM.GetViewInv(camera);
		DirectX::XMMATRIX viewM = DirectX::XMLoadFloat4x4(&tempView);

		rayO = DirectX::XMVector4Transform(rayO, viewM);
		rayD = DirectX::XMVector4Transform(rayD, viewM);
		rayD = XMVector3Normalize(rayD);

		//float distance = 0.0f;
		float distance = XMVectorGetY(rayO) / -XMVectorGetY(rayD);
		//bool pickTest = coM.PickEntity(floor, rayO, rayD, &distance);

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
		if (win->ButtonDown(MoveDir::SPACE))
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
						tm.SetRotation(arrows[arrowIndex], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
					}
					else if (xMagnitude == -1.0f)
					{
						tm.SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PIDIV2, 0.0f);
					}
					else if (zMagnitude == 1.0f)
					{
						tm.SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PI, 0.0f);
					}
					else if (zMagnitude == -1.0f)
					{
						tm.SetRotation(arrows[arrowIndex], 0.0f, 0.0f, 0.0f);

					}
					else if (xMagnitude == 0.707f)
					{
						if (zMagnitude == 0.707f)
						{
							tm.SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
						}
						else if (zMagnitude == -0.707f)
						{
							tm.SetRotation(arrows[arrowIndex], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
						}
					}
					else if (xMagnitude == -0.707f)
					{
						if (zMagnitude == 0.707f)
						{
							tm.SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0f);
						}
						else if (zMagnitude == -0.707f)
						{
							tm.SetRotation(arrows[arrowIndex], 0.0f, DirectX::XM_PIDIV4, 0.0f);
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

		if (win->ButtonPressed(0))
			running = false;
		if (win->ButtonPressed(1))
		{
			stepping = !stepping;
		}

		if (stepping)
		{
			if (win->ButtonDown(2))
			{
				testRoom->Update(dt, playerPos.x, playerPos.y);
			}
		}
		else {
			testRoom->Update(dt, playerPos.x, playerPos.y);
		}
		e.Frame(dt);


		/*	Utilz::TimeMap times;
		e.GetProfilingInformation(times);
		for (auto& t : times)
		console->Print("%s: %f\n", t.first.str, t.second);*/
	}

	delete projectileManager;
	delete testRoom;
	delete player;




	e.Release();

	ProfileReturnConst(true)
}