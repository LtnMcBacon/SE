#include "FlowFieldTest.h"
#include <Gameplay\Game.h>
#include <Gameplay/Flowfield.h>
#include <Profiler.h>

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


SE::Test::FlowFieldTest::FlowFieldTest()
{

}

SE::Test::FlowFieldTest::~FlowFieldTest()
{

}
enum
{
	Exit
};
bool SE::Test::FlowFieldTest::Run(SE::Utilz::IConsoleBackend* console)
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
	const int numberOfBlocks = 25*25;
	SE::Core::Entity entities[numberOfBlocks];

	for (int i = 0; i < numberOfBlocks; i++)
		entities[i] = managers.entityManager->Create();

	managers.transformManager->Create(player);
	managers.transformManager->SetPosition(player, DirectX::XMFLOAT3(1.f, 1.5f, 1.f));
	managers.transformManager->SetScale(player, 1.5f);
	managers.transformManager->Create(floor);
	managers.transformManager->SetPosition(floor, DirectX::XMFLOAT3(12.0f, 0.0f, 12.0f));
	
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
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	};
	int numberOfEntitesPlaced = 0;
	pos playerPos;
	playerPos.x = 1.5f;
	playerPos.y = 1.5f;

	SE::Gameplay::FlowField flowField(mapRepresentation, 1.0f, playerPos, 0.0f, 0.0f);
	for(int x = 0; x < 25; x++)
	{
		for(int y = 0; y < 25; y++, numberOfEntitesPlaced++)
		{
			if(mapRepresentation[x][y])
			{
				managers.renderableManager->CreateRenderableObject(entities[numberOfEntitesPlaced], { Block });
				managers.renderableManager->ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
				managers.transformManager->SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x, 0.5f, y));
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
				flowField.SampleFromMap(enemyPos, xMagnitude, zMagnitude);
				managers.transformManager->SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x, 0.5f, y));
				if(xMagnitude == 1.0f)
				{
					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
				}
				else if(xMagnitude == -1.0f)
				{
					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PIDIV2, 0.0f);
				}
				else if(zMagnitude == 1.0f)
				{
					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI , 0.0f);
				}
				else if(zMagnitude == -1.0f)
				{
					managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, 0.0f, 0.0f);
					
				}
				else if(xMagnitude == 0.707f)
				{
					if(zMagnitude == 0.707f)
					{
						managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
					}
					else if (zMagnitude == -0.707f)
					{
						managers.transformManager->SetRotation(entities[numberOfEntitesPlaced], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
					}
				}
				else if(xMagnitude == -0.707f)
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
			}
		}
	}




	subSystem.window->MapActionButton(0, Window::KeyEscape);
	subSystem.window->MapActionButton(Exit, Window::KeyEscape);

	bool running = true;
	unsigned char counter = 0;
	float dt = 1 / 60.0f;
	while (running)
	{
		if (subSystem.window->ButtonPressed(Exit))
			running = false;
		counter++;
		if(!counter)
		{
			do
			{
				playerPos.x = (rand() % 25) + 0.5f;
				playerPos.y = (rand() % 25) + 0.5f;
			} while (mapRepresentation[int(playerPos.x)][int(playerPos.y)]);
			managers.transformManager->SetPosition(player, DirectX::XMFLOAT3(playerPos.x - 0.5f, 1.5f, playerPos.y - 0.5f));
			flowField.Update(playerPos);
			numberOfEntitesPlaced = 0;
			for (int x = 0; x < 25; x++)
			{
				for (int y = 0; y < 25; y++, numberOfEntitesPlaced++)
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
						flowField.SampleFromMap(enemyPos, xMagnitude, zMagnitude);
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
					}
				}
			}
		}
		engine->Frame();
	}



	game.Shutdown();
	engine->Release(); delete engine;
	ProfileReturnConst(true)
}
