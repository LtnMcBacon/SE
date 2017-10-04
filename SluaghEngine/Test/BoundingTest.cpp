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
	Core::CameraBindInfoStruct cInfo;
	cInfo.aspectRatio = (float)om.GetOption("Window", "width", 800) / (float)om.GetOption("Window", "height", 640);
	cm.Bind(camera, cInfo);

	auto& block1 = em.Create();



	e.Release();
	return true;
}
