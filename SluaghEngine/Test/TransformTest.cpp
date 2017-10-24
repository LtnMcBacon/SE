#include "TransformTest.h"
#include <Core/IEngine.h>
#include <Utilz/Timer.h>

using namespace SE;
using namespace Core;

SE::Test::TransformTest::TransformTest()
{
}

SE::Test::TransformTest::~TransformTest()
{
}

bool SE::Test::TransformTest::Run(DevConsole::IConsole* console)
{
	auto engine = Core::CreateEngine();
	engine->Init();
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();

	auto Block = Utilz::GUID("Placeholder_Block.mesh");

	auto camera = managers.entityManager->Create();
	Core::ICameraManager::CreateInfo cbis;
	cbis.aspectRatio = (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800) / subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 640);
	managers.cameraManager->Create(camera, cbis);
	managers.cameraManager->SetActive(camera);
	enum KeyBindings
	{
		FORWARD,
		BACK,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		ROTYAW,
		NROTYAW,
		ROTPITCH,
		NROTPITCH,
		QUIT
	};
	subSystem.window->MapActionButton(QUIT, Window::KeyEscape);
	subSystem.window->MapActionButton(DOWN, Window::KeyCtrlR);
	subSystem.window->MapActionButton(UP, Window::KeyShiftL);
	subSystem.window->MapActionButton(FORWARD, Window::KeyW);
	subSystem.window->MapActionButton(BACK, Window::KeyS);
	subSystem.window->MapActionButton(LEFT, Window::KeyA);
	subSystem.window->MapActionButton(RIGHT, Window::KeyD);
	subSystem.window->MapActionButton(ROTYAW, Window::KeyRight);
	subSystem.window->MapActionButton(NROTYAW, Window::KeyLeft);
	subSystem.window->MapActionButton(ROTPITCH, Window::KeyUp);
	subSystem.window->MapActionButton(NROTPITCH, Window::KeyDown);

	auto grandFather = managers.entityManager->Create();
	managers.transformManager->Create(grandFather);
	managers.renderableManager->CreateRenderableObject(grandFather, { Block });
	managers.renderableManager->ToggleRenderableObject(grandFather, true);

	Entity seconds[10];
	for (int i = 0; i < 10; i++)
	{
		seconds[i] = managers.entityManager->Create();
		managers.transformManager->Create(seconds[i], { (float)i, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });
		managers.transformManager->BindChild(grandFather, seconds[i], true, true);
		managers.renderableManager->CreateRenderableObject(seconds[i], { Block });
		managers.renderableManager->ToggleRenderableObject(seconds[i], true);
	}

	Entity thirds[10];
	for (int i = 0; i < 10; i++)
	{
		thirds[i] = managers.entityManager->Create();
		managers.transformManager->Create(thirds[i], { (float)i, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.25f, 0.25f, 0.25f });
		managers.transformManager->BindChild(seconds[i], thirds[i], true, false);
		managers.renderableManager->CreateRenderableObject(thirds[i], { Block });
		managers.renderableManager->ToggleRenderableObject(thirds[i], true);
	}

	float speed = 1.0f;
	bool run = true;
	managers.entityManager->Destroy(seconds[4]);
	managers.entityManager->Destroy(seconds[7]);
	subSystem.window->UpdateTime();
	while(run)
	{
		subSystem.window->UpdateTime();
		float dt = subSystem.window->GetDelta();
		if (subSystem.window->ButtonPressed(QUIT))
			run = false;
		if (subSystem.window->ButtonDown(FORWARD))
		{
			DirectX::XMFLOAT3 forward = managers.transformManager->GetForward(camera);
			forward.x *= speed * dt;
			forward.y *= speed * dt;
			forward.z *= speed * dt;
			managers.transformManager->Move(camera, forward);
		}
		if (subSystem.window->ButtonDown(BACK))
		{
			DirectX::XMFLOAT3 forward = managers.transformManager->GetForward(camera);
			forward.x *= -speed * dt;
			forward.y *= -speed * dt;
			forward.z *= -speed * dt;
			managers.transformManager->Move(camera, forward);
		}
		if (subSystem.window->ButtonDown(RIGHT))
		{
			DirectX::XMFLOAT3 right = managers.transformManager->GetRight(camera);
			right.x *= speed * dt;
			right.y *= speed * dt;
			right.z *= speed * dt;
			managers.transformManager->Move(camera, right);
		}
		if (subSystem.window->ButtonDown(LEFT))
		{
			DirectX::XMFLOAT3 right = managers.transformManager->GetRight(camera);
			right.x *= -speed * dt;
			right.y *= -speed * dt;
			right.z *= -speed * dt;
			managers.transformManager->Move(camera, right);
		}
		if (subSystem.window->ButtonDown(ROTYAW))
		{
			managers.transformManager->Rotate(camera, 0.0f, 6.28f * dt, 0.0f);
		}
		if (subSystem.window->ButtonDown(NROTYAW))
		{
			managers.transformManager->Rotate(camera, 0.0f, -6.28f * dt, 0.0f);
		}
		if (subSystem.window->ButtonDown(ROTPITCH))
		{
			managers.transformManager->Rotate(camera, 0.25f * -6.28f * dt, 0.0f, 0.0f);
		}
		if (subSystem.window->ButtonDown(NROTPITCH))
		{
			managers.transformManager->Rotate(camera, 0.25f *6.28f * dt, 0.0f, 0.0f);
		}

		managers.transformManager->Rotate(grandFather, dt * 3.14f*0.25f, 0.0f, 0.0f);
		for (int i = 0; i < 10; i++)
		{
			if(managers.entityManager->Alive(seconds[i]))
				managers.transformManager->Rotate(seconds[i], dt * 3.14f * 0.5f, 0.0f, 0.0f);
		}
		for(int i = 0; i < 10; i++)
		{
			if(managers.entityManager->Alive(thirds[i]))
				managers.transformManager->Rotate(thirds[i], 0.0f, dt*3.14f * 0.75f, 0.0f);
		}
		
		engine->BeginFrame();
		engine->EndFrame();
	}
	engine->Release(); delete engine;
	return true;
}
