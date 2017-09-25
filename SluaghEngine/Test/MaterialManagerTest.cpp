#include "MaterialManagerTest.h"
#include <Core\Engine.h>


SE::Test::MaterialManagerTest::MaterialManagerTest()
{
}


SE::Test::MaterialManagerTest::~MaterialManagerTest()
{
}

bool SE::Test::MaterialManagerTest::Run(Utilz::IConsoleBackend * console)
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

	tm.Create(entity);

	rm.CreateRenderableObject(entity, Utilz::GUID("Placeholder_levsel.obj"));
	rm.ToggleRenderableObject(entity, true);

	engine.GetWindow()->MapActionButton(0, Window::KeyEscape);
	engine.GetWindow()->MapActionButton(1, Window::KeyO);
	engine.GetWindow()->MapActionButton(2, Window::KeyK);

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
