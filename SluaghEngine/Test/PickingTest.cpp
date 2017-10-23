#include "PickingTest.h"
#include <Core\IEngine.h>
#include <Utilz\Tools.h>
#include <Windows.h>

SE::Test::PickingTest::PickingTest()
{
}

SE::Test::PickingTest::~PickingTest()
{
}
enum
{
	Exit,
	Click
};

using namespace DirectX;

bool SE::Test::PickingTest::Run(DevConsole::IConsole * console)
{
	auto engine = Core::CreateEngine();
	engine->Init();
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	

	float width = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800);
	float height = subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 600);

	auto floor = managers.entityManager->Create();
	auto object = managers.entityManager->Create();

	auto camera = managers.entityManager->Create();
	Core::ICameraManager::CreateInfo cInfo;
	cInfo.aspectRatio = (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800)/(float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 640);
	managers.cameraManager->Create(camera, cInfo);
	managers.cameraManager->SetActive(camera);

	float cameraRotationX = DirectX::XM_PI / 3;
	float cameraRotationY = DirectX::XM_PI / 3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);


	managers.transformManager->BindChild(floor, camera);
	managers.transformManager->Move(camera, -5 * cameraTranslation);
	managers.transformManager->SetRotation(camera, cameraRotationX, cameraRotationY, 0);

	managers.transformManager->Create(floor);
	managers.transformManager->SetPosition(floor, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	managers.renderableManager->CreateRenderableObject(floor, {"Placeholder_Block.mesh"});
	managers.renderableManager->ToggleRenderableObject(floor, true);

	managers.collisionManager->CreateBoundingHierarchy(floor, Utilz::GUID("Placeholder_Block.mesh"));
	
	DirectX::XMFLOAT3 pos = managers.transformManager->GetPosition(floor);
	DirectX::XMFLOAT3 rot = managers.transformManager->GetRotation(floor);
	DirectX::XMFLOAT3 scale = managers.transformManager->GetScale(floor);

	DirectX::XMMATRIX worldM = { scale.x, 0, 0, 0,
								0, scale.y, 0, 0,
								0, 0, scale.z, 0,
								pos.x, pos.y, pos.z, 1.0f };
	subSystem.window->MapActionButton(Exit, Window::KeyEscape);
	subSystem.window->MapActionButton(Click, Window::MouseLeft);
	bool running = true;
	while (running)
	{
		if (subSystem.window->ButtonPressed(Exit))
			running = false;
		if (subSystem.window->ButtonPressed(Click))
		{
			int x, y;
			subSystem.window->GetMousePos(x, y);
			{
				DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
				DirectX::XMVECTOR rayD;
				Utilz::Tools::RayToView(x, y, width, height, rayD);
				DirectX::XMFLOAT4X4 tempView = managers.cameraManager->GetViewInv(camera);
				DirectX::XMMATRIX viewM = DirectX::XMLoadFloat4x4(&tempView);

				rayO = DirectX::XMVector4Transform(rayO, viewM);
				rayD = DirectX::XMVector4Transform(rayD, viewM);
				rayD = XMVector3Normalize(rayD);

				float distance = 0.0f;
				Core::Entity ent;
				bool test = managers.collisionManager->Pick(rayO, rayD,ent, distance);

				if (test)
				{
					managers.renderableManager->ToggleRenderableObject(floor, false);
					running = false;
				}
				
			}
		}
		engine->BeginFrame();
		engine->EndFrame();
	}

	engine->Release(); delete engine;
	return true;
}
