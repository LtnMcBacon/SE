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
	StartProfile;

	this->input = Input;
	this->engine = engine;
	playStateGUI.ParseFiles("PlayStateGui.HuD");
	playStateGUI.InitiateTextures();
	for (auto& button : playStateGUI.ButtonVector)
	{
		if (button.rectName == "HealthBar")
		{
			// here's the health bar.
			playStateGUI.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, NULL, button.textName, button.hoverTex, button.PressTex);
		}
	}
	playStateGUI.GUIButtons.CreateButton(50,650,50, 50,0,"tempButton",NULL);
	playStateGUI.GUIButtons.CreateButton(125, 650, 50, 50, 0, "tempButton", NULL);
	playStateGUI.GUIButtons.CreateButton(200, 650, 50, 50, 0, "tempButton", NULL);

	playStateGUI.GUIButtons.CreateButton(930, 650, 50, 50, 0, "tempButton", NULL);
	playStateGUI.GUIButtons.CreateButton(1030, 650, 50, 50, 0, "tempButton", NULL);
	playStateGUI.GUIButtons.CreateButton(1130, 650, 50, 50, 0, "tempButton", NULL);

	playStateGUI.GUIButtons.CreateButton(930, 575, 50, 50, 0, "tempButton", NULL);
	playStateGUI.GUIButtons.CreateButton(1030, 575, 50, 50, 0, "tempButton", NULL);
	playStateGUI.GUIButtons.CreateButton(1130, 575, 50, 50, 0, "tempButton", NULL);
	playStateGUI.GUIButtons.DrawButtons();

	InitializeRooms();
	InitializePlayer(passedInfo);
	InitializeEnemies();
	InitializeOther();

	/* Play sounds */
	currentSound = 0u;
	sounds[0] = Utilz::GUID("BLoop.wav");
	sounds[1] = Utilz::GUID("BLoop2.wav");
	sounds[2] = Utilz::GUID("BLoop3.wav");

	soundEnt = CoreInit::managers.entityManager->Create();
	for (int i = 0; i < 3; ++i)
	{
		CoreInit::managers.audioManager->Create(soundEnt, { sounds[i], SE::Audio::BakgroundLoopSound });
	}
	CoreInit::managers.audioManager->PlaySound(soundEnt, sounds[0]);

	BehaviourPointers temp;
	temp.currentRoom = &currentRoom;
	temp.player = player;
	
	projectileManager = new ProjectileManager(temp);


	InitWeaponPickups();
	ProfileReturnVoid;
}

PlayState::~PlayState()
{
	StartProfile;
	delete projectileManager;
	delete player;
	//delete currentRoom;
	for (auto room : rooms)
		delete room;
	ProfileReturnVoid;
}

void PlayState::UpdateInput(PlayerUnit::MovementInput &movement, PlayerUnit::ActionInput &action)
{
	StartProfile;
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
	ProfileReturnVoid;
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
	StartProfile;
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
	ProfileReturnVoid;
}
void SE::Gameplay::PlayState::InitializeEnemies()
{
	StartProfile;
	char map[25][25];

	EnemyCreationStruct eStruct;
	EnemyUnit** enemies = new EnemyUnit*[enemiesInEachRoom];
	for(auto& room : rooms)
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
			data.type = ENEMY_TYPE_RANDOM;
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
	ProfileReturnVoid;
}

void PlayState::InitializePlayer(void* playerInfo)
{
	StartProfile;
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
	ProfileReturnVoid;
}

void SE::Gameplay::PlayState::InitializeOther()
{
	StartProfile;
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
	ProfileReturnVoid;
}

