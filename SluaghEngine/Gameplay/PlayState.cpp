#include "PlayState.h"
#include <Profiler.h>
#include <Utilz\Tools.h>
#include "CoreInit.h"

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

PlayState::PlayState(Window::IWindow* Input, SE::Core::IEngine* engine)
{
	this->input = Input;
	this->engine = engine;

	InitializeRooms();
	InitializePlayer();
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
	uint32_t nrOfRooms;
	Utilz::GUID* RoomArr;
	auto subSystem = engine->GetSubsystems();
	int nrOfRoomsToCreate = 10;
	int nrOfRoomsCreated = 0;
	int nrOfOpenDoors = 0;

	subSystem.resourceHandler->LoadResource("RoomGeneration.txt", [&nrOfRooms, &RoomArr](auto GUID, auto data, auto size)
	{
		nrOfRooms = *(uint32_t *)data;
		RoomArr = new Utilz::GUID[nrOfRooms];
		memcpy(RoomArr, (char*)data + sizeof(uint32_t), sizeof(Utilz::GUID) * nrOfRooms);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});

	while (nrOfOpenDoors != 0 || nrOfRoomsCreated < nrOfRoomsToCreate)
	{
		//Skips nrOfOpenDoors for now since I don't know how many doors a room has got
		int random = CoreInit::subSystems.window->GetRand() % nrOfRooms;

		Gameplay::Room* temp = new Gameplay::Room(RoomArr[random]);
		rooms.push_back(temp);
		nrOfRoomsCreated++;
		temp->RenderRoom(false);

	}

	currentRoom = rooms[0];
	currentRoom->RenderRoom(true);
	delete[] RoomArr;

}
void PlayState::InitializePlayer()
{
	char map[25][25];
	currentRoom->GetMap(map);

	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			if (currentRoom->tileValues[x][y] == 1)
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
				player = new Gameplay::PlayerUnit(nullptr, nullptr, x + (0.5f + xOffset), y + (0.5f + yOffset), currentRoom->tileValues);
				
				player->SetZPosition(1.0f);
				player->PositionEntity(x + (0.5f + xOffset), y + (0.5f + yOffset));
				break;
			}
		}
	}

	//CoreInit::managers.transformManager->SetPosition(player->GetEntity(), DirectX::XMFLOAT3(1.5f, 0.9f, 1.5f));

	CoreInit::managers.transformManager->SetScale(player->GetEntity(), 1.f);
	CoreInit::managers.renderableManager->CreateRenderableObject(player->GetEntity(), { "MCModell.mesh" });

	Core::IMaterialManager::CreateInfo materialInfo;
	materialInfo.shader = "SimpleLightPS.hlsl";
	Utilz::GUID material = Utilz::GUID("MCModell.mat");
	materialInfo.materialFile = material;
	CoreInit::managers.materialManager->Create(player->GetEntity(), materialInfo);

	CoreInit::managers.renderableManager->ToggleRenderableObject(player->GetEntity(), true);
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

	player->UpdateMovement(input->GetDelta() * 3.0f, movementInput);
	player->UpdateActions(input->GetDelta(), newProjectiles, actionInput);

	UpdateProjectiles(newProjectiles);

	ProfileReturn(returnValue);

}