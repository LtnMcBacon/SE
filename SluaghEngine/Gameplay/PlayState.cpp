#include "PlayState.h"
#include <Profiler.h>
#include <Utilz\Tools.h>
#include "CoreInit.h"
#include "EnemyFactory.h"
#include <GameBlackboard.h>

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

using namespace SE;
using namespace Gameplay;
using namespace DirectX;
PlayState::PlayState()
{

}

PlayState::PlayState(Window::IWindow* Input, SE::Core::IEngine* engine, void* passedInfo)
{
	this->input = Input;
	this->engine = engine;

	InitializeRooms();
	InitializePlayer(passedInfo);
	InitializeEnemies();
	InitializeOther();

	BehaviourPointers temp;
	temp.currentRoom = &currentRoom;
	temp.player = player;
	
	projectileManager = new ProjectileManager(temp);
}

PlayState::~PlayState()
{
	delete projectileManager;
	delete player;
	//delete currentRoom;
	for (auto room : rooms)
		delete room;
}

void PlayState::UpdateInput(PlayerUnit::MovementInput &movement, PlayerUnit::ActionInput &action)
{
	if (input->ButtonDown(uint32_t(GameInput::UP)))
	{
		movement.upButton = true;
	}
	if (input->ButtonDown(uint32_t(GameInput::DOWN)))
	{
		movement.downButton = true;
	}
	if (input->ButtonDown(uint32_t(GameInput::LEFT)))
	{
		movement.leftButton = true;
	}
	if (input->ButtonDown(uint32_t(GameInput::RIGHT)))
	{
		movement.rightButton = true;
	}

	int mX, mY;
	input->GetMousePos(mX, mY);

	DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR rayD;


	auto width = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "width", 800);
	auto height = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "height", 640);
	CoreInit::managers.cameraManager->WorldSpaceRayFromScreenPos(mX, mY, width, height, rayO, rayD);

	float distance = DirectX::XMVectorGetY(rayO) / -XMVectorGetY(rayD);

	auto clickPos = rayO + rayD*distance;

	movement.mousePosX = DirectX::XMVectorGetX(clickPos);
	movement.mousePosY = DirectX::XMVectorGetZ(clickPos);


	if (input->ButtonPressed(uint32_t(GameInput::SKILL1)))
	{
		action.skill1Button = true;
	}

	if (input->ButtonPressed(uint32_t(GameInput::SKILL2)))
	{
		action.skill2Button = true;
	}

	if (input->ButtonDown(uint32_t(GameInput::ACTION)))
	{
		action.actionButton = true;
	}

}

void SE::Gameplay::PlayState::UpdateProjectiles(std::vector<ProjectileData>& newProjectiles)
{
	projectileManager->AddProjectiles(newProjectiles);

	projectileManager->UpdateProjectilePositions(input->GetDelta());
	currentRoom->CheckProjectileCollision(projectileManager->GetAllProjectiles());
	projectileManager->UpdateProjectileActions(input->GetDelta());
}

