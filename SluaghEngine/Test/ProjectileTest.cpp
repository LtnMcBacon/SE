#include "ProjectileTest.h"
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



SE::Test::ProjectileTest::ProjectileTest()
{

}

SE::Test::ProjectileTest::~ProjectileTest()
{

}

bool SE::Test::ProjectileTest::Run(SE::Utilz::IConsoleBackend* console)
{
	StartProfile;
	using namespace DirectX;
	auto& e = Core::Engine::GetInstance();
	auto& info = Core::Engine::InitializationInfo();
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

	Tools::Tools t;

	float width = om.GetOptionUnsignedInt("Window", "width", 800);
	float height = om.GetOptionUnsignedInt("Window", "height", 600);

	auto floor = em.Create();
	const int numberOfBlocks = 25 * 25;
	SE::Core::Entity entities[numberOfBlocks];
	SE::Core::Entity arrows[numberOfBlocks];

	for (int i = 0; i < numberOfBlocks; i++)
	{
		entities[i] = em.Create();
		arrows[i] = em.Create();
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

	tm.SetScale(player->GetEntity(), 1.f);
	rm.CreateRenderableObject(player->GetEntity(), Utilz::GUID("MCModell.mesh"));

	rm.ToggleRenderableObject(player->GetEntity(), true);
	tm.SetRotation(player->GetEntity(), 0, 0, 0);

	Core::Entity rotatingBox1 = em.Create();
	Core::Entity rotatingBox2 = em.Create();
	Core::Entity rotatingBox3 = em.Create();
	tm.Create(rotatingBox1, { 0.5f, 3.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.2f,0.2f,0.2f });
	tm.Create(rotatingBox2, { -0.5f, 3.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.2f,0.2f,0.2f });
	tm.Create(rotatingBox3, { 0.25f, 0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.1f,0.1f,0.1f });
	rm.CreateRenderableObject(rotatingBox1, Block);
	rm.CreateRenderableObject(rotatingBox2, Block);
	rm.CreateRenderableObject(rotatingBox3, Block);
	tm.BindChild(player->GetEntity(), rotatingBox1,true,true);
	tm.BindChild(player->GetEntity(), rotatingBox2, true,true);
	tm.BindChild(rotatingBox2, rotatingBox3, true, true);
	rm.ToggleRenderableObject(rotatingBox1, true);
	rm.ToggleRenderableObject(rotatingBox2, true);
	rm.ToggleRenderableObject(rotatingBox3, true);

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
	SE::Core::Engine::GetInstance().GetTransformManager().BindChild(player->GetEntity(), camera, false, true);
	SE::Core::Engine::GetInstance().GetTransformManager().Move(camera, -5 * cameraTranslation);
	SE::Core::Engine::GetInstance().GetTransformManager().SetRotation(camera, cameraRotationX, cameraRotationY, 0);

	SE::Gameplay::ProjectileManager* projectileManager = new SE::Gameplay::ProjectileManager();

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

		//new Gameplay::EnemyUnit(testRoom->GetFlowFieldMap(), enemyPos.x + .5f, enemyPos.y + .5f, 10.0f);
		rm.CreateRenderableObject(enemy->GetEntity(), Block);
		rm.ToggleRenderableObject(enemy->GetEntity(), true);
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
	//unsigned char counter = 0;
	float dt = 1.0f / 60.0f;
	while (running)
	{
		newProjectiles.clear();

		Gameplay::PlayerUnit::MovementInput input(false, false, false, false, false, 0.0f, 0.0f);
		float movX = 0.0f;
		float movY = 0.0f;

		if (e.GetWindow()->ButtonDown(MoveDir::UP))
		{
			input.upButton = true;
			movY += 1.0f;
		}
		if (e.GetWindow()->ButtonDown(MoveDir::DOWN))
		{
			input.downButton = true;
			movY -= 1.0f;
		}
		if (e.GetWindow()->ButtonDown(MoveDir::RIGHT))
		{
			input.rightButton = true;
			movX += 1.0f;
		}
		if (e.GetWindow()->ButtonDown(MoveDir::LEFT))
		{
			input.leftButton = true;
			movX -= 1.0f;
		}
		if (e.GetWindow()->ButtonDown(MoveDir::RIGHT_MOUSE))
		{
			input.mouseRightDown = true;
		}
		if(e.GetWindow()->ButtonPressed(MoveDir::CONSOLE))
		{
			e.GetDevConsole().Toggle();
		}

		int mX = 0;
		int mY = 0;
		e.GetWindow()->GetMousePos(mX, mY);

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
		if (e.GetWindow()->ButtonDown(MoveDir::SPACE))
		{
			actionInput.skill1Button = true;
		}

		tm.Rotate(rotatingBox2, 0.0f, dt * 6.28, 0.0f);
		tm.Rotate(rotatingBox3, 0.0f, dt*3.14, 0.0f);
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

		if (e.GetWindow()->ButtonPressed(0))
			running = false;
		if (e.GetWindow()->ButtonPressed(1))
		{
			stepping = !stepping;
		}

		if (stepping)
		{
			if (e.GetWindow()->ButtonDown(2))
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

