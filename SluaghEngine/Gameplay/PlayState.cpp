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
static std::ofstream streamTimings;
bool firstFrame = true;
PlayState::PlayState(Window::IWindow* Input, SE::Core::IEngine* engine, void* passedInfo)
{
	StartProfile;
	firstFrame = true;
	CoreInit::subSystems.devConsole->AddCommand([this](DevConsole::IConsole* con, int argc, char** argv)
	{
		currentRoom->RemoveEnemyFromRoom(nullptr);
	}, "kill","Kill all enemies");


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
			con->PrintChannel("give", "give ItemType Type Slot");
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
	CoreInit::subSystems.devConsole->AddCommand([this](DevConsole::IConsole* con, int argc, char** argv)
	{
		if (argc >= 3)
		{
			auto type = ItemType(std::stoi(argv[1]));
			switch (type)
			{
			case SE::Gameplay::ItemType::WEAPON:
			{
				auto wType = Item::Weapon::Type(std::stoi(argv[2]));
				auto pos = CoreInit::managers.transformManager->GetPosition(player->GetEntity());
				Item::Drop(Item::Weapon::Create(wType), pos);
				break;
			}

			case SE::Gameplay::ItemType::CONSUMABLE:
			{
				auto cType = Item::Consumable::Type(std::stoi(argv[2]));
				auto pos = CoreInit::managers.transformManager->GetPosition(player->GetEntity());
				Item::Drop(Item::Consumable::Create(cType), pos);
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
			con->PrintChannel("drop", "Usage:");
			con->PrintChannel("drop", "drop ItemType Type");
			con->PrintChannel("drop", "ItemTypes:");
			con->PrintChannel("drop", "\tWeapon - 0:");
			con->PrintChannel("drop", "\tConsumable - 1");
			con->PrintChannel("drop", "");
			con->PrintChannel("drop", "WeaponTypes:");
			con->PrintChannel("drop", "\tSword - 0");
			con->PrintChannel("drop", "\tCrossbow - 1");
			con->PrintChannel("drop", "\tWand - 2");
			con->PrintChannel("drop", "ConsumableTypes:");
			con->PrintChannel("drop", "\tHp - 0");
		}
	}, "drop", "Drop item at players feet");
	this->input = Input;
	this->engine = engine;

	playStateGUI.ParseFiles("PlayStateGui.HuD");
	playStateGUI.InitiateTextures();
	int tempPos = 0;
	for (auto& button : playStateGUI.ButtonVector)
	{
		if (button.rectName == "HealthBar")
		{
			bool perhaps = false;
			bool fullscreen = CoreInit::subSystems.optionsHandler->GetOptionBool("Window", "fullScreen", perhaps);
			if (fullscreen)
			{
				button.Width = button.Width / 1.5;
				button.Height = button.Height / 1.5;
				button.PositionX = button.PositionX / 1.5;
				button.PositionY = button.PositionY / 1.5;
			}
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
	delete rooms[sluaghRoomX * worldHeight + sluaghRoomY].room;
	rooms[sluaghRoomX * worldHeight  + sluaghRoomY].room = new SluaghRoom("Room18.room", player, projectileManager);


	for (int x = 0; x < worldWidth; x++)
	{
		for (int y = 0; y < worldHeight; y++)
		{
			auto room = GetRoom(x, y)->get();
			if (auto leftRoom = GetRoom(x - 1, y); leftRoom.has_value())
				room.room->AddAdjacentRoomByDirection(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST, (leftRoom->get().room));
			if (auto rightRoom = GetRoom(x + 1, y); rightRoom.has_value())
				room.room->AddAdjacentRoomByDirection(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST, (rightRoom->get().room));
			if (auto upRoom = GetRoom(x, y + 1); upRoom.has_value())
				room.room->AddAdjacentRoomByDirection(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH, (upRoom->get().room));
			if (auto downRoom = GetRoom(x, y - 1); downRoom.has_value())
				room.room->AddAdjacentRoomByDirection(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH, (downRoom->get().room));
		}
	}


	GetRoom(sluaghRoomX, sluaghRoomY)->get().room->CloseDoor(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST);
	GetRoom(sluaghRoomX, sluaghRoomY)->get().room->CloseDoor(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST);
	GetRoom(sluaghRoomX, sluaghRoomY)->get().room->CloseDoor(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH);
	GetRoom(sluaghRoomX, sluaghRoomY)->get().room->CloseDoor(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH);

	CloseDoorsToRoom(sluaghRoomX, sluaghRoomY);

	currentRoom->Load();
	auto enemiesInRoom = currentRoom->GetEnemiesInRoom();
	for (auto enemy : enemiesInRoom)
		enemy->SetEntity(eFactory.CreateEntityDataForEnemyType(enemy->GetType()));

	LoadAdjacentRooms(currentRoomX, currentRoomY, currentRoomX, currentRoomY);
	blackBoard.currentRoom = currentRoom;
	blackBoard.roomFlowField = currentRoom->GetFlowFieldMap();
	currentRoom->RenderRoom(true);
	currentRoom->InitializeAdjacentFlowFields();

	CoreInit::subSystems.window->MapActionButton(Window::KeyReturn, Window::KeyReturn);

	struct
	{
		float x, y, z;
	} vertices[4];
	
	vertices[0].x = -100.0f; vertices[0].y = -0.01f; vertices[0].z = -100.0f;
	vertices[1].x = -100.0f; vertices[1].y = -0.01f; vertices[1].z = 100.0f;
	vertices[2].x = 100.0f; vertices[2].y = -0.01f; vertices[2].z = -100.0f;
	vertices[3].x = 100.0f; vertices[3].y = -0.01f; vertices[3].z = 100.0f;

	CoreInit::subSystems.renderer->GetPipelineHandler()->CreateVertexBuffer("DummyPlane", vertices, 4, sizeof(float) * 3, false);
	const std::string dummyShader = "cbuffer OncePerFrame : register(b1)\
	{\
		float4x4 View;\
		float4x4 ViewProj;\
	};\
	\
	struct VS_IN\
	{\
		float3 Pos : POSITION;\
	};\
	\
	struct VS_OUT\
	{\
		float4 Pos : SV_Position;\
	};\
	\
	VS_OUT VS_main(VS_IN input)\
	{\
		VS_OUT output = (VS_OUT)0;\
		output.Pos = mul(float4(input.Pos, 1.0f), ViewProj);\
		return output;\
	}";
	CoreInit::subSystems.renderer->GetPipelineHandler()->CreateVertexShaderFromSource("DummyPlaneVS.hlsl", dummyShader, "VS_main", "vs_5_0");
	Graphics::RenderJob dummyJob;
	dummyJob.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_STRIP;
	dummyJob.pipeline.IAStage.inputLayout = "DummyPlaneVS.hlsl";
	dummyJob.pipeline.IAStage.vertexBuffer = "DummyPlane";
	dummyJob.pipeline.VSStage.shader = "DummyPlaneVS.hlsl";
	dummyJob.pipeline.OMStage.renderTargetCount = 1;
	dummyJob.pipeline.OMStage.renderTargets[0] = "backbuffer";
	dummyJob.pipeline.OMStage.depthStencilView = "backbuffer";
	dummyJob.vertexCount = 4;
	dummyBoxJobID = CoreInit::subSystems.renderer->AddRenderJob(dummyJob, Graphics::RenderGroup::RENDER_PASS_5);

	CreateMiniMap(200, 170);

	ProfileReturnVoid;
}

PlayState::~PlayState()
{
	StartProfile;

	//Show the cursor again if we exit the playstate
	CoreInit::subSystems.window->ToggleCursor(true);
	CoreInit::subSystems.renderer->RemoveRenderJob(dummyBoxJobID);
	if (streamTimings.is_open())
		streamTimings.close();
	CoreInit::subSystems.devConsole->RemoveCommand("tgm");
	CoreInit::subSystems.devConsole->RemoveCommand("give");
	CoreInit::subSystems.devConsole->RemoveCommand("kill");
	CoreInit::subSystems.devConsole->RemoveCommand("drop");
	CoreInit::subSystems.devConsole->RemoveCommand("setspeed");
	CoreInit::subSystems.devConsole->RemoveCommand("tff");
	CoreInit::subSystems.devConsole->RemoveCommand("killself");
	
	delete projectileManager;
	delete player;
	//delete currentRoom;
	for (int x = 0; x < worldWidth; x++)
		for (int y = 0; y < worldHeight; y++)
			if (auto room = GetRoom(x, y); room.has_value())
				delete room->get().room;
	CoreInit::managers.entityManager->DestroyNow(dummy);
	CoreInit::managers.entityManager->DestroyNow(cameraDummy);
	CoreInit::managers.entityManager->DestroyNow(deathText);
	CoreInit::managers.entityManager->DestroyNow(returnPrompt);
	CoreInit::managers.entityManager->DestroyNow(usePrompt);
	CoreInit::managers.entityManager->DestroyNow(soundEnt);
	for (auto& s : skillIndicators)
	{
		CoreInit::managers.entityManager->DestroyNow(s.Image);
		CoreInit::managers.entityManager->DestroyNow(s.frame);
	}

	for (auto entity : flowFieldEntities)
		CoreInit::managers.entityManager->Destroy(entity);
	flowFieldEntities.clear();

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
#include <Utilz\CPUTimeCluster.h>
#include <Utilz\Memory.h>
static int streamCount = 0;
static SE::Utilz::CPUTimeCluster streamingTimes;

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
			streamingTimes.Start("Full");
			int x = currentRoomX, y = currentRoomY;
			GetRoomPosFromDir(dir, x, y);

			if (auto newRoom = GetRoom(x, y); newRoom.has_value())
			{
				// Set new room symbol to green
				auto debug = newRoom->get().symbol;
				CoreInit::managers.guiManager->SetTexture(newRoom->get().symbol, "InRoom.jpg");

				// Grey out the symbol of the old room
				auto oldRoom = GetRoom(currentRoomX, currentRoomY)->get();
				CoreInit::managers.guiManager->SetTexture(oldRoom.symbol, "VisitedRoom.jpg");

				streamingTimes.Start("Unload");
				UnloadAdjacentRooms(currentRoomX, currentRoomY, x, y);
				streamingTimes.Stop("Unload");
				streamingTimes.Start("Load");
				LoadAdjacentRooms(x, y, currentRoomX, currentRoomY);
				streamingTimes.Stop("Load");
				streamingTimes.Start("StopRender");
				currentRoom->RenderRoom(false);
				streamingTimes.Stop("StopRender");

				streamingTimes.Start("Other");
				currentRoomX = x;
				currentRoomY = y;
				blackBoard.currentRoom = currentRoom = newRoom->get().room;
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

				streamingTimes.Stop("Other");

				streamingTimes.Start("StartRender");
				currentRoom->RenderRoom(true);
				streamingTimes.Stop("StartRender");

				float xToSet, yToSet;
				currentRoom->GetPositionOfActiveDoor(Room::ReverseDirection(dir), xToSet, yToSet);
				GetRoomPosFromDir(dir, xToSet, yToSet);
				player->PositionEntity(xToSet, yToSet);


				CoreInit::managers.eventManager->TriggerEvent("RoomChange", true);

				if(showFlowField)
				{
					DestroyFlowFieldRendering();
					CreateFlowFieldRendering();
					UpdateFlowFieldRendering();
				}
			}
			streamingTimes.Stop("Full");

			Utilz::TimeMap times;
			streamingTimes.GetMap(times);
			if (firstFrame)
			{
				streamTimings.open("StreamTimings.csv", std::ios::trunc);
				if (streamTimings.is_open())
				{
					streamTimings << "count,RAM_T,VRAM_T,RAM_R,VRAM_R";
					for (auto& t : times)
						streamTimings << "," << t.first.c_str();
				}
				firstFrame = false;
			}
			if (streamTimings.is_open())
			{
				auto rhi = CoreInit::subSystems.resourceHandler->GetInfo();
				
				streamTimings << std::endl;
				streamTimings << streamCount++;
				streamTimings << "," << toMB(rhi.RAM.getCurrentMemoryUsage());
				streamTimings << "," << toMB(rhi.VRAM.getCurrentMemoryUsage());
				streamTimings << "," << toMB(CoreInit::subSystems.resourceHandler->GetMemoryUsed(ResourceHandler::ResourceType::RAM)); // Should be from resoruce handler
				streamTimings << "," << toMB(CoreInit::subSystems.resourceHandler->GetMemoryUsed(ResourceHandler::ResourceType::VRAM));// Should be from resoruce handler
				for (auto& t : times)
				{
					streamTimings << "," << t.second;
					CoreInit::subSystems.devConsole->PrintChannel("Streaming", "%s - %f", t.first.c_str(), t.second);
				}
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

std::wstring SE::Gameplay::PlayState::GenerateDeathMessage() {

	int value = CoreInit::subSystems.window->GetRand() % 11;

	switch (value) {

	case 0:
		return L"DU ÄR DÖD";
		
	case 1:
		return L"BÄTTRE LYCKA NÄSTA GÅNG";
		
	case 2:
		return L"DU HAR AVLIDIT. FÖRSÖK IGEN.";

	case 3:
		return L"SPELARTIPS: DÖ INTE";

	case 4:
		return L"DU GÅR MOT LJUSET...";

	case 5:
		return L"NU SLIPPER DU SKOTTEN I ALLA FALL";

	case 6:
		return L"HIMLEN HAR FÅTT SIG EN NY ÄNGEL";

	case 7:
		return L"ÄR DU HÄR IGEN?";

	case 8:
		return L"SPELARTIPS: DU KAN FÖRSVARA DIG";

	case 9:
		return L"SPELARTIPS: FIENDERNA ÄR FARLIGA";

	case 10:
		return L"FÖRSÖKER DU ENS?";

	}

	return L"NÅGONTING HAR GÅTT SNETT";
}

void SE::Gameplay::PlayState::InitializeDeathSequence() {

	deathText = CoreInit::managers.entityManager->Create();
	returnPrompt = CoreInit::managers.entityManager->Create();

	Core::ITextManager::CreateInfo deathInfo;
	deathInfo.info.text = GenerateDeathMessage();
	deathInfo.info.scale = { 0.7f, 0.7f };
	deathInfo.info.posX = 0;
	deathInfo.info.posY = 0;
	deathInfo.info.anchor = { 0.5f, 0.5f };
	deathInfo.info.screenAnchor = { 0.5f, 0.25f };
	deathInfo.font = "Knights.spritefont";

	CoreInit::managers.textManager->Create(deathText, deathInfo);

	deathInfo.info.text = L"TRYCK RETUR FÖR ATT ÅTERGÅ TILL MENYN";
	deathInfo.info.scale = { 0.3f, 0.3f };
	deathInfo.info.posX = 0;
	deathInfo.info.posY = 0;
	deathInfo.info.anchor = { 0.5f, 0.5f };
	deathInfo.info.screenAnchor = { 0.5f, 0.90f };

	CoreInit::managers.textManager->Create(returnPrompt, deathInfo);

	CoreInit::managers.textManager->SetTextColour(deathText, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });
	CoreInit::managers.textManager->SetTextColour(returnPrompt, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });
	CoreInit::managers.textManager->ToggleRenderableText(deathText, true);
	CoreInit::managers.textManager->ToggleRenderableText(returnPrompt, true);

	// Create dummy entity and initialize it with player position
	cameraDummy = CoreInit::managers.entityManager->Create();

	// Get the player entity
	auto playerEntity = player->GetEntity();

	XMFLOAT3 playerPos = CoreInit::managers.transformManager->GetPosition(playerEntity);

	CoreInit::managers.transformManager->Create(cameraDummy, playerPos);

	XMFLOAT3 playerForward = CoreInit::managers.transformManager->GetForward(playerEntity);

	// We must unbind the camera from the player
	CoreInit::managers.transformManager->UnbindChild(cam);

	// Bind the camera to the dummy entity
	CoreInit::managers.transformManager->BindChild(cameraDummy, cam, true, false);
}

void SE::Gameplay::PlayState::UpdateDeathCamera(float dt, float rotValue, float zoomValue, float zoomLimit) {

	auto cameraTranslation = DirectX::XMVECTOR{ 0.0f, -0.01f, 0.0f, 1.0f };
	CoreInit::managers.transformManager->Rotate(cameraDummy, 0.00f, rotValue * dt, 0.0f);

	XMFLOAT3 camPos = CoreInit::managers.transformManager->GetPosition(cam);
	XMFLOAT3 dummyPos = CoreInit::managers.transformManager->GetPosition(cameraDummy);

	XMVECTOR camPosXM = XMLoadFloat3(&camPos);
	XMVECTOR dummyPosXM = XMLoadFloat3(&dummyPos);

	XMFLOAT3 difVec;
	XMStoreFloat3(&difVec, XMVector3Normalize(dummyPosXM - camPosXM) * zoomValue * dt);

	if(camPos.y >= zoomLimit){

		CoreInit::managers.transformManager->Move(cam, difVec);

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
				(adjRoom)->get().room->Load();
				auto enemiesInRoom = (adjRoom)->get().room->GetEnemiesInRoom();
				for (auto enemy : enemiesInRoom)
					enemy->SetEntity(eFactory.CreateEntityDataForEnemyType(enemy->GetType()));
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
				(adjRoom)->get().room->Unload();
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
			adjRoom->get().room->CloseDoor(Room::ReverseDirection(dir));
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
			adjRoom->get().room->OpenDoor(Room::ReverseDirection(dir));
		}
	}
}

void PlayState::InitializeRooms()
{
	StartProfile;
	worldWidth = 9;
	worldHeight = 9;
	auto subSystem = engine->GetSubsystems();

	auto s = std::chrono::high_resolution_clock::now();

	std::vector<Utilz::GUID> roomGuids;
	subSystem.resourceHandler->GetAllGUIDsWithExtension("room", roomGuids);

	if (!roomGuids.size())
		throw std::exception("No rooms found");

	rooms = new RoomContainer[worldWidth * worldHeight];
	for (int i = 0; i < worldWidth*worldHeight; i++)
		rooms[i].room = new Gameplay::Room(roomGuids[std::rand() % roomGuids.size()]);

	currentRoomX = 0;
	currentRoomY = 0;
	currentRoom = GetRoom(currentRoomX, currentRoomY)->get().room;

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
		auto& room = rooms[r].room;
		room->GetMap(map);
		eStruct.information.clear();
	//	enemiesInEachRoom = 2;
		EnemyUnit** enemies = new EnemyUnit*[enemiesInEachRoom];
		Room::DirectionToAdjacentRoom throwAway;
		for (int i = 0; i < enemiesInEachRoom; i++)
		{
			int x, y;
			float playerDistanceX = 0.f;
			float playerDistanceY = 0.f;
			pos enemyPos;
			do
			{
				x = CoreInit::subSystems.window->GetRand() % 25;
				y = CoreInit::subSystems.window->GetRand() % 25;
				playerDistanceX = player->GetXPosition() - x;
				playerDistanceY = player->GetYPosition() - y;
			} while (map[x][y] || (room->DistanceToClosestDoor(x, y, throwAway) < 5.5f) || sqrtf(playerDistanceX*playerDistanceX + playerDistanceY*playerDistanceY) < 5.5f);


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
					data.type = EnemyType(std::rand() % 3);
			}
			data.startX = enemyPos.x;
			data.startY = enemyPos.y;
			data.useVariation = true;
			//eStruct.information.push_back(data);
		
			auto enemy = eFactory.CreateEnemyDataForEnemyType(data.type, data.useVariation);
			enemy->SetBehaviouralTree(eFactory.CreateBehaviouralTreeForEnemyType(data.type, &blackBoard, enemy->GetEnemyBlackboard()));
			//enemy->SetEntity(eFactory.CreateEntityDataForEnemyType(data.type));
			enemy->PositionEntity(data.startX, data.startY);
			room->AddEnemyToRoom(enemy);
		}

	/*	eFactory.CreateEnemies(eStruct, &blackBoard, enemies);

		for (int i = 0; i < enemiesInEachRoom; i++)
		{
			room->AddEnemyToRoom(enemies[i]);
		}*/
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
				player = new Gameplay::PlayerUnit(tempPtr->skills, tempPtr->perks, x + (0.5f + xOffset), y + (0.5f + yOffset), map);
				
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

				/*std::ofstream out("sw.itm", std::ios::binary);
				if(out.is_open())
				{
					Item::WriteToFile(startWeapon,out);
					Item::WriteToFile(startWeapon, out);
					out.close();
					std::ifstream in("sw.itm", std::ios::binary);
					if (in.is_open())
					{
						auto fromFile = Item::Create(in);
						player->AddItem(fromFile, 3);
						fromFile = Item::Create(in);
						player->AddItem(fromFile, 4);
					}
				}*/

				
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
	cam = CoreInit::managers.entityManager->Create();
	CoreInit::managers.cameraManager->Create(cam, cInfo);
	CoreInit::managers.cameraManager->SetActive(cam);

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

	aimDecal = CoreInit::managers.entityManager->Create();
	DirectX::XMFLOAT4X4 dectrans;
	DirectX::XMStoreFloat4x4(&dectrans,DirectX::XMMatrixScaling(0.5f, 0.5f, 0.05f) * DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PIDIV2, 0.0f, 0.0f));
	CoreInit::managers.decalManager->Create(aimDecal, { "AimDecal.png", 0.5f});
	CoreInit::managers.decalManager->SetLocalTransform(aimDecal, (float*)&dectrans);
	CoreInit::managers.decalManager->ToggleVisible(aimDecal, true);
	
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

	CoreInit::subSystems.devConsole->AddCommand([this](DevConsole::IConsole* back, int argc, char** argv) {
		this->player->Suicide();

	}, "killself", "Kills the player character");
	
	
	CoreInit::subSystems.devConsole->AddCommand([this](DevConsole::IConsole* back, int argc, char** argv) {
		
		this->ToggleFlowField(!this->showFlowField);
		if (this->showFlowField)
		{
			back->Print("FlowField Rendering on");
			
		}
		else
			back->Print("FlowField Rendering off");

	}, "tff", "Toggles FlowField Rendering.");

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
		CoreInit::managers.eventManager->TriggerEvent("StopRenderItemInfo", false);
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
	CoreInit::managers.eventManager->RegisterEntityEvent("ItemPickup", pickUpEvent);

	Core::IEventManager::EntityEventCallbacks renderItemInfoEC;
	renderItemInfoEC.triggerCheck = [pe](const Core::Entity ent)
	{
		if (!CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO))
			return false;

		if (auto visible = std::get<bool>(CoreInit::managers.dataManager->GetValue(pe, "InfoVisible", false)); visible)
			return false;

		return CoreInit::managers.collisionManager->CheckCollision(ent, pe);
	};

	renderItemInfoEC.triggerCallback = [pe, this](const Core::Entity ent)
	{
		CoreInit::managers.eventManager->TriggerEvent("StopRenderItemInfo", true);
		CoreInit::subSystems.devConsole->PrintChannel("Debug", "Render");
		Item::RenderItemInfo(ent, player->GetItemToCompareWith());
		CoreInit::managers.dataManager->SetValue(pe, "InfoVisible", true);
		CoreInit::managers.dataManager->SetValue(pe, "InfoItem", ent);

	};

	CoreInit::managers.eventManager->RegisterEntityEvent("RenderItemInfo", renderItemInfoEC);
	CoreInit::managers.eventManager->RegisterTriggerEvent("StopRenderItemInfo", [pe](const Core::Entity ent) {
		CoreInit::subSystems.devConsole->PrintChannel("Debug", "StopRender");
		CoreInit::managers.entityManager->Destroy(ent);
		CoreInit::managers.dataManager->SetValue(pe, "InfoVisible", false);
	});



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

void PlayState::CreateFlowFieldRendering()
{
	StartProfile;
	auto flowField = currentRoom->GetFlowFieldMap();

	for(int x = 0; x < 25; x++)
		for(int y = 0; y < 25; y++)
		{
			if(!flowField->IsBlocked(x, y))
			{
				auto arrowEntity = CoreInit::managers.entityManager->Create();
				
				CoreInit::managers.transformManager->Create(arrowEntity, XMFLOAT3{ float(x) + 0.5f, 1.5f, float(y) + 0.5f });
				CoreInit::managers.transformManager->Scale(arrowEntity, 0.5f);
				CoreInit::managers.renderableManager->CreateRenderableObject(arrowEntity, { "Arrow.mesh" });
				CoreInit::managers.renderableManager->ToggleRenderableObject(arrowEntity, true);
				flowFieldEntities.push_back(arrowEntity);
			}
		}
	StopProfile;
}

void PlayState::DestroyFlowFieldRendering()
{
	StartProfile;
	for (auto entity : flowFieldEntities)
		CoreInit::managers.entityManager->Destroy(entity);
	flowFieldEntities.clear();
	StopProfile;
}

void PlayState::UpdateFlowFieldRendering()
{
	StartProfile;
	auto flowField = currentRoom->GetFlowFieldMap();
	int counter = 0;
	float xRot;
	float yRot;
	pos mapPos;
	for(int x = 0; x < 25; x++)
		for(int y = 0; y < 25; y++)
		{ 
			if (!flowField->IsBlocked(x, y))
			{
				mapPos = { float(x), float(y) };
				flowField->SampleFromMap(mapPos, xRot, yRot);

				if (xRot == 1.0f)
				{
					CoreInit::managers.transformManager->SetRotation(flowFieldEntities[counter], 0.0f, 0.0f, 0.0f);
				}
				else if (xRot == -1.0f)
				{
					CoreInit::managers.transformManager->SetRotation(flowFieldEntities[counter], 0.0f, DirectX::XM_PI, 0.0f);
				}
				else if (yRot == 1.0f)
				{
					CoreInit::managers.transformManager->SetRotation(flowFieldEntities[counter], 0.0f, -DirectX::XM_PIDIV2, 0.0f);
				}
				else if (yRot == -1.0f)
				{
					CoreInit::managers.transformManager->SetRotation(flowFieldEntities[counter], 0.0f, DirectX::XM_PIDIV2, 0.0f);

				}
				else if (xRot == 0.707f)
				{
					if (yRot == 0.707f)
					{
						CoreInit::managers.transformManager->SetRotation(flowFieldEntities[counter], 0.0f, -DirectX::XM_PIDIV4, 0.0f);
					}
					else if (yRot == -0.707f)
					{
						CoreInit::managers.transformManager->SetRotation(flowFieldEntities[counter], 0.0f, DirectX::XM_PIDIV4, 0.0f);
					}
				}
				else if (xRot == -0.707f)
				{
					if (yRot == 0.707f)
					{
						CoreInit::managers.transformManager->SetRotation(flowFieldEntities[counter], 0.0f, DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0f);
					}
					else if (yRot == -0.707f)
					{
						CoreInit::managers.transformManager->SetRotation(flowFieldEntities[counter], 0.0f, DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0f);
					}
				}

				counter++;
			}
			
		}
	StopProfile;
}

void PlayState::CreateMiniMap(const int width, const int height) {

	// Create the entity for the mini map frame
	miniMap.map = CoreInit::managers.entityManager->Create();

	// Create the information for the minimap
	Core::IGUIManager::CreateInfo miniMapInfo;
	miniMapInfo.texture = "MiniMap.jpg";
	miniMapInfo.textureInfo.colour = XMFLOAT4{ 0.0f, 0.0f, 0.0f, 0.5f };
	miniMapInfo.textureInfo.posX = 0.0f - 10.0f;
	miniMapInfo.textureInfo.posY = 0.0f;
	miniMapInfo.textureInfo.height = height;
	miniMapInfo.textureInfo.width = width;
	miniMapInfo.textureInfo.anchor = { 1.0f, 0.0f };
	miniMapInfo.textureInfo.screenAnchor = { 1.0f, 0.0f };
	miniMapInfo.textureInfo.layerDepth = 0.011f;

	// Create the GUI element
	CoreInit::managers.guiManager->Create(miniMap.map, miniMapInfo);
	CoreInit::managers.guiManager->ToggleRenderableTexture(miniMap.map, true);
	
	// Loop through the rooms
	for (int x = 0; x < worldWidth; x++) {

		for (int y = 0; y < worldHeight; y++) {

			// Get the room for the given coordinates
			auto& room = GetRoom(x, y)->get();

			// Only display the symbol if the room is active
			if (room.room != nullptr) {

				// Create the information for the minimap symbols
				// Create the information for the minimap
				
				Core::IGUIManager::CreateInfo roomSymbolInfo;
				roomSymbolInfo.texture = "EmptyRoom.jpg";
				roomSymbolInfo.textureInfo.colour = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
				roomSymbolInfo.textureInfo.posX = (0.0f - x * width / 12) - 35;
				roomSymbolInfo.textureInfo.posY = (0.0f - y * height / 12) + 135;
				roomSymbolInfo.textureInfo.height = height / 16;
				roomSymbolInfo.textureInfo.width = width / 16;
				roomSymbolInfo.textureInfo.anchor = { 1.0f, 0.0f };
				roomSymbolInfo.textureInfo.screenAnchor = { 1.0f, 0.0f };
				roomSymbolInfo.textureInfo.layerDepth = 0.010f;

				if (currentRoomX == x && currentRoomY == y) {

					roomSymbolInfo.texture = "InRoom.jpg";
				}

				if (sluaghRoomX == x && sluaghRoomY == y) {

					roomSymbolInfo.texture = "SlaughRoom.jpg";
				}

				// Create the GUI element
				room.symbol = CoreInit::managers.entityManager->Create();
				CoreInit::managers.guiManager->Create(room.symbol, roomSymbolInfo);
				CoreInit::managers.guiManager->ToggleRenderableTexture(room.symbol, true);
			}
			
		}
	}

}

IGameState::State PlayState::Update(void*& passableInfo)
{
	StartProfile;
	IGameState::State returnValue = State::PLAY_STATE;
	//Only show cursor if the devConsole is on
	CoreInit::subSystems.window->ToggleCursor(CoreInit::subSystems.devConsole->IsVisible());
	if(numberOfFreeFrames < 0)
	{
		numberOfFreeFrames--;
		
		ProfileReturn(returnValue);
	}
	
	if (!input->ButtonDown(GameInput::SHOWINFO))
	{
		CoreInit::managers.eventManager->TriggerEvent("StopRenderItemInfo", true);
	}
	else
	{
		auto pe = player->GetEntity();
		if (auto visible = std::get<bool>(CoreInit::managers.dataManager->GetValue(pe, "InfoVisible", false)); visible)
		{
			auto item = std::get<Core::Entity>(CoreInit::managers.dataManager->GetValue(pe, "InfoItem", Core::Entity()));
			if(!CoreInit::managers.collisionManager->CheckCollision(pe, item))
				CoreInit::managers.eventManager->TriggerEvent("StopRenderItemInfo", true);
		}
		
	}
	if (!sluaghDoorsOpen)
	{
		int totalEnemiesLeft = 0;
		for (int x = 0; x < worldWidth; x++)
			for (int y = 0; y < worldHeight; y++)
				totalEnemiesLeft += GetRoom(x, y)->get().room->NumberOfEnemiesInRoom();

		if (totalEnemiesLeft <= 2) {
			OpenDoorsToRoom(worldWidth - 1, worldHeight - 1);
			sluaghDoorsOpen = true;
			auto sluaghRoom = dynamic_cast<SluaghRoom*>(GetRoom(sluaghRoomX, sluaghRoomY)->get().room);
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
	UpdateAimDecal();

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

	
	if(sluaghDoorsOpen)
	{
		auto sluaghRoom = dynamic_cast<SluaghRoom*>(GetRoom(sluaghRoomX, sluaghRoomY)->get().room);
		if(sluaghRoom)
		{
			if (sluaghRoom->GetSluagh()->GetSluagh()->GetHealth() <= 0.0f)
			{
				returnValue = State::WIN_STATE;

				char* appdataBuffer;
				size_t bcount;
				if (_dupenv_s(&appdataBuffer, &bcount, "APPDATA"))
					throw std::exception("Failed to retrieve path to appdata.");
				std::string appdata(appdataBuffer);
				free(appdataBuffer);
				appdata += "\\Sluagh\\";
				std::ofstream out(appdata + "sluaghFile.sluagh", std::ios::out | std::ios::binary);
				player->SavePlayerToFile(out);
				out.close();
			}
		}
	}
	if (!player->IsAlive() && deathSequence == false) {

		deathSequence = true;
		InitializeDeathSequence();
	}

	if(deathSequence == true){

		if (CoreInit::subSystems.window->ButtonPressed(Window::KeyReturn)) {

			returnValue = State::MAIN_MENU_STATE;
		}

		deathTimer += dt;

		UpdateDeathCamera(dt, -0.5f, 0.2f, 3.0f);
		
		if (deathTimer > 15){
			deathTimer = 0.0f;
			returnValue = State::MAIN_MENU_STATE;
		}
	}

	if (showFlowField)
		UpdateFlowFieldRendering();

	ProfileReturn(returnValue);

}

void PlayState::ToggleFlowField(bool showFlowField)
{
	StartProfile;
	if(this->showFlowField != showFlowField)
	{
		this->showFlowField = showFlowField;
		if(showFlowField)
		{
			CreateFlowFieldRendering();
			UpdateFlowFieldRendering();
		}
		else
		{
			DestroyFlowFieldRendering();
		}
	}
	StopProfile;
}

void PlayState::UpdateAimDecal()
{
	int mX, mY;
	input->GetMousePos(mX, mY);

	DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR rayD;


	auto width = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "width", 1280);
	auto height = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "height", 720);
	CoreInit::managers.cameraManager->WorldSpaceRayFromScreenPos(mX, mY, width, height, rayO, rayD);

	float distance = DirectX::XMVectorGetY(rayO) / -XMVectorGetY(rayD);

	auto clickPos = rayO + rayD * distance;

	DirectX::XMFLOAT3 decalPos;
	DirectX::XMStoreFloat3(&decalPos, clickPos);
	if (currentRoom->IsWall(decalPos.x, decalPos.z))
		decalPos.y = 3.0f;
	
	CoreInit::managers.transformManager->SetPosition(aimDecal, decalPos);

}