void PlayState::InitializeRooms()
{
	uint32_t nrOfRooms = 0;
	Utilz::GUID* RoomArr;
	auto subSystem = engine->GetSubsystems();
	int sideLength = 3;
	int nrOfRoomsToCreate = sideLength * sideLength;
	int nrOfRoomsCreated = 0;
	

	subSystem.resourceHandler->LoadResource("RoomGeneration.txt", [&nrOfRooms, &RoomArr](auto GUID, auto data, auto size)
	{
		nrOfRooms = *(uint32_t *)data;
		RoomArr = new Utilz::GUID[nrOfRooms];
		memcpy(RoomArr, (char*)data + sizeof(uint32_t), sizeof(Utilz::GUID) * nrOfRooms);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});

	while (nrOfRoomsCreated < nrOfRoomsToCreate)
	{
		//Skips nrOfOpenDoors for now since I don't know how many doors a room has got

		int random = CoreInit::subSystems.window->GetRand() % nrOfRooms;
		
		Gameplay::Room* temp = new Gameplay::Room(RoomArr[random]);

		rooms.push_back(temp);
		nrOfRoomsCreated++;
		temp->RenderRoom(false);
	}

	for (int i = 0; i < nrOfRoomsToCreate; i++)
	{
		if (i < sideLength) // top row
		{
			rooms[i]->CloseDoor(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH);
		}
		else
		{
			rooms[i]->AddAdjacentRoomByDirection(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH, rooms[i - sideLength]);
		}

		if (i % sideLength == sideLength - 1) // right side
		{
			rooms[i]->CloseDoor(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST);
		}
		else
		{
			rooms[i]->AddAdjacentRoomByDirection(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST, rooms[i + 1]);
		}

		if (i % sideLength == 0) // left side
		{
			rooms[i]->CloseDoor(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST);
		}
		else
		{
			rooms[i]->AddAdjacentRoomByDirection(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST, rooms[i - 1]);
		}

		if (i >= sideLength * (sideLength - 1)) // bottom row
		{
			rooms[i]->CloseDoor(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH);
		}
		else
		{
			rooms[i]->AddAdjacentRoomByDirection(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH, rooms[i + sideLength]);
		}



	}

	currentRoomIndex = 0;
	blackBoard.currentRoom = currentRoom = rooms[0];
	blackBoard.roomFlowField = currentRoom->GetFlowFieldMap();
	currentRoom->RenderRoom(true);
	delete[] RoomArr;

}
void SE::Gameplay::PlayState::InitializeEnemies()
{
	char map[25][25];

	EnemyCreationStruct eStruct;
	EnemyUnit** enemies = new EnemyUnit*[enemiesInEachRoom];
	for(auto room : rooms)
	{
		room->GetMap(map);
		eStruct.information.clear();

		for (int i = 0; i < enemiesInEachRoom; i++)
		{
			pos enemyPos;
			do
			{
				enemyPos.x = CoreInit::subSystems.window->GetRand() % 25;
				enemyPos.y = CoreInit::subSystems.window->GetRand() % 25;
			} while (map[int(enemyPos.x)][int(enemyPos.y)]);

			EnemyCreationData data;
			data.type = ENEMY_TYPE_GLAISTIG;
			data.startX = enemyPos.x;
			data.startY = enemyPos.y;
			data.useVariation = true;
			eStruct.information.push_back(data);
		}
		
		eFactory.CreateEnemies(eStruct, &blackBoard, enemies);

		for (int i = 0; i < enemiesInEachRoom; i++)
		{
			room->AddEnemyToRoom(enemies[i]);
		}

	}
	delete[] enemies;

}

void PlayState::InitializePlayer(void* playerInfo)
{
	char map[25][25];
	currentRoom->GetMap(map);
	PlayStateData* tempPtr = (PlayStateData*)playerInfo;

	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			if (map[x][y] == (char)22)
			{
				float rotation = ceilf((currentRoom->FloorCheck(x, y) * (180 / 3.1416) - 270) - 0.5f);
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
				player = new Gameplay::PlayerUnit(tempPtr->skills, nullptr, x + (0.5f + xOffset), y + (0.5f + yOffset), map);

				player->SetZPosition(0.9f);
				player->PositionEntity(x + (0.5f + xOffset), y + (0.5f + yOffset));
				break;
			}
		}
	}
}

void SE::Gameplay::PlayState::InitializeOther()
{
	//Setup camera to the correct perspective and bind it to the players position
	Core::ICameraManager::CreateInfo cInfo;
	cInfo.aspectRatio = (float)CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 800) / (float)CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 640);
	cam = CoreInit::managers.cameraManager->GetActive();
	CoreInit::managers.cameraManager->UpdateCamera(cam, cInfo);

	float cameraRotationX = DirectX::XM_PI / 3;
	float cameraRotationY = DirectX::XM_PI / 3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	player->UpdatePlayerRotation(cameraRotationX, cameraRotationY);
	CoreInit::managers.transformManager->BindChild(player->GetEntity(), cam, false, true);
	CoreInit::managers.transformManager->Move(cam, -5 * cameraTranslation);
	CoreInit::managers.transformManager->SetRotation(cam, cameraRotationX, cameraRotationY, 0);

	//Create a default light

	dummy = CoreInit::managers.entityManager->Create();
	CoreInit::managers.transformManager->Create(dummy, { 12.5, 3, 12.5 });
	CoreInit::managers.renderableManager->CreateRenderableObject(dummy, { "Placeholder_Block.mesh" });
	CoreInit::managers.renderableManager->ToggleRenderableObject(dummy, true);

	SE::Core::ILightManager::CreateInfo lightInfo;
	lightInfo.pos = { 0.0f, 0.0f, 0.0f };
	lightInfo.color = { 1.0f, 1.0f, 1.0f };
	lightInfo.radius = 10000.0f;

	CoreInit::managers.lightManager->Create(dummy, lightInfo);
	CoreInit::managers.lightManager->ToggleLight(dummy, true);
}