void SE::Gameplay::PlayState::InitWeaponPickups()
{
	StartProfile;
	auto pe = player->GetEntity();

	Core::IEventManager::EventCallbacks pickUpEvent;
	pickUpEvent.triggerCallback = [this](const Core::Entity ent, void* data) {
		bool isWep = std::get<bool>(CoreInit::managers.dataManager->GetValue(ent, "Weapon", false));
		if (isWep)
		{
			CoreInit::subSystems.devConsole->Print("Picked up weapon %s.", std::get<std::string>(CoreInit::managers.dataManager->GetValue(ent, "Name", "Nan"s)).c_str());
		}
		CoreInit::managers.entityManager->DestroyNow(ent); // Just save the entity instead and use it as the picket up weapon as well.
	};

	Core::ITextManager::CreateInfo ciname;
	ciname.info.posX = -35;
	ciname.info.posY = -125;
	ciname.info.screenAnchor = { 0.5f, 0.5f };
	ciname.info.anchor = { 1,0.0f };
	ciname.info.scale = { 0.4f,0.4f };
	ciname.info.height = 50;
	auto weaponName = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponName, ciname);

	Core::ITextManager::CreateInfo citype;
	citype.info.posX = -35;
	citype.info.posY = -80;
	citype.info.screenAnchor = { 0.5f, 0.5f };
	citype.info.anchor = { 1,0.0f };
	citype.info.scale = { 0.4f,0.4f };
	citype.info.height = 50;
	auto weaponType = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponType, citype);

	Core::ITextManager::CreateInfo cielement;
	cielement.info.posX = -35;
	cielement.info.posY = -25;
	cielement.info.screenAnchor = { 0.5f, 0.5f };
	cielement.info.anchor = { 1,0.0f };
	cielement.info.scale = { 0.4f,0.4f };
	cielement.info.height = 50;
	cielement.info.colour = { 0.7f,0.7f,0.7f,1 };
	auto weaponElement = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponElement, cielement);


	Core::ITextManager::CreateInfo cidamage;
	cidamage.info.posX = -35;
	cidamage.info.posY = 40;
	cidamage.info.screenAnchor = { 0.5f, 0.5f };
	cidamage.info.anchor = { 1,0.0f };
	cidamage.info.scale = { 0.4f,0.4f };
	cidamage.info.height = 50;
	auto weaponDamage = CoreInit::managers.entityManager->Create();
	CoreInit::managers.textManager->Create(weaponDamage, cidamage);

	Core::IGUIManager::CreateInfo ciback;
	ciback.texture = "Crossbow_texture_metal.jpg";
	ciback.textureInfo.width = 200;
	ciback.textureInfo.height = 200;
	ciback.textureInfo.posX = -30;
	ciback.textureInfo.posY = -130;
	ciback.textureInfo.screenAnchor = { 0.5f, 0.5f };
	ciback.textureInfo.anchor = { 1.0, 0.0f };
	auto weaponBack = CoreInit::managers.entityManager->Create();
	CoreInit::managers.guiManager->Create(weaponBack, ciback);

	pickUpEvent.triggerCheck = [pe, weaponName, weaponType, weaponElement, weaponBack, weaponDamage](const Core::Entity ent, void* data) {
		if (CoreInit::managers.collisionManager->CheckCollision(ent, pe))
		{
			auto s = std::get<std::string>(CoreInit::managers.dataManager->GetValue(ent, "Name", "NaN"s));
			std::wstring ws;
			ws.assign(s.begin(), s.end());
			CoreInit::managers.textManager->SetText(weaponName, ws);
			CoreInit::managers.textManager->ToggleRenderableText(weaponName, true);

			auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Type", -1));
			CoreInit::managers.textManager->SetText(weaponType, (type == 0 ? L"Sword" : L"None"));
			CoreInit::managers.textManager->ToggleRenderableText(weaponType, true);

			auto element = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Element", -1));
			CoreInit::managers.textManager->SetText(weaponElement, (element == 0 ? L"Physical" : L"Fire"));
			CoreInit::managers.textManager->SetTextColour(weaponElement, (element == 0 ? DirectX::XMFLOAT4{ 0.7f, 0.7f, 0.7f, 1.0f } : DirectX::XMFLOAT4{ 0.8f, 0.3f, 0.2f, 1.0f }));
			CoreInit::managers.textManager->ToggleRenderableText(weaponElement, true);

			auto damage = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(ent, "Damage", -1));
			CoreInit::managers.textManager->SetText(weaponDamage, std::to_wstring(damage));
			CoreInit::managers.textManager->ToggleRenderableText(weaponDamage, true);

			CoreInit::managers.guiManager->ToggleRenderableTexture(weaponBack, true);

			return CoreInit::subSystems.window->ButtonDown(GameInput::INTERACT);
		}
		else
		{
			CoreInit::managers.textManager->ToggleRenderableText(weaponName, false);
			CoreInit::managers.textManager->ToggleRenderableText(weaponType, false);
			CoreInit::managers.textManager->ToggleRenderableText(weaponElement, false);
			CoreInit::managers.textManager->ToggleRenderableText(weaponDamage, false);
			CoreInit::managers.guiManager->ToggleRenderableTexture(weaponBack, false);
		}

		return false;
	};
	CoreInit::managers.eventManager->RegisterEventCallback("WeaponPickUp", pickUpEvent);
	ProfileReturnVoid;
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

	//-----sound update
	soundTime += input->GetDelta();
	if (soundTime > 60.0f)
	{
		uint8_t newSound = input->GetRand() % 3;
		if (currentSound != newSound)
		{
			CoreInit::managers.audioManager->StopSound(soundEnt, sounds[currentSound]);
			CoreInit::managers.audioManager->PlaySound(soundEnt, sounds[newSound]);
			currentSound = newSound;
		}
		soundTime = 0.0f;
	}
	//-----end sound update

	//-----------------------------------------------

	if (input->ButtonPressed(uint32_t(GameInput::INTERACT)))
	{
		SE::Gameplay::Room::DirectionToAdjacentRoom dir = currentRoom->CheckForTransition(player->GetXPosition(), player->GetYPosition());

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

