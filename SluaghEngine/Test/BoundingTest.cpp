#include "BoundingTest.h"
#include <Core\Engine.h>
//#include <Core\TransformManager.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

SE::Test::BoundingTest::BoundingTest()
{
}


SE::Test::BoundingTest::~BoundingTest()
{
}


static bool test = false;
static bool test2 = false;
void Collide2(const SE::Core::Entity& hit, const SE::Core::Entity& hitter)
{
	test2 = true;
}

void Collide1(const SE::Core::Entity& hit, const SE::Core::Entity& hitter)
{
	if (!test)
	{
		test = true;
		auto& e = SE::Core::Engine::GetInstance();
		auto& em = e.GetEntityManager();
		auto& col = e.GetCollisionManager();
		auto& tm = e.GetTransformManager();
		auto& cm = e.GetCameraManager();
		auto& drm = e.GetDebugRenderManager();
		auto& om = e.GetOptionHandler();
		auto& camera = em.Create();
		auto w = e.GetWindow();
		auto& rm = e.GetRenderableManager();

		col.RegisterCollideWithAnyCallback(&Collide2);

		auto& block2 = em.Create();
		col.CreateBoundingHierarchy(block2, "MCModell.mesh");
		rm.CreateRenderableObject(block2, "MCModell.mesh");
		rm.ToggleRenderableObject(block2, true);
		tm.SetPosition(block2, DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));

		tm.SetPosition(hit, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

		em.Destroy(hitter);
	}
		
}

enum ActionButton
{
	Exit,
	Hide,
	Show,
	Up,
	Down,
	Left,
	Right,
	Fullscreen,
	Rise,
	Sink
};


bool SE::Test::BoundingTest::Run(Utilz::IConsoleBackend * console)
{
	auto& e = Core::Engine::GetInstance();
	e.Init(Core::Engine::InitializationInfo());
	auto& em = e.GetEntityManager();
	auto& col = e.GetCollisionManager();
	auto& tm = e.GetTransformManager();
	auto& cm = e.GetCameraManager();
	auto& drm = e.GetDebugRenderManager();
	auto& om = e.GetOptionHandler();
	auto camera = em.Create();
	auto w = e.GetWindow();
	auto& rm = e.GetRenderableManager();

	col.RegisterCollideWithAnyCallback(&Collide1);

	Core::CameraBindInfoStruct cInfo;
	cInfo.aspectRatio = (float)om.GetOptionUnsignedInt("Window", "width", 800) / (float)om.GetOptionUnsignedInt("Window", "height", 640);
	cInfo.posistion = { 0.0f, 0.0f, -2.0f };
	cm.Bind(camera, cInfo);
	cm.SetActive(camera);

	auto block1 = em.Create();
	col.CreateBoundingHierarchy(block1, "MCModell.mesh");
	rm.CreateRenderableObject(block1, "MCModell.mesh");
	rm.ToggleRenderableObject(block1, true);
	tm.SetPosition(block1, DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));

	auto block2 = em.Create();
	col.CreateBoundingHierarchy(block2, "MCModell.mesh");	
	rm.CreateRenderableObject(block2, "MCModell.mesh");
	rm.ToggleRenderableObject(block2, true);
	tm.SetPosition(block2, DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));

	col.BindOnCollideWithAny(block1);
	
	drm.ToggleDebugRendering(block1, true);
	drm.ToggleDebugRendering(block2, true);

	e.GetWindow()->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	e.GetWindow()->MapActionButton(ActionButton::Left, Window::KeyD);
	test = false;
	test2 = false;
	bool su = false;
	for(int i = 0; i < 500; i++)
	{
		if (w->ButtonPressed(ActionButton::Exit))
			break;

		if(!test)
			tm.Move(block1, DirectX::XMFLOAT3(0.01f, 0.0f, 0.0f));
		else if(test)
		{
			if(!test2)
				tm.Move(block1, DirectX::XMFLOAT3(-0.01f, 0.0f, 0.0f));
		}
		
		e.Frame(0.01f);
		
		drm.ToggleDebugRendering(block1, true);
		drm.ToggleDebugRendering(block2, true);
		if (test && test2)
		{
			su = true;
		}
			
	}

	e.Release();
	return su;
}
