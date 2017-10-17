#include"PlayState.h"
#include <Profiler.h>
#include <Utilz\Tools.h>
#include <Core\Engine.h>
#include <Core\TransformManager.h>
#include <Core\CameraManager.h>
#include <Core\CollisionManager.h>
#include <Core\DebugRenderManager.h>
#include <Core\EntityManager.h>
#include <Core\RenderableManager.h>


using namespace SE;
using namespace Gameplay;

PlayState::PlayState()
{

}

PlayState::PlayState(Window::IWindow* Input)
{
	this->input = Input;

	projectileManager = new ProjectileManager();

	InitializeRooms();
	InitializePlayer();
	InitializeOther();
}

PlayState::~PlayState()
{

}

void PlayState::UpdateInput(PlayerUnit::MovementInput &movement, PlayerUnit::ActionInput &action)
{
	Tools::Tools tools;
	auto& cManager = Core::Engine::GetInstance().GetCameraManager();
	auto& oHandler = Core::Engine::GetInstance().GetOptionHandler();

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
	DirectX::XMVECTOR rayD = tools.rayToView(mX, mY, oHandler.GetOptionUnsignedInt("Window", "width", 800), oHandler.GetOptionUnsignedInt("Window", "height", 600));
	DirectX::XMFLOAT4X4 tempView = cManager.GetViewInv(cam);
	DirectX::XMMATRIX viewM = DirectX::XMLoadFloat4x4(&tempView);

	rayO = DirectX::XMVector4Transform(rayO, viewM);
	rayD = DirectX::XMVector4Transform(rayD, viewM);
	rayD = XMVector3Normalize(rayD);

	float distance = XMVectorGetY(rayO) / -XMVectorGetY(rayD);

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

}

void SE::Gameplay::PlayState::UpdateProjectiles(std::vector<ProjectileData>& newProjectiles)
{
	projectileManager->AddProjectiles(newProjectiles);

	projectileManager->UpdateProjectilePositions(1 / 60.0f);
	currentRoom->CheckProjectileCollision(projectileManager->GetAllProjectiles());
	projectileManager->UpdateProjectileActions(1 / 60.0f);
}

void PlayState::InitializeRooms()
{
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

	Room room(mapRepresentation);
	rooms.push_back(room);
	currentRoom = &rooms.back();

}
void PlayState::InitializePlayer()
{
	auto& tManager = Core::Engine::GetInstance().GetTransformManager();
	auto& rManager = Core::Engine::GetInstance().GetRenderableManager();
	auto& oHandler = Core::Engine::GetInstance().GetOptionHandler();
	auto& cManager = Core::Engine::GetInstance().GetCameraManager();

	char map[25][25];
	currentRoom->GetMap(map);
	player = new PlayerUnit(nullptr, nullptr, 1.5f, 1.5f, map);
	tManager.SetPosition(player->GetEntity(), DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));

	tManager.SetScale(player->GetEntity(), 1.f);
	rManager.CreateRenderableObject(player->GetEntity(), Utilz::GUID("MCModell.mesh"));

	Core::MaterialManager::CreateInfo materialInfo;
	materialInfo.shader = "SimpleLightPS.hlsl";
	Utilz::GUID shaderResources[1];
	shaderResources[0] = "diffuseTex";
	materialInfo.shaderResourceNames = shaderResources;
	Utilz::GUID textureNames[] = { "texture8.sei" };
	materialInfo.textureFileNames = textureNames;
	materialInfo.textureCount = 1;
	Core::Engine::GetInstance().GetMaterialManager().Create(player->GetEntity(), materialInfo);

	rManager.ToggleRenderableObject(player->GetEntity(), true);
}

void SE::Gameplay::PlayState::InitializeOther()
{
	auto& tManager = Core::Engine::GetInstance().GetTransformManager();
	auto& rManager = Core::Engine::GetInstance().GetRenderableManager();
	auto& oHandler = Core::Engine::GetInstance().GetOptionHandler();
	auto& cManager = Core::Engine::GetInstance().GetCameraManager();
	auto& lManager = Core::Engine::GetInstance().GetLightManager();

	//Setup camera to the correct perspective and bind it to the players position
	Core::CameraBindInfoStruct cInfo;
	cInfo.aspectRatio = (float)oHandler.GetOptionUnsignedInt("Window", "width", 800) / (float)oHandler.GetOptionUnsignedInt("Window", "height", 640);
	cam = cManager.GetActive();
	cManager.UpdateCamera(cam, cInfo);

	float cameraRotationX = DirectX::XM_PI / 3;
	float cameraRotationY = DirectX::XM_PI / 3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	player->UpdatePlayerRotation(cameraRotationX, cameraRotationY);
	tManager.BindChild(player->GetEntity(), cam, false);
	tManager.Move(cam, -5 * cameraTranslation);
	tManager.SetRotation(cam, cameraRotationX, cameraRotationY, 0);

	//Create a default light

	dummy = Core::Engine::GetInstance().GetEntityManager().Create();
	tManager.Create(dummy, { 12.5, 3, 12.5 });
	rManager.CreateRenderableObject(dummy, Utilz::GUID("Placeholder_Block.mesh"));
	rManager.ToggleRenderableObject(dummy, true);

	SE::Core::AddLightInfo lightInfo;
	lightInfo.pos = { 0.0f, 0.0f, 0.0f };
	lightInfo.color = { 1.0f, 1.0f, 1.0f };
	lightInfo.radius = 10000.0f;

	lManager.AddLight(dummy, lightInfo);
	lManager.ToggleLight(dummy, true);
}

IGameState::State PlayState::Update(void*& passableInfo)
{
	StartProfile;

	IGameState::State returnValue = State::PLAY_STATE;
	PlayerUnit::MovementInput movementInput(false, false, false, false, false, 0.0f, 0.0f);
	PlayerUnit::ActionInput actionInput(false, false);
	std::vector<ProjectileData> newProjectiles;

	UpdateInput(movementInput, actionInput);

	player->UpdateMovement(1 / 60.0f, movementInput);
	player->UpdateActions(1 / 60.0f, newProjectiles, actionInput);

	UpdateProjectiles(newProjectiles);

	ProfileReturn(returnValue);

}