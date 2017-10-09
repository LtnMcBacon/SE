#include "PlayerMovementTest.h"
#include <Core\Engine.h>
#include <Gameplay/EnemyUnit.h>
#include <Gameplay/Room.h>
#include <chrono>
#include <Gameplay/PlayerUnit.h>
#include <Profiler.h>
#include <Utilz\Tools.h>

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



SE::Test::PlayerMovementTest::PlayerMovementTest()
{

}

SE::Test::PlayerMovementTest::~PlayerMovementTest()
{

}



bool SE::Test::PlayerMovementTest::Run(SE::Utilz::IConsoleBackend* console)
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
	auto& mm = e.GetMaterialManager();
	auto& lm = e.GetLightManager();

	Tools::Tools t;

	float width = om.GetOption("Window", "width", 800);
	float height = om.GetOption("Window", "height", 600);

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



	tm.Create(player->GetEntity());
	tm.SetPosition(player->GetEntity(), DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));
//	tm.SetRotation(player->GetEntity(), 0.0f, 3.14f, 0.0f);
	//tm.SetScale(player->GetEntity(), 1.f);

	Core::MaterialManager::CreateInfo minfo;
	Utilz::GUID textures[] = { Utilz::GUID("texture8.sei") };
	Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex") };
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	minfo.shader = shader;
	minfo.shaderResourceNames = resourceNames;
	minfo.textureFileNames = textures;
	minfo.textureCount = 2;

	mm.Create(player->GetEntity(), minfo, true);

	rm.CreateRenderableObject(player->GetEntity(), Utilz::GUID("MCModell.mesh"), true);
	rm.ToggleRenderableObject(player->GetEntity(), true);

	auto& l = em.Create();
	Core::AddLightInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 5.0f, -5.0f };
	d.color = { 1, 1,1 };
	lm.AddLight(l, d);
	lm.ToggleLight(l, true);

	
	SE::Core::Entity camera = SE::Core::Engine::GetInstance().GetEntityManager().Create();
	
	Core::CameraBindInfoStruct cInfo;
	cInfo.aspectRatio = (float)om.GetOption("Window", "width", 800) / (float)om.GetOption("Window", "height", 640);
	SE::Core::Engine::GetInstance().GetCameraManager().Bind(camera, cInfo);
	SE::Core::Engine::GetInstance().GetCameraManager().SetActive(camera);

	float cameraRotationX = DirectX::XM_PI/3;
	float cameraRotationY = DirectX::XM_PI/3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	player->UpdatePlayerRotation(cameraRotationX, cameraRotationY);
	SE::Core::Engine::GetInstance().GetTransformManager().BindChild(player->GetEntity(), camera, false);
	SE::Core::Engine::GetInstance().GetTransformManager().Move(camera, -5* cameraTranslation);
	SE::Core::Engine::GetInstance().GetTransformManager().SetRotation(camera, cameraRotationX, cameraRotationY, 0);//2 * DirectX::XM_PI / 3, 0);

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

	for (int i = 0; i < 100; i++)
	{
		pos enemyPos;
		do
		{
			enemyPos.x = rand() % 25;
			enemyPos.y = rand() % 25;
		} while (mapRepresentation[int(enemyPos.x)][int(enemyPos.y)]);

		Gameplay::EnemyUnit* enemy = new Gameplay::EnemyUnit(testRoom->GetFlowFieldMap(), enemyPos.x + .5f, enemyPos.y + .5f, 10.0f);
		rm.CreateRenderableObject(enemy->GetEntity(), Block);
		rm.ToggleRenderableObject(enemy->GetEntity(), true);
		tm.SetRotation(enemy->GetEntity(), -DirectX::XM_PIDIV2, 0, 0);
		tm.SetScale(enemy->GetEntity(), 0.5f);
		testRoom->AddEnemyToRoom(enemy);
	}

	Gameplay::EnemyUnit* enemy = new Gameplay::EnemyUnit(testRoom->GetFlowFieldMap(), 1 + .5f, 22 + .5f, 10.0f);
	rm.CreateRenderableObject(enemy->GetEntity(), Block);
	rm.ToggleRenderableObject(enemy->GetEntity(), true);
	tm.SetRotation(enemy->GetEntity(), -DirectX::XM_PIDIV2, 0, 0);
	tm.SetScale(enemy->GetEntity(), 0.5f);
	testRoom->AddEnemyToRoom(enemy);

	e.GetWindow()->MapActionButton(0, Window::KeyEscape);
	e.GetWindow()->MapActionButton(1, Window::Key1);
	e.GetWindow()->MapActionButton(2, Window::Key2);

	enum MoveDir
	{
		UP = 3,
		RIGHT = 4,
		DOWN = 5,
		LEFT = 6,
		RIGHT_MOUSE = 7
	};


	e.GetWindow()->MapActionButton(UP, Window::KeyUp);
	e.GetWindow()->MapActionButton(RIGHT, Window::KeyRight);
	e.GetWindow()->MapActionButton(DOWN, Window::KeyDown);
	e.GetWindow()->MapActionButton(LEFT, Window::KeyLeft);
	e.GetWindow()->MapActionButton(RIGHT_MOUSE, Window::MouseRight);

	pos playerPos;
	playerPos.x = 1.5f;
	playerPos.y = 1.5f;

	DirectX::XMFLOAT3 tPos = tm.GetPosition(floor);
	DirectX::XMFLOAT3 tRot = tm.GetRotation(floor);
	DirectX::XMFLOAT3 tScale = tm.GetScale(floor);
	DirectX::XMMATRIX worldM = { tScale.x, 0, 0, 0,
		0, tScale.y, 0, 0,
		0, 0, tScale.z, 0,
		tPos.x, tPos.y, tPos.z, 1.0f };

	bool stepping = false;
	bool running = true;
	unsigned char counter = 0;
	float dt = 1.0f / 60.0f;
	while (running)
	{
		SE::Core::Engine::GetInstance().GetTransformManager().Rotate(player->GetEntity(), 0.0f, 1.0f*dt, 0.0f);

		Gameplay::PlayerUnit::MovementInput input(false, false, false, false, false, 0.0f, 0.0f);
		float movX = 0.0f;
		float movY = 0.0f;

		if(e.GetWindow()->ButtonDown(MoveDir::UP))
		{
			input.downW = true;
			movY += 1.0f;
		}
		if(e.GetWindow()->ButtonDown(MoveDir::DOWN))
		{
			input.downS = true;
			movY -= 1.0f;
		}
		if (e.GetWindow()->ButtonDown(MoveDir::RIGHT))
		{
			input.downD = true;
			movX += 1.0f;
		}
		if (e.GetWindow()->ButtonDown(MoveDir::LEFT))
		{
			input.downA = true;
			movX -= 1.0f;
		}
		if (e.GetWindow()->ButtonDown(MoveDir::RIGHT_MOUSE))
		{
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

			float distance = XMVectorGetY(rayO)/-XMVectorGetY(rayD);
			//bool pickTest = coM.PickEntity(floor, rayO, rayD, &distance);

			auto clickPos = rayO + rayD*distance;

			input.mouseRightDown = true;
			input.mousePosX = DirectX::XMVectorGetX(clickPos);
			input.mousePosY = DirectX::XMVectorGetZ(clickPos);
		}
		float totMov = abs(movX) + abs(movY);
		if(totMov != 0.f)
		{
			movX /= totMov;
			movY /= totMov;
		}
		
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

		player->UpdateMovement(dt*5, input);

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
	}

	delete testRoom;
	delete player;
	e.Release();

	ProfileReturnConst(true)
}