IGameState::State PlayState::Update(void*& passableInfo)
{
	StartProfile;
	IGameState::State returnValue = State::PLAY_STATE;
	PlayerUnit::MovementInput movementInput(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput actionInput(false, false);
	std::vector<ProjectileData> newProjectiles;

	UpdateInput(movementInput, actionInput);

	projectileManager->CheckCollisionBetweenUnitAndProjectiles(player, Gameplay::ValidTarget::PLAYER);
	player->Update(input->GetDelta(), movementInput, newProjectiles, actionInput);

	UpdateProjectiles(newProjectiles);

	blackBoard.playerPositionX = player->GetXPosition();
	blackBoard.playerPositionY = player->GetYPosition();
	blackBoard.deltaTime = input->GetDelta();
	blackBoard.playerHealth = player->GetHealth();
	
	currentRoom->Update(input->GetDelta(), player->GetXPosition(), player->GetYPosition());

	projectileManager->AddProjectiles(blackBoard.enemyProjectiles);
	blackBoard.enemyProjectiles.clear();

	//-----------------------------------------------

	if (actionInput.actionButton)
	{
		SE::Gameplay::Room::DirectionToAdjacentRoom dir = currentRoom->CheckForTransition(player->GetXPosition(), player->GetYPosition(), movementInput.mousePosX, movementInput.mousePosY);

		if (dir != SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NONE)
		{
			currentRoom->RenderRoom(false);

			if (dir == SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH)
			{
				currentRoom = rooms[currentRoomIndex + sqrt(rooms.size())];
				currentRoomIndex = currentRoomIndex + sqrt(rooms.size());
				float xToSet, yToSet;
				xToSet = yToSet = -999999;
				currentRoom->GetPositionOfActiveDoor(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH, xToSet, yToSet);
				player->PositionEntity(xToSet, yToSet - 1);
			}
			else if (dir == SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH)
			{
				currentRoom = rooms[currentRoomIndex - sqrt(rooms.size())];
				currentRoomIndex = currentRoomIndex - sqrt(rooms.size());
				float xToSet, yToSet;
				xToSet = yToSet = -999999;
				currentRoom->GetPositionOfActiveDoor(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH, xToSet, yToSet);
				player->PositionEntity(xToSet, yToSet + 1);
			}
			else if (dir == SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST)
			{
				currentRoom = rooms[currentRoomIndex - 1];
				currentRoomIndex = currentRoomIndex - 1;
				float xToSet, yToSet;
				xToSet = yToSet = -999999;
				currentRoom->GetPositionOfActiveDoor(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST, xToSet, yToSet);
				player->PositionEntity(xToSet + 1, yToSet);
			}
			else if (dir == SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST)
			{
				currentRoom = rooms[currentRoomIndex + 1];
				currentRoomIndex = currentRoomIndex + 1;
				float xToSet, yToSet;
				xToSet = yToSet = -999999;
				currentRoom->GetPositionOfActiveDoor(SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST, xToSet, yToSet);
				player->PositionEntity(xToSet - 1, yToSet);
			}

			currentRoom->RenderRoom(true);
			projectileManager->RemoveAllProjectiles();

			char newMap[25][25];
			currentRoom->GetMap(newMap);

			char** tempPtr = new char*[25];

			for (int i = 0; i < 25; i++)
			{
				tempPtr[i] = new char[25];
				for (int j = 0; j < 25; j++)
				{
					tempPtr[i][j] = newMap[i][j];
				}
			}

			player->UpdateMap(tempPtr);

			for (int i = 0; i < 25; i++)
			{
				delete tempPtr[i];
			}

			delete tempPtr;

			/**
			*	Must be put in change room once the function is done!
			*/
			blackBoard.currentRoom = currentRoom;
			blackBoard.roomFlowField = currentRoom->GetFlowFieldMap();
		}
	}

	//-----------------------------------------------

	if (!player->IsAlive())
		returnValue = State::GAME_OVER_STATE;

	ProfileReturn(returnValue);

}

