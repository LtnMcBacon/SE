#include "GarbageTest.h"
#include <Core\IEngine.h>
#include <Gameplay/EnemyUnit.h>
#include <Gameplay/Room.h>
#include <Gameplay/ProjectileData.h>
#include <Gameplay/ProjectileManager.h>
#include <chrono>
#include <Gameplay/PlayerUnit.h>
#include <Profiler.h>
#include <Utilz\Tools.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib, "gameplayD.lib")
#else
#pragma comment(lib, "gameplay.lib")
#endif



SE::Test::GarbageTest::GarbageTest()
{

}

SE::Test::GarbageTest::~GarbageTest()
{

}

bool SE::Test::GarbageTest::MassiveTest(SE::DevConsole::IConsole* console)
{
	StartProfile;
	using namespace DirectX;
	auto engine = Core::CreateEngine();
	engine->Init();
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();

	const int entSize = 10000;

	SE::Core::Entity ents[entSize];
	auto name = Utilz::GUID("Placeholder_Block.mesh");

	for (int i = 0; i < entSize; i++)
	{
		ents[i] = managers.entityManager->Create();
		managers.transformManager->Create(ents[i], { float(i % 100), 0.0f, float(i / 100) }, { 0,0,0 }, { 0.5f, 0.5f, 0.5f });
		managers.renderableManager->CreateRenderableObject(ents[i], { name });
		managers.renderableManager->ToggleRenderableObject(ents[i], true);
	}

	//for (int i = 0; i < entSize; i++)
	//{
	//	ents[i] = managers.entityManager->Create();
	//	//managers.materialManager->Create(ents[i], info);
	//	managers.transformManager->Create(ents[i], { (float)(i*3.0f),0.0f,(float)((i * 3) % 2) }, { 0.0f,0.0f,0.0f }, { 5.02f,5.02f,5.02f });
	//	managers.transformManager->Create(ents[i]);
	//	managers.renderableManager->CreateRenderableObject(ents[i], Utilz::GUID("Placeholder_Block.mesh"));
	//	managers.renderableManager->ToggleRenderableObject(ents[i], true);

	//}


	//SE::Core::Entity camera = managers.entityManager->Create();

	//Core::CameraBindInfoStruct cInfo;
	//cInfo.aspectRatio = (float)subSystem.optionsHandler->GetOption("Window", "width", 800) / (float)subSystem.optionsHandler->GetOption("Window", "height", 640);

	//managers.cameraManager->Bind(camera, cInfo);
	//managers.cameraManager->SetActive(camera);

	//managers.transformManager->SetPosition(camera, { 0, 10, 0 });
	//managers.transformManager->SetRotation(camera, 3.14 / 2, 0, 0);

	//float cameraRotationX = DirectX::XM_PI / 3;
	//float cameraRotationY = DirectX::XM_PI / 3;

	//auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	//auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	//managers.transformManager->Move(camera, -5 * cameraTranslation);
	//managers.transformManager->SetRotation(camera, cameraRotationX, cameraRotationY, 0);//2 * DirectX::XM_PI / 3, 0);

	const auto camera = managers.entityManager->Create();
	managers.cameraManager->Create(camera);
	managers.cameraManager->SetActive(camera);
	managers.transformManager->SetRotation(camera, 0.9f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(camera, { 50.0f, 30.0f, 50.0f });

	subSystem.window->MapActionButton(0, Window::KeyEscape);
	subSystem.window->MapActionButton(1, Window::Key1);
	subSystem.window->MapActionButton(2, Window::Key2);

	enum MoveDir
	{
		UP = 3,
		RIGHT = 4,
		DOWN = 5,
		LEFT = 6,
		RIGHT_MOUSE = 7,
		SPACE = 8,
		ENTER = 9,
		M = 10
	};


	subSystem.window->MapActionButton(UP, Window::KeyUp);
	subSystem.window->MapActionButton(RIGHT, Window::KeyRight);
	subSystem.window->MapActionButton(DOWN, Window::KeyDown);
	subSystem.window->MapActionButton(LEFT, Window::KeyLeft);
	subSystem.window->MapActionButton(RIGHT_MOUSE, Window::MouseRight);
	subSystem.window->MapActionButton(SPACE, Window::KeySpace);
	subSystem.window->MapActionButton(ENTER, Window::KeyReturn);
	subSystem.window->MapActionButton(M, Window::KeyM);

	bool stepping = false;
	bool running = true;
	float dt = 1 / 60.0f;

	while (running)
	{

		if (subSystem.window->ButtonDown(RIGHT))
		{
			for (int i = 0; i < entSize; i++)
			{
				managers.transformManager->Move(ents[i], { 100 * dt, 0.0f, 0.0f, 0.0f });
			}
		}

		if (subSystem.window->ButtonDown(LEFT))
		{
			for (int i = 0; i < entSize; i++)
			{
				managers.transformManager->Move(ents[i], { 100 * -dt, 0.0f, 0.0f, 0.0f });
			}
		}


		if (subSystem.window->ButtonPressed(SPACE))
		{
			console->Print("Eliminate the heretics!\n");

			for (int i = 0; i < entSize; i++)
			{
				managers.entityManager->Destroy(ents[i]);
			}
		}

		if (subSystem.window->ButtonPressed(ENTER))
		{
			console->Print("Breed new soldiers!\n");

			for (int i = 0; i < entSize; i++)
			{
				ents[i] = managers.entityManager->Create();
				managers.transformManager->Create(ents[i], { float(i % 100), 0.0f, float(i / 100) }, { 0,0,0 }, { 0.5f, 0.5f, 0.5f });
				managers.renderableManager->CreateRenderableObject(ents[i], {name});
				managers.renderableManager->ToggleRenderableObject(ents[i], true);
			}
		}

		if (subSystem.window->ButtonPressed(M))
		{
			console->Print("Mutating new soldiers!\n");

			for (int i = 0; i < entSize; i++)
			{
				managers.entityManager->Destroy(ents[i]);
				ents[i] = managers.entityManager->Create();
				managers.transformManager->Create(ents[i], { float(i % 100), 0.0f, float(i / 100) + 100.0f }, { 0,0,0 }, { 0.5f, 0.5f, 0.5f });
				managers.renderableManager->CreateRenderableObject(ents[i], {name});
				managers.renderableManager->ToggleRenderableObject(ents[i], true);
			}

			console->Print("Mutation complete!\n");
		}

		if (subSystem.window->ButtonPressed(0))
			running = false;
		if (subSystem.window->ButtonPressed(1))
		{
			stepping = !stepping;
		}

		if (stepping)
		{
			if (subSystem.window->ButtonDown(2))
			{

			}
		}
		else {

		}
		engine->BeginFrame();
		engine->EndFrame();
	}

	engine->Release(); delete engine;

	ProfileReturnConst(true)
}

bool SE::Test::GarbageTest::ProjectilesIshTest(DevConsole::IConsole * console) const
{
	StartProfile;
	using namespace DirectX;
	auto engine = Core::CreateEngine();
	engine->Init();
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();

	struct projectileish
	{
		float x, y;
		float lifeTime;
		SE::Core::Entity ent;

		void update(float dt)
		{
			lifeTime -= dt;
		}

		bool IsActive() const
		{
			return lifeTime > 0.0f;
		}

		projectileish(float xVal = 0.0f, float yVal = 0.0f, float lifeTimeVal = 0.0f, Core::Entity entity = 0)
		{
			x = xVal;
			y = yVal;
			lifeTime = lifeTimeVal;
			ent = entity;
		}
	};

	auto name = Utilz::GUID("Placeholder_Block.mesh");
	std::vector<projectileish> projectiles;

	/*Core::Entity dummyEnt = managers.entityManager->Create();
	managers.transformManager->Create(dummyEnt);
	managers.renderableManager->CreateRenderableObject(dummyEnt, {name});
	managers.renderableManager->ToggleRenderableObject(dummyEnt, true);

	projectileish dummy(0.0f, 0.0f, -1, dummyEnt);*/

	//const int nrOfMaxProjectiles = 4;
	//projectileish ourProjectiles[nrOfMaxProjectiles];
	//int nrOfActiveProjectiles = 0;


	const auto camera = managers.entityManager->Create();
	managers.cameraManager->Create(camera);
	managers.cameraManager->SetActive(camera);
	managers.transformManager->SetRotation(camera, 0.9f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(camera, { 0.0f, 20.0f, -5.0f });

	subSystem.window->MapActionButton(0, Window::KeyEscape);
	subSystem.window->MapActionButton(1, Window::Key1);
	subSystem.window->MapActionButton(2, Window::Key2);

	enum MoveDir
	{
		UP = 3,
		RIGHT = 4,
		DOWN = 5,
		LEFT = 6,
		RIGHT_MOUSE = 7,
		SPACE = 8,
		ENTER = 9,
		M = 10
	};


	subSystem.window->MapActionButton(UP, Window::KeyUp);
	subSystem.window->MapActionButton(RIGHT, Window::KeyRight);
	subSystem.window->MapActionButton(DOWN, Window::KeyDown);
	subSystem.window->MapActionButton(LEFT, Window::KeyLeft);
	subSystem.window->MapActionButton(RIGHT_MOUSE, Window::MouseRight);
	subSystem.window->MapActionButton(SPACE, Window::KeySpace);
	subSystem.window->MapActionButton(ENTER, Window::KeyReturn);
	subSystem.window->MapActionButton(M, Window::KeyM);

	bool stepping = false;
	bool running = true;
	float dt = 1 / 60.0f;

	//std::vector<projectileish> chungus;

	//for (int i = 0; i < 5; i++)
	//{
	//	Core::Entity tempEnt = managers.entityManager->Create();
	//	managers.transformManager->Create(tempEnt, { float(i), 0, 0 }, { 0,0,0 }, { 0.5,0.5,0.5 });
	//	managers.renderableManager->CreateRenderableObject(tempEnt, {name});
	//	managers.renderableManager->ToggleRenderableObject(tempEnt, true);

	//	projectileish tempProj(0.0f, 0.0f, 3, tempEnt);
	//	chungus.push_back(tempProj);
	//}

	while (running)
	{


		if (subSystem.window->ButtonPressed(SPACE)/* && nrOfActiveProjectiles < nrOfMaxProjectiles*/)
		{
			Core::Entity tempEnt = managers.entityManager->Create();
			managers.transformManager->Create(tempEnt);
			managers.renderableManager->CreateRenderableObject(tempEnt, {name});
			managers.renderableManager->ToggleRenderableObject(tempEnt, true);

			projectileish tempProj(0.0f, 0.0f, 3, tempEnt);
			projectiles.push_back(tempProj);

		/*	Core::Entity tempEnt2 = managers.entityManager->Create();
			managers.transformManager->Create(tempEnt2);
			managers.renderableManager->CreateRenderableObject(tempEnt2, {name});
			managers.renderableManager->ToggleRenderableObject(tempEnt2, true);

			if (dummy.IsActive())
				managers.entityManager->Destroy(dummy.ent);

			dummy = projectileish(0.0f, 0.0f, 3, tempEnt2);*/

			//ourProjectiles[nrOfActiveProjectiles++] = projectileish(0.0f, 0.0f, 3, tempEnt);
		}

		//for (auto& c : chungus)
		//{
		//	if(managers.entityManager->Alive(c.ent))
		//		managers.transformManager->Move(c.ent, { 0,0,3.0f*dt,0.0f });
		//}
		//if (subSystem.window->ButtonPressed(ENTER)/* && nrOfActiveProjectiles < nrOfMaxProjectiles*/)
		//{
		//	managers.entityManager->Destroy(chungus[2].ent);
		//}
		

		for (int i = 0; i < projectiles.size(); i++)
		{
			if (projectiles[i].IsActive())
			{
				if(managers.entityManager->Alive(projectiles[i].ent))
					managers.transformManager->Move(projectiles[i].ent, { 0.0f, 0.0f, 5*dt, 0.0f});

				projectiles[i].update(dt);
			}
			else
			{
				managers.entityManager->Destroy(projectiles[i].ent);
				projectiles[i] = projectiles.back();
				projectiles.pop_back();
				i--;
			}
		}

		/*if (dummy.IsActive())
		{
			managers.transformManager->Move(dummy.ent, { 4 * dt, 0.0f, 0, 0.0f });
			dummy.update(dt);
		}
		else
		{
			if (managers.entityManager->Alive(dummy.ent))
				managers.entityManager->Destroy(dummy.ent);
		}*/

		//for (int i = 0; i < nrOfActiveProjectiles; i++)
		//{
		//	if (ourProjectiles[i].IsActive())
		//	{
		//		if (managers.entityManager->Alive(ourProjectiles[i].ent))
		//			managers.transformManager->Move(ourProjectiles[i].ent, { 0,0,5 * dt,0 });

		//		ourProjectiles[i].update(dt);
		//	}
		//	else
		//	{
		//		managers.entityManager->Destroy(ourProjectiles[i].ent);
		//		//nrOfActiveProjectiles--;
		//	}
		//}

		if (subSystem.window->ButtonPressed(0))
			running = false;
		if (subSystem.window->ButtonPressed(1))
		{
			stepping = !stepping;
		}

		if (stepping)
		{
			if (subSystem.window->ButtonDown(2))
			{

			}
		}
		else {

		}
		engine->BeginFrame();
		engine->EndFrame();
	}

	engine->Release(); delete engine;

	ProfileReturnConst(true)
}

bool SE::Test::GarbageTest::Run(SE::DevConsole::IConsole* console)
{
	return ProjectilesIshTest(console);
}

