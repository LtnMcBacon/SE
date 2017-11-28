#include "PlayState.h"
#include <Profiler.h>
#include <Utilz\Tools.h>
#include "CoreInit.h"
#include "EnemyFactory.h"
#include <GameBlackboard.h>
#include "SluaghRoom.h"
#include <string>
#include <Items.h>
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
static size_t dmgOverlayIndex = 0;
PlayState::PlayState(Window::IWindow* Input, SE::Core::IEngine* engine, void* passedInfo)
{
	StartProfile;
	CoreInit::subSystems.devConsole->AddCommand([this](DevConsole::IConsole* con, int argc, char** argv)
	{
		if (argc >= 3)
		{
			uint8_t slot = 0;
			if (argc == 4)
				slot = std::stoi(argv[3]);
			auto type = ItemType(std::stoi(argv[1]));
			switch (type)
			{
			case SE::Gameplay::ItemType::WEAPON:
			{
				auto wType = Item::Weapon::Type(std::stoi(argv[2]));
				
				
				player->AddItem(Item::Weapon::Create(wType), slot);
				break;
			}
				
			case SE::Gameplay::ItemType::CONSUMABLE:
			{
				auto cType = Item::Consumable::Type(std::stoi(argv[2]));
				player->AddItem(Item::Consumable::Create(cType), slot);
				break;
			}
			case SE::Gameplay::ItemType::NUM_TYPES:
				break;
			default:
				break;
			}
		}
		else
		{
			con->PrintChannel("give", "Usage:");
			con->PrintChannel("give", "give ItemType Type");
			con->PrintChannel("give", "ItemTypes:");
			con->PrintChannel("give", "\tWeapon - 0:");
			con->PrintChannel("give", "\tConsumable - 1");
			con->PrintChannel("give", "");
			con->PrintChannel("give", "WeaponTypes:");
			con->PrintChannel("give", "\tSword - 0");
			con->PrintChannel("give", "\tCrossbow - 1");
			con->PrintChannel("give", "\tWand - 2");
			con->PrintChannel("give", "ConsumableTypes:");
			con->PrintChannel("give", "\tHp - 0");
		}
	}, "give", "Give the player an item");


	this->input = Input;
	this->engine = engine;
	playStateGUI.ParseFiles("PlayStateGui.HuD");
	playStateGUI.InitiateTextures();
	int tempPos = 0;
	for (auto& button : playStateGUI.ButtonVector)
	{
		if (button.rectName == "HealthBar")
		{
			// here's the health bar.
			playStateGUI.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, NULL, button.textName, button.hoverTex, button.PressTex);
			healthBarPos = tempPos;
		}
		else if (button.rectName == "EnemyHpFrame")
		{

		}
		else if (button.rectName == "EnemyHp")
		{

		}

		tempPos++;
	}

	playStateGUI.GUIButtons.DrawButtons();

	InitializeRooms();
	InitializePlayer(passedInfo);
	InitializeOther();
	InitializeEnemies();

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
	CoreInit::managers.audioManager->SetCameraEnt(CoreInit::managers.cameraManager->GetActive());


	sluaghRoomX = worldWidth - 1;
	sluaghRoomY = worldHeight - 1;
	/*Initialize Sluagh*/
	delete rooms[sluaghRoomX * worldHeight + sluaghRoomY];
	rooms[sluaghRoomX * worldHeight  + sluaghRoomY] = new SluaghRoom("Room18.room", player, projectileManager);


	for (int x = 0; x < worldWidth; x++)
	{
		for (int y = 0; y < worldHeight; y++)
		{
			auto room = GetRoom(x, y).value();
			if (auto leftRoom = GetRoom(x - 1, y); leftRoom.has_value())
				room->AddAdjacentRoomByDirection(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST, (*leftRoom));
			if (auto rightRoom = GetRoom(x + 1, y); rightRoom.has_value())
				room->AddAdjacentRoomByDirection(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST, (*rightRoom));
			if (auto upRoom = GetRoom(x, y + 1); upRoom.has_value())
				room->AddAdjacentRoomByDirection(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH, (*upRoom));
			if (auto downRoom = GetRoom(x, y - 1); downRoom.has_value())
				room->AddAdjacentRoomByDirection(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH, (*downRoom));
		}
	}


	GetRoom(sluaghRoomX, sluaghRoomY).value()->CloseDoor(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST);
	GetRoom(sluaghRoomX, sluaghRoomY).value()->CloseDoor(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST);
	GetRoom(sluaghRoomX, sluaghRoomY).value()->CloseDoor(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH);
	GetRoom(sluaghRoomX, sluaghRoomY).value()->CloseDoor(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH);

	CloseDoorsToRoom(sluaghRoomX, sluaghRoomY);

	currentRoom->Load();
	LoadAdjacentRooms(currentRoomX, currentRoomY, currentRoomX, currentRoomY);
	blackBoard.currentRoom = currentRoom;
	blackBoard.roomFlowField = currentRoom->GetFlowFieldMap();
	currentRoom->RenderRoom(true);
	currentRoom->InitializeAdjacentFlowFields();

	ProfileReturnVoid;
}

