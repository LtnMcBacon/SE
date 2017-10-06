#include "GarbageTest.h"
#include <Core\Engine.h>
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

bool SE::Test::GarbageTest::Run(SE::Utilz::IConsoleBackend* console)
{
	StartProfile;
	using namespace DirectX;
	auto& e = Core::Engine::GetInstance();
	auto& info = Core::Engine::InitializationInfo();
	auto re = e.Init(info);
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		ProfileReturnConst(false)
	}

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();
	auto& om = e.GetOptionHandler();
	auto& caM = e.GetCameraManager();
	auto& coM = e.GetCollisionManager();
	auto& cm = e.GetCameraManager();

	const int entSize = 10000;

	SE::Core::Entity ents[entSize];
	auto name = Utilz::GUID("Placeholder_Block.mesh");

	for (int i = 0; i < entSize; i++)
	{
		ents[i] = em.Create();
		tm.Create(ents[i], { float(i % 100), 0.0f, float(i / 100) }, { 0,0,0 }, { 0.5f, 0.5f, 0.5f });
		rm.CreateRenderableObject(ents[i], name);
		rm.ToggleRenderableObject(ents[i], true);
	}

	//for (int i = 0; i < entSize; i++)
	//{
	//	ents[i] = em.Create();
	//	//mm.Create(ents[i], info);
	//	tm.Create(ents[i], { (float)(i*3.0f),0.0f,(float)((i * 3) % 2) }, { 0.0f,0.0f,0.0f }, { 5.02f,5.02f,5.02f });
	//	tm.Create(ents[i]);
	//	rm.CreateRenderableObject(ents[i], Utilz::GUID("Placeholder_Block.mesh"));
	//	rm.ToggleRenderableObject(ents[i], true);

	//}

	Tools::Tools t;

	float width = om.GetOption("Window", "width", 800);
	float height = om.GetOption("Window", "height", 600);

	//SE::Core::Entity camera = SE::Core::Engine::GetInstance().GetEntityManager().Create();

	//Core::CameraBindInfoStruct cInfo;
	//cInfo.aspectRatio = (float)om.GetOption("Window", "width", 800) / (float)om.GetOption("Window", "height", 640);

	//SE::Core::Engine::GetInstance().GetCameraManager().Bind(camera, cInfo);
	//SE::Core::Engine::GetInstance().GetCameraManager().SetActive(camera);

	//tm.SetPosition(camera, { 0, 10, 0 });
	//tm.SetRotation(camera, 3.14 / 2, 0, 0);

	//float cameraRotationX = DirectX::XM_PI / 3;
	//float cameraRotationY = DirectX::XM_PI / 3;

	//auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0);

	//auto cameraTranslation = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), cameraRotationMatrix);

	//SE::Core::Engine::GetInstance().GetTransformManager().Move(camera, -5 * cameraTranslation);
	//SE::Core::Engine::GetInstance().GetTransformManager().SetRotation(camera, cameraRotationX, cameraRotationY, 0);//2 * DirectX::XM_PI / 3, 0);

	const auto camera = em.Create();
	cm.Bind(camera);
	cm.SetActive(camera);
	tm.SetRotation(camera, 0.9f, 0.0f, 0.0f);
	tm.SetPosition(camera, { 50.0f, 30.0f, 50.0f });

	e.GetWindow()->MapActionButton(0, Window::KeyEscape);
	e.GetWindow()->MapActionButton(1, Window::Key1);
	e.GetWindow()->MapActionButton(2, Window::Key2);

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


	e.GetWindow()->MapActionButton(UP, Window::KeyUp);
	e.GetWindow()->MapActionButton(RIGHT, Window::KeyRight);
	e.GetWindow()->MapActionButton(DOWN, Window::KeyDown);
	e.GetWindow()->MapActionButton(LEFT, Window::KeyLeft);
	e.GetWindow()->MapActionButton(RIGHT_MOUSE, Window::MouseRight);
	e.GetWindow()->MapActionButton(SPACE, Window::KeySpace);
	e.GetWindow()->MapActionButton(ENTER, Window::KeyReturn);
	e.GetWindow()->MapActionButton(M, Window::KeyM);

	bool stepping = false;
	bool running = true;
	float dt = 1 / 60.0f;

	while (running)
	{

		if (e.GetWindow()->ButtonPressed(SPACE))
		{
			console->Print("Eliminate the heretics!\n");

			for (int i = 0; i < entSize; i++)
			{
				em.Destroy(ents[i]);
			}
		}

		if (e.GetWindow()->ButtonPressed(ENTER))
		{
			console->Print("Breed new soldiers!\n");

			for (int i = 0; i < entSize; i++)
			{
				ents[i] = em.Create();
				tm.Create(ents[i], { float(i % 100), 0.0f, float(i / 100) }, { 0,0,0 }, { 0.5f, 0.5f, 0.5f });
				rm.CreateRenderableObject(ents[i], name);
				rm.ToggleRenderableObject(ents[i], true);
			}
		}

		if (e.GetWindow()->ButtonPressed(M))
		{
			console->Print("Mutating new soldiers!\n");

			for (int i = 0; i < entSize; i++)
			{
				em.Destroy(ents[i]);
				ents[i] = em.Create();
				tm.Create(ents[i], { float(i % 100), 0.0f, float(i / 100) + 100.0f }, { 0,0,0 }, { 0.5f, 0.5f, 0.5f });
				rm.CreateRenderableObject(ents[i], name);
				rm.ToggleRenderableObject(ents[i], true);
			}

			console->Print("Mutation complete!\n");
		}

		if (e.GetWindow()->ButtonPressed(0))
			running = false;
		if (e.GetWindow()->ButtonPressed(1))
		{
			stepping = !stepping;
		}

		if (stepping)
		{
			if (e.GetWindow()->ButtonDown(2))
			{
				
			}
		}
		else {
			
		}
		e.Frame(dt);
	}

	e.Release();

	ProfileReturnConst(true)
}

