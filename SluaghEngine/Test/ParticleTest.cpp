#include "ParticleTest.h"
#include <Core\IEngine.h>

bool SE::Test::ParticleTest::Run(SE::DevConsole::IConsole * console)
{
	auto engine = Core::CreateEngine();
	engine->Init();

	auto subSystems = engine->GetSubsystems();
	auto managers = engine->GetManagers();
	auto particleEnt = managers.entityManager->Create();
	Core::IParticleSystemManager::CreateInfo particleInfo;
	particleInfo.systemFile;
	managers.particleSystemManager->CreateSystem(particleEnt, { "testBall.txt" });
	Core::ICameraManager::CreateInfo camInfo;
	camInfo.posistion = { 0.0f, 0.0f, -5.0f };
	managers.cameraManager->UpdateCamera(camInfo);

	while (true)
	{
		engine->BeginFrame();



		engine->EndFrame();
	}

	engine->Release();
	delete engine;
	return false;
}

SE::Test::ParticleTest::ParticleTest()
{
}


SE::Test::ParticleTest::~ParticleTest()
{
}
