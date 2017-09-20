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
	auto& texture = Utilz::GUID("dummytex.jpg");
	auto& shader = Utilz::GUID("SimplePS.hlsl");
	info.textures = &texture;
	info.textureCount = 1;
	info.shader = &shader;
	info.shaderCount = 1;
	mm.Create(entity, info);



	engine.Release();

	return true;
}
