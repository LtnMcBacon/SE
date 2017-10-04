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
void Collide1(const SE::Core::Entity& hit, const SE::Core::Entity& hitter)
{
	test = true;
}
void Collide2(const SE::Core::Entity& hit, const SE::Core::Entity& hitter)
{
	test = false;
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
	auto& camera = em.Create();
	auto w = e.GetWindow();
	auto& rm = e.GetRenderableManager();


	Core::CameraBindInfoStruct cInfo;
	cInfo.aspectRatio = (float)om.GetOption("Window", "width", 800) / (float)om.GetOption("Window", "height", 640);
	cInfo.posistion = { 0.0f, 0.0f, -1.0f };
	cm.Bind(camera, cInfo);

	auto& block1 = em.Create();
	col.CreateBoundingHierarchy(block1, "MCModell.mesh");
	rm.CreateRenderableObject(block1, "MCModell.mesh");
	rm.ToggleRenderableObject(block1, true);
	//drm.ToggleDebugRendering(block1, true);

	e.GetWindow()->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	bool running = true;

	while (running)
	{
		if (w->ButtonPressed(ActionButton::Exit))
			running = false;
		e.Frame(0.01f);
	}

	e.Release();
	return true;
}