PlayState::~PlayState()
{
	StartProfile;
	CoreInit::subSystems.devConsole->RemoveCommand("tgm");
	CoreInit::subSystems.devConsole->RemoveCommand("give");
	CoreInit::subSystems.devConsole->RemoveCommand("setspeed");
	delete projectileManager;
	delete player;
	//delete currentRoom;
	for (int x = 0; x < worldWidth; x++)
		for (int y = 0; y < worldHeight; y++)
			if (auto room = GetRoom(x, y); room.has_value())
				delete *room;
	CoreInit::managers.entityManager->DestroyNow(dummy);
	CoreInit::managers.entityManager->DestroyNow(usePrompt);
	CoreInit::managers.entityManager->DestroyNow(soundEnt);
	for (auto& s : skillIndicators)
	{
		CoreInit::managers.entityManager->DestroyNow(s.Image);
		CoreInit::managers.entityManager->DestroyNow(s.frame);
	}

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
	if (input->ButtonDown(GameInput::SHOWINFO))
	{
		action.showInfo = true;
	}
	if (input->ButtonPressed(GameInput::ONE))
	{
		action.one = true;
	}
	else if (input->ButtonPressed(GameInput::TWO))
	{
		action.two = true;
	}
	else if (input->ButtonPressed(GameInput::THREE))
	{
		action.three = true;
	}
	else if (input->ButtonPressed(GameInput::FOUR))
	{
		action.four = true;
	}
	else if (input->ButtonPressed(GameInput::FIVE))
	{
		action.five = true;
	}
	int mX, mY;
	input->GetMousePos(mX, mY);

	DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR rayD;


	auto width = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "width", 1280);
	auto height = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "height", 720);
	CoreInit::managers.cameraManager->WorldSpaceRayFromScreenPos(mX, mY, width, height, rayO, rayD);

	float distance = DirectX::XMVectorGetY(rayO) / -XMVectorGetY(rayD);

	auto clickPos = rayO + rayD * distance;

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
template<class T>
void GetRoomPosFromDir(SE::Gameplay::Room::DirectionToAdjacentRoom dir, T& x, T& y)
{

	switch (dir)
	{
	case SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH:
		y+=1;
		break;
	case SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST:
		x += 1;
		break;
	case SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH:
		y -= 1;
		break;
	case SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST:
		x -= 1;
		break;
	case SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NONE:
		break;
	default:
		break;
	}
}


