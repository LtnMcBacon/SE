#include "DebugRenderTest.h"
#include <Core\Engine.h>

SE::Test::DebugRenderManagerTest::DebugRenderManagerTest()
{
}

SE::Test::DebugRenderManagerTest::~DebugRenderManagerTest()
{
}

bool SE::Test::DebugRenderManagerTest::Run(Utilz::IConsoleBackend * console)
{
	auto& engine = Core::Engine::GetInstance();
	engine.Init(Core::Engine::InitializationInfo());
	auto& em = engine.GetEntityManager();
	auto& mm = engine.GetMaterialManager();
	Core::Entity entity = em.Create();

	Core::MaterialManager::CreateInfo info;
	Utilz::GUID textures[] = { Utilz::GUID("dummytex.sei"), Utilz::GUID("purewhite.sei") };
	Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex"), Utilz::GUID("diffuseTexSec") };
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.shaderResourceNames = resourceNames;
	info.textureFileNames = textures;
	info.textureCount = 2;



	mm.Create(entity, info);

	auto& rm = engine.GetRenderableManager();
	auto& tm = engine.GetTransformManager();
	auto& drm = engine.GetDebugRenderManager();

	tm.Create(entity);

	rm.CreateRenderableObject(entity, Utilz::GUID("Placeholder_level.obj"));
	rm.ToggleRenderableObject(entity, true);

	engine.GetWindow()->MapActionButton(0, Window::KeyEscape);
	engine.GetWindow()->MapActionButton(1, Window::KeyO);
	engine.GetWindow()->MapActionButton(2, Window::KeyK);

	//drm.DrawCross(entity, 50.0f);
	for (int i = 0; i < 100; i++)
	{
		drm.DrawCross(entity, 5.0f, i - 50, 10, 10);
	}
	//drm.DrawLine(entity, 0, 0, 0, 0, 100, 100);
	//drm.DrawLine(entity, 0, 0, 0, -100, -100, -100);
	//drm.DrawLine(entity, rand() % 200 - 100, rand() % 200 - 100, rand() % 200 - 100, rand() % 200 - 100, rand() % 200 - 100, rand() % 200 - 100);
	
	
	
	bool running = true;
	while (running)
	{
		if (engine.GetWindow()->ButtonPressed(0))
			running = false;

		if (engine.GetWindow()->ButtonPressed(1))
			rm.ToggleRenderableObject(entity, false);
		if (engine.GetWindow()->ButtonPressed(2))
			rm.ToggleRenderableObject(entity, true);

		engine.Frame(0.01f);
	}


	engine.Release();

	return true;
}
