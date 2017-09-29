#include "FlowFieldTest.h"
#include <Core\Engine.h>
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

bool SE::Test::FlowFieldTest::Run(SE::Utilz::IConsoleBackend* console)
{
	StartProfile;
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
	
	auto floor = em.Create();
	auto player = em.Create();
	const int numberOfBlocks = 25*25;
	SE::Core::Entity entities[numberOfBlocks];

	for (int i = 0; i < numberOfBlocks; i++)
		entities[i] = em.Create();

	tm.Create(player);
	tm.SetPosition(player, DirectX::XMFLOAT3(1.f, 1.5f, 1.f));
	tm.SetScale(player, 1.5f);
	tm.Create(floor);
	tm.SetPosition(floor, DirectX::XMFLOAT3(12.0f, 0.0f, 12.0f));
	
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
				rm.CreateRenderableObject(entities[numberOfEntitesPlaced], Block);
				rm.ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
				tm.SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x, 0.5f, y));
			}
			else
			{
				rm.CreateRenderableObject(entities[numberOfEntitesPlaced], Arrow );
				rm.ToggleRenderableObject(entities[numberOfEntitesPlaced], true);
				float xMagnitude = 0.0f;
				float zMagnitude = 0.0f;
				pos enemyPos;
				enemyPos.x = x + 0.5f;
				enemyPos.y = y + 0.5f;
				flowField.SampleFromMap(enemyPos, xMagnitude, zMagnitude);
				tm.SetPosition(entities[numberOfEntitesPlaced], DirectX::XMFLOAT3(x, 0.5f, y));
				if(xMagnitude == 1.0f)
				{
					tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
				}
				else if(xMagnitude == -1.0f)
				{
					tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PIDIV2, 0.0f);
				}
				else if(zMagnitude == 1.0f)
				{
					tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI , 0.0f);
				}
				else if(zMagnitude == -1.0f)
				{
					tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, 0.0f, 0.0f);
					
				}
				else if(xMagnitude == 0.707f)
				{
					if(zMagnitude == 0.707f)
					{
						tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
					}
					else if (zMagnitude == -0.707f)
					{
						tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
					}
				}
				else if(xMagnitude == -0.707f)
				{
					if (zMagnitude == 0.707f)
					{
						tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0f);
					}
					else if (zMagnitude == -0.707f)
					{
						tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PIDIV4, 0.0f);
					}
				}
			}
		}
	}




	e.GetWindow()->MapActionButton(0, Window::KeyEscape);

	bool running = true;
	unsigned char counter = 0;
	float dt = 1 / 60.0f;
	while (running)
	{
		if (e.GetWindow()->ButtonPressed(0))
			running = false;
		counter++;
		if(!counter)
		{
			do
			{
				playerPos.x = (rand() % 25) + 0.5f;
				playerPos.y = (rand() % 25) + 0.5f;
			} while (mapRepresentation[int(playerPos.x)][int(playerPos.y)]);
			tm.SetPosition(player, DirectX::XMFLOAT3(playerPos.x - 0.5f, 1.5f, playerPos.y - 0.5f));
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
							tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
						}
						else if (xMagnitude == -1.0f)
						{
							tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PIDIV2, 0.0f);
						}
						else if (zMagnitude == 1.0f)
						{
							tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI, 0.0f);
						}
						else if (zMagnitude == -1.0f)
						{
							tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, 0.0f, 0.0f);

						}
						else if (xMagnitude == 0.707f)
						{
							if (zMagnitude == 0.707f)
							{
								tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
							}
							else if (zMagnitude == -0.707f)
							{
								tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
							}
						}
						else if (xMagnitude == -0.707f)
						{
							if (zMagnitude == 0.707f)
							{
								tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0f);
							}
							else if (zMagnitude == -0.707f)
							{
								tm.SetRotation(entities[numberOfEntitesPlaced], 0.0f, DirectX::XM_PIDIV4, 0.0f);
							}
						}
					}
				}
			}
		}
		e.Frame(dt);
	}



	e.Release();
	ProfileReturnConst(true)
}