void SE::Gameplay::PlayState::CheckForRoomTransition()
{
	StartProfile;
	auto ducks = currentRoom->CheckForTransition(player->GetXPosition(), player->GetYPosition());
	if(ducks != Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NONE)
	{
		CoreInit::managers.textManager->SetTextPos(usePrompt, CoreInit::subSystems.window->Width() / 2, CoreInit::subSystems.window->Height() / 2);
		CoreInit::managers.textManager->ToggleRenderableText(usePrompt, true);
	}
	else
	{
		CoreInit::managers.textManager->ToggleRenderableText(usePrompt, false);
	}
	if (input->ButtonPressed(uint32_t(GameInput::INTERACT)))
	{
		if (auto dir = currentRoom->CheckForTransition(player->GetXPosition(), player->GetYPosition()); dir != Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NONE)
		{
			int x = currentRoomX, y = currentRoomY;
			GetRoomPosFromDir(dir, x, y);

			if (auto newRoom = GetRoom(x, y); newRoom.has_value())
			{
				UnloadAdjacentRooms(currentRoomX, currentRoomY, x, y);
				LoadAdjacentRooms(x, y, currentRoomX, currentRoomY);

				currentRoom->RenderRoom(false);

				currentRoomX = x;
				currentRoomY = y;
				blackBoard.currentRoom = currentRoom = *newRoom;
				blackBoard.roomFlowField = currentRoom->GetFlowFieldMap();
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
				currentRoom->InitializeAdjacentFlowFields();

				currentRoom->RenderRoom(true);
			

				float xToSet, yToSet;
				currentRoom->GetPositionOfActiveDoor(Room::ReverseDirection(dir), xToSet, yToSet);
				GetRoomPosFromDir(dir, xToSet, yToSet);
				player->PositionEntity(xToSet, yToSet);


				CoreInit::managers.eventManager->TriggerEvent("RoomChange", true);
			}
		
		}
	}

	ProfileReturnVoid;
}

void SE::Gameplay::PlayState::UpdateHUD(float dt)
{
	CoreInit::managers.guiManager->SetTextureDimensions(playStateGUI.GUIButtons.ButtonEntityVec[healthBarPos], playStateGUI.GUIButtons.Buttons[healthBarPos].Width, playStateGUI.GUIButtons.Buttons[healthBarPos].Height * (1 - player->GetHealth() / player->GetMaxHealth()));
	
	for (int i = 0; i < 2; i++)
	{
		if (player->GetCurrentCooldown(i) > 0.0f)
			CoreInit::managers.textManager->SetText(skillIndicators[i].Image, std::to_wstring(int(ceil(player->GetCurrentCooldown(i)))));
		else
			CoreInit::managers.textManager->SetText(skillIndicators[i].Image, L"");
	}
}
static const std::vector<std::tuple<int, int, Room::DirectionToAdjacentRoom>> adjIndices = { { -1,0, Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST },{ 1,0, Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST },{ 0, 1, Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH },{ 0,-1, Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH } };


void SE::Gameplay::PlayState::LoadAdjacentRooms(int x, int y, int sx, int sy)
{
	for (auto a : adjIndices)
	{
		// Load the new adjacent rooms
		int ax = x + std::get<0>(a);
		int ay = y + std::get<1>(a);
		if (!(sx == ax && sy == ay)) // Exluding the one we where in.
		{
			if (auto adjRoom = GetRoom(ax, ay); adjRoom.has_value())
			{
				(*adjRoom)->Load();
			}
		}
	}
}

void SE::Gameplay::PlayState::UnloadAdjacentRooms(int x, int y, int sx, int sy)
{
	for (auto a : adjIndices)
	{
		// Unload adjacent room to previous room
		int ax = x + std::get<0>(a);
		int ay = y + std::get<1>(a);
		if (!(sx == ax && sy == ay)) // Excluding the one we are changing to.
		{
			if (auto adjRoom = GetRoom(ax, ay); adjRoom.has_value())
			{
				(*adjRoom)->Unload();
			}

		}
	}
}

void SE::Gameplay::PlayState::CloseDoorsToRoom(int x, int y)
{
	for (auto a : adjIndices)
	{
		int ax = x + std::get<0>(a);
		int ay = y + std::get<1>(a);
		Room::DirectionToAdjacentRoom dir = std::get<2>(a);
		if (auto adjRoom = GetRoom(ax, ay); adjRoom.has_value())
		{
			adjRoom.value()->CloseDoor(Room::ReverseDirection(dir));
		}
	}
}

