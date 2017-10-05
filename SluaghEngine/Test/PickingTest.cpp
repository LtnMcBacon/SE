#include "PickingTest.h"
#include <Core\Engine.h>
#include <Core\CollisionManager.h>
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
bool SE::Test::PickingTest::Run(Utilz::IConsoleBackend * console)
{
	auto& e = Core::Engine::GetInstance();
	auto& info = Core::Engine::InitializationInfo();
	auto re = e.Init(info);
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		return false;
	}

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& cm = e.GetCollisionManager();
	auto& tm = e.GetTransformManager();
	auto& vm = e.GetCameraManager();
	auto& om = e.GetOptionHandler();
	Tools::Tools t;

	float width = om.GetOption("Window", "width", 800);
	float height = om.GetOption("Window", "height", 600);

	auto floor = em.Create();
	auto object = em.Create();

	auto camera = em.Create();
	Core::CameraBindInfoStruct cInfo;
	cInfo.aspectRatio = (float)om.GetOption("Window", "width", 800)/(float)om.GetOption("Window", "height", 640);
	vm.Bind(camera, cInfo);
	vm.SetActive(camera);

	float cameraRotationX = DirectX::XM_PI / 3;
	float cameraRotationY = DirectX::XM_PI / 3;

	auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);


	SE::Core::Engine::GetInstance().GetTransformManager().BindChild(floor, camera);
	SE::Core::Engine::GetInstance().GetTransformManager().Move(camera, -5 * cameraTranslation);
	SE::Core::Engine::GetInstance().GetTransformManager().SetRotation(camera, cameraRotationX, cameraRotationY, 0);

	tm.Create(floor);
	tm.SetPosition(floor, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	rm.CreateRenderableObject(floor, Utilz::GUID("Placeholder_Block.mesh"));
	rm.ToggleRenderableObject(floor, true);

	cm.CreateBoundingHierarchy(floor, Utilz::GUID("Placeholder_Block.mesh"));
	
	DirectX::XMFLOAT3 pos = tm.GetPosition(floor);
	DirectX::XMFLOAT3 rot = tm.GetRotation(floor);
	DirectX::XMFLOAT3 scale = tm.GetScale(floor);

	DirectX::XMMATRIX worldM = { scale.x, 0, 0, 0,
								0, scale.y, 0, 0,
								0, 0, scale.z, 0,
								pos.x, pos.y, pos.z, 1.0f };
	auto w = e.GetWindow();
	w->MapActionButton(Exit, Window::KeyEscape);
	w->MapActionButton(Click, Window::MouseLeft);
	bool running = true;
	while (running)
	{
		if (w->ButtonPressed(Exit))
			running = false;
		if (w->ButtonPressed(Click))
		{
			int x, y;
			w->GetMousePos(x, y);
			{
				DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
				DirectX::XMVECTOR rayD = t.rayToView(x, y, width, height);
				DirectX::XMFLOAT4X4 tempView = vm.GetViewInv(camera);
				DirectX::XMMATRIX viewM = DirectX::XMLoadFloat4x4(&tempView);

				rayO = DirectX::XMVector4Transform(rayO, viewM);
				rayD = DirectX::XMVector4Transform(rayD, viewM);
				rayD = XMVector3Normalize(rayD);

				float distance = 0.0f;
				bool test = cm.PickEntity(floor, rayO, rayD, &distance);

				if (test)
				{
					rm.ToggleRenderableObject(floor, false);
					running = false;
				}
				
			}
		}
		e.Frame(0.01f);
	}

	e.Release();
	return true;
}