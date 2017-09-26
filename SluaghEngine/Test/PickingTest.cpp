#include "PickingTest.h"
#include <Core\Engine.h>
#include <Core\CollisionManager.h>
#include <Windows.h>

bool SE::Test::PickingTest::Run(SE::Utilz::IConsoleBackend * console)
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

	auto floor = em.Create();
	auto object = em.Create();

	rm.CreateRenderableObject(floor, Utilz::GUID("Placeholder_Floor.obj"));
	rm.ToggleRenderableObject(floor, true);

	cm.CreateBoundingHierarchy(floor, Utilz::GUID("Placeholder_Floor.obj"));

	DirectX::XMFLOAT3 pos = tm.GetPosition(floor);
	DirectX::XMFLOAT3 rot = tm.GetRotation(floor);
	DirectX::XMFLOAT3 scale = tm.GetScale(floor);

	DirectX::XMMATRIX worldM = { scale.x, 0, 0, 0,
								0, scale.y, 0, 0,
								0, 0, scale.z, 0,
								pos.x, pos.y, pos.z, 1.0f };

	//while (true)
	//{
	//	if (GetAsyncKeyState(0x01))
	//	{
	//		POINT p;
	//		if (GetCursorPos(&p))
	//		{
	//			DirectX::XMVECTOR ray = { p.x, p.y, 1.0f};
	//			
	//		}
	//	}
	//}


	return false;
}