void SE::Gameplay::PlayState::OpenDoorsToRoom(int x, int y)
{
	for (auto a : adjIndices)
	{
		int ax = x + std::get<0>(a);
		int ay = y + std::get<1>(a);
		Room::DirectionToAdjacentRoom dir = std::get<2>(a);
		if (auto adjRoom = GetRoom(ax, ay); adjRoom.has_value())
		{
			adjRoom.value()->OpenDoor(Room::ReverseDirection(dir));
		}
	}
}

void PlayState::InitializeRooms()
{
	StartProfile;
	worldWidth = 2;
	worldHeight = 2;
	auto subSystem = engine->GetSubsystems();

	auto s = std::chrono::high_resolution_clock::now();

	std::vector<Utilz::GUID> roomGuids;
	subSystem.resourceHandler->GetAllGUIDsWithExtension("room", roomGuids);

	if (!roomGuids.size())
		throw std::exception("No rooms found");

	rooms = new Room*[worldWidth * worldHeight];
	for (int i = 0; i < worldWidth*worldHeight; i++)
		rooms[i] = new Gameplay::Room(roomGuids[std::rand() % roomGuids.size()]);

	currentRoomX = 0;
	currentRoomY = 0;
	currentRoom = GetRoom(currentRoomX, currentRoomY).value();

	auto e = std::chrono::high_resolution_clock::now();

	auto diff = std::chrono::duration<float>(e - s).count();
	CoreInit::subSystems.devConsole->Print("Room Load Time: %f", diff);
	ProfileReturnVoid;
}
void SE::Gameplay::PlayState::InitializeEnemies()
{
	StartProfile;
	char map[25][25];

	EnemyCreationStruct eStruct;
	int counter = 0;

	for(size_t r = 0; r < worldWidth*worldHeight; r++)
	{
		auto& room = rooms[r];
		room->GetMap(map);
		eStruct.information.clear();
	//	enemiesInEachRoom = 2;
		EnemyUnit** enemies = new EnemyUnit*[enemiesInEachRoom];
		Room::DirectionToAdjacentRoom throwAway;
		for (int i = 0; i < enemiesInEachRoom; i++)
		{
			int x, y;
			pos enemyPos;
			do
			{
				x = CoreInit::subSystems.window->GetRand() % 25;
				y = CoreInit::subSystems.window->GetRand() % 25;
			} while (map[x][y]);// || (room->DistanceToClosestDoor(enemyPos.x, enemyPos.y, throwAway) < 5.5f));


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
			enemyPos.x = x + 0.5f;
			enemyPos.y = y -0.5f;
	
			EnemyCreationData data;
			if (counter < 1)
			{
				data.type = ENEMY_TYPE_BODACH;
			}
			else if(counter < 4)
			{
				if (CoreInit::subSystems.window->GetRand() % 2)
					data.type = ENEMY_TYPE_BODACH;
				else
					data.type = ENEMY_TYPE_GLAISTIG;
			}
			else 
			{
				data.type = ENEMY_TYPE_RANDOM;
			}
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
		if (!(++counter % 3))
			enemiesInEachRoom++;
		delete[] enemies;


	}
	ProfileReturnVoid;
}
#include <Items.h>
void PlayState::InitializePlayer(void* playerInfo)
{
	StartProfile;
	char map[25][25];
	currentRoom->GetMap(map);
	PlayStateData* tempPtr = (PlayStateData*)playerInfo;

	skillIndicators[0].maxCooldown = tempPtr->skills[0].cooldown;
	skillIndicators[1].maxCooldown = tempPtr->skills[1].cooldown;

	skillIndicators[0].frame = CoreInit::managers.entityManager->Create();
	skillIndicators[1].frame = CoreInit::managers.entityManager->Create();
	skillIndicators[0].Image = CoreInit::managers.entityManager->Create();
	skillIndicators[1].Image = CoreInit::managers.entityManager->Create();

	SE::Core::IGUIManager::CreateInfo indicatorInfo;
	indicatorInfo.texture = "Q.png";
	indicatorInfo.textureInfo.posX = 1000;
	indicatorInfo.textureInfo.posY = 600;
	indicatorInfo.textureInfo.height = 100;
	indicatorInfo.textureInfo.width = 100;
	indicatorInfo.textureInfo.layerDepth = 0.001;
	indicatorInfo.textureInfo.anchor = {0.0f, 0.0f};

	SE::Core::ITextManager::CreateInfo textInfo;
	textInfo.font = "CloisterBlack.spritefont";
	textInfo.info.colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	textInfo.info.layerDepth = indicatorInfo.textureInfo.layerDepth / 2;
	textInfo.info.anchor = { 0.0f, 0.0f };
	textInfo.info.height = indicatorInfo.textureInfo.height;
	textInfo.info.width = indicatorInfo.textureInfo.width;
	textInfo.info.posX = indicatorInfo.textureInfo.posX - 15;
	textInfo.info.posY = indicatorInfo.textureInfo.posY;
	textInfo.info.text = L"";

	CoreInit::managers.guiManager->Create(skillIndicators[0].frame, indicatorInfo);
	CoreInit::managers.guiManager->ToggleRenderableTexture(skillIndicators[0].frame, true);
	CoreInit::managers.textManager->Create(skillIndicators[0].Image, textInfo);
	CoreInit::managers.textManager->ToggleRenderableText(skillIndicators[0].Image, true);

	indicatorInfo.texture = "E.png";
	indicatorInfo.textureInfo.posX = 1150;
	indicatorInfo.textureInfo.posY = 600;

	textInfo.info.text = L"";
	textInfo.info.posX = indicatorInfo.textureInfo.posX - 15;
	textInfo.info.posY = indicatorInfo.textureInfo.posY;

	CoreInit::managers.guiManager->Create(skillIndicators[1].frame, indicatorInfo);
	CoreInit::managers.guiManager->ToggleRenderableTexture(skillIndicators[1].frame, true);
	CoreInit::managers.textManager->Create(skillIndicators[1].Image, textInfo);
	CoreInit::managers.textManager->ToggleRenderableText(skillIndicators[1].Image, true);

	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			if (map[x][y] == 0)
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

				auto startWeapon = Item::Weapon::Create(Item::Weapon::Type(std::rand() % 3));
				player->AddItem(startWeapon, 0);
				//auto wc = Item::Copy(startWeapon);
				//player->AddItem(wc, 1);
				//auto pot = Item::Consumable::Create();
				//player->AddItem(pot, 2);
				//auto pc = Item::Copy(pot);
				//player->AddItem(pc, 3);

				//Item::WriteToFile(pot, "sw.itm");
				//auto fromFile = Item::Create("sw.itm");
				//player->AddItem(fromFile, 4);
				return;
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
	cInfo.aspectRatio = CoreInit::subSystems.optionsHandler->GetOptionDouble("Camera", "aspectRatio", (1280.0f / 720.0f));
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
	CoreInit::managers.transformManager->Create(dummy, { 0.0f, 1.95f, 0.0f });
	CoreInit::managers.transformManager->BindChild(player->GetEntity(), dummy, false, true);
	//CoreInit::managers.renderableManager->CreateRenderableObject(dummy, { "Placeholder_Block.mesh" });
	//CoreInit::managers.renderableManager->ToggleRenderableObject(dummy, true);

	SE::Core::ILightManager::CreateInfo lightInfo;
	lightInfo.pos = { 0.0f, 0.0f, 0.0f };
	lightInfo.color = { 0.74f, 0.92f, 0.95f };
	lightInfo.radius = 10.5f;
	lightInfo.intensity = 1.3f;
	CoreInit::managers.lightManager->Create(dummy, lightInfo);
	CoreInit::managers.lightManager->ToggleLight(dummy, true);
	CoreInit::managers.lightManager->SetShadowCaster(dummy);

	usePrompt = CoreInit::managers.entityManager->Create();
	Core::ITextManager::CreateInfo promptCreateInfo;
	promptCreateInfo.info.text = L"f LÄMNA RUMMET";
	promptCreateInfo.info.scale = { 0.3f, 0.3f };
	promptCreateInfo.font = "Knights.spritefont";
	CoreInit::managers.textManager->Create(usePrompt, promptCreateInfo);
	
	CoreInit::subSystems.devConsole->AddCommand([this](DevConsole::IConsole* back, int argc, char** argv) {
		bool god = true;
		if (argc == 2)
			if (std::string(argv[1]) == "0")
				god = false;

		this->player->SetGodMode(god);
		if (god)
			back->Print("Godmode on");
		else
			back->Print("Godmode off");
		
	}, "tgm", "Toggles godmode.");

	CoreInit::subSystems.devConsole->AddCommand([this](DevConsole::IConsole* back, int argc, char** argv) {
		float speed = 5.0f;
		if (argc > 1)
		{
			try
			{
				speed = std::stof(argv[1]);
			}catch(...){}
		}
		this->player->SetSpeed(speed);

	}, "setspeed", "setspeed <value>");
	
	ProfileReturnVoid;
}
#include <Items.h>
void SE::Gameplay::PlayState::InitWeaponPickups()
{
	StartProfile;
	auto pe = player->GetEntity();

	Core::IEventManager::EntityEventCallbacks pickUpEvent;
	pickUpEvent.triggerCallback = [this](const Core::Entity ent) {

		CoreInit::subSystems.devConsole->Print("Picked up item %s.", std::get<std::string>(CoreInit::managers.dataManager->GetValue(ent, "Name", "NaN"s)).c_str());

		if (CoreInit::subSystems.window->ButtonDouble(GameInput::ONE))
		{
			player->AddItem(ent, 0);
		}
		else if (CoreInit::subSystems.window->ButtonDouble(GameInput::TWO))
		{
			player->AddItem(ent, 1);
		}
		else if (CoreInit::subSystems.window->ButtonDouble(GameInput::THREE))
		{
			player->AddItem(ent, 2);
		}
		else if (CoreInit::subSystems.window->ButtonDouble(GameInput::FOUR))
		{
			player->AddItem(ent, 3);
		}
		else if (CoreInit::subSystems.window->ButtonDouble(GameInput::FIVE))
		{
			player->AddItem(ent, 4);
		}

		CoreInit::managers.dataManager->SetValue(ent, "Pickup", true);
	};



	pickUpEvent.triggerCheck = [pe](const Core::Entity ent) {
		if (CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO))
		if (CoreInit::subSystems.window->ButtonDouble(GameInput::PICKUP))
		{
			return CoreInit::managers.collisionManager->CheckCollision(ent, pe);
		}
		return false;
	};

	Core::IEventManager::EntityEventCallbacks startrenderWIC;
	startrenderWIC.triggerCheck = [pe](const Core::Entity ent)
	{
		auto vis = std::get<bool>(CoreInit::managers.dataManager->GetValue(pe, "WICV", false));
		if (vis && !CoreInit::subSystems.window->ButtonPressed(GameInput::PICKUP))
			return false;
		if (!CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO))
			return false;
		return CoreInit::managers.collisionManager->CheckCollision(ent, pe);
	};
	
	startrenderWIC.triggerCallback = [pe, this](const Core::Entity ent)
	{
		CoreInit::managers.dataManager->SetValue(pe, "WICV", true);
		Item::ToggleRenderPickupInfo(ent);
	};

	Core::IEventManager::EntityEventCallbacks stoprenderWIC;
	stoprenderWIC.triggerCheck = [pe](const Core::Entity ent)
	{
		if (auto parent = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(ent, "Parent", false)))
		{
			if (!CoreInit::managers.collisionManager->CheckCollision(*parent, pe)) {
				return true;
			}	
		}

		return (!CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO)) || CoreInit::subSystems.window->ButtonPressed(GameInput::PICKUP);
	};

	stoprenderWIC.triggerCallback = [pe](const Core::Entity ent)
	{
		CoreInit::managers.entityManager->DestroyNow(ent);	
		auto parent = std::get<Core::Entity>(CoreInit::managers.dataManager->GetValue(ent, "Parent", Core::Entity()));
		CoreInit::managers.dataManager->SetValue(pe, "WICV", false);
	};
	CoreInit::managers.eventManager->RegisterEntityEvent("StartRenderWIC", startrenderWIC);
	CoreInit::managers.eventManager->RegisterEntityEvent("StopRenderWIC", stoprenderWIC);
	CoreInit::managers.eventManager->RegisterEntityEvent("ItemPickup", pickUpEvent);


	CoreInit::managers.eventManager->RegisterTriggerEvent("RoomChange", [this](Core::Entity ent) {

		if (auto roomEnt = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(ent, "Room", false)))
		{
			if ((*roomEnt) == currentRoom->GetEntity())
			{
				Item::Drop(ent);
			}
			{
				Item::GodPickup(ent);
			}

		}
	});

	ProfileReturnVoid;
}

IGameState::State PlayState::Update(void*& passableInfo)
{
	StartProfile;
	IGameState::State returnValue = State::PLAY_STATE;
	if(numberOfFreeFrames < 0)
	{
		numberOfFreeFrames--;
		
		ProfileReturn(returnValue);
	}
	
	if (!sluaghDoorsOpen)
	{
		int totalEnemiesLeft = 0;
		for (int x = 0; x < worldWidth; x++)
			for (int y = 0; y < worldHeight; y++)
				totalEnemiesLeft += GetRoom(x, y).value()->NumberOfEnemiesInRoom();

		if (totalEnemiesLeft <= 2) {
			OpenDoorsToRoom(worldWidth - 1, worldHeight - 1);
			sluaghDoorsOpen = true;
			auto sluaghRoom = dynamic_cast<SluaghRoom*>(GetRoom(sluaghRoomX, sluaghRoomY).value());
			sluaghRoom->InitSluagh();
			Core::ITextManager::CreateInfo ti;
			ti.font = "Ancient.spritefont";
			ti.info.text = L"Vägen till din död har öppnats...";
			ti.info.screenAnchor = { 0.5f, 0.5f };
			ti.info.anchor = { 0.5f,0.5f };
			ti.info.colour = { 1.0f, 0.0f, 0.0f, 1.0f };
			auto slt = CoreInit::managers.entityManager->Create();
			CoreInit::managers.textManager->Create(slt, ti);
			CoreInit::managers.textManager->ToggleRenderableText(slt, true);
			CoreInit::managers.eventManager->SetLifetime(slt, 5);


		}
	}


	PlayerUnit::MovementInput movementInput(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput actionInput;
	std::vector<ProjectileData> newProjectiles;

	UpdateInput(movementInput, actionInput);

	float dt = min(1 / 30.f, input->GetDelta());

	projectileManager->CheckCollisionBetweenUnitAndProjectiles(player, Gameplay::ValidTarget::PLAYER);
	player->Update(dt, movementInput, newProjectiles, actionInput);

	UpdateProjectiles(newProjectiles);

	blackBoard.playerPositionX = player->GetXPosition();
	blackBoard.playerPositionY = player->GetYPosition();
	blackBoard.deltaTime = dt;
	blackBoard.playerHealth = player->GetHealth();
	
	currentRoom->Update(dt, player->GetXPosition(), player->GetYPosition());

	projectileManager->AddProjectiles(blackBoard.enemyProjectiles);
	blackBoard.enemyProjectiles.clear();

	//-----sound update
	soundTime += dt;
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
	CheckForRoomTransition();
	UpdateHUD(dt);

	if (!player->IsAlive())
		returnValue = State::GAME_OVER_STATE;

	ProfileReturn(returnValue);

}

