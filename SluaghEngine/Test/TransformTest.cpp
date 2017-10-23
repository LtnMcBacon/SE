#include "TransformTest.h"
#include <Core/Engine.h>
#include <Utilz/Timer.h>

using namespace SE;
using namespace Core;

SE::Test::TransformTest::TransformTest()
{
}

SE::Test::TransformTest::~TransformTest()
{
}

bool SE::Test::TransformTest::Run(Utilz::IConsoleBackend* console)
{
	auto& e = Engine::GetInstance();
	e.Init();
	auto& em = e.GetEntityManager();
	auto& tm = e.GetTransformManager();
	auto& rm = e.GetRenderableManager();
	auto& cm = e.GetCameraManager();
	auto& o = e.GetOptionHandler();
	auto window = e.GetWindow();

	auto Block = Utilz::GUID("Placeholder_Block.mesh");

	auto camera = em.Create();
	CameraBindInfoStruct cbis;
	cbis.aspectRatio = (float)o.GetOptionUnsignedInt("Window", "width", 800) / o.GetOptionUnsignedInt("Window", "height", 640);
	cm.Bind(camera, cbis);
	cm.SetActive(camera);
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
	window->MapActionButton(QUIT, Window::KeyEscape);
	window->MapActionButton(DOWN, Window::KeyCtrlR);
	window->MapActionButton(UP, Window::KeyShiftL);
	window->MapActionButton(FORWARD, Window::KeyW);
	window->MapActionButton(BACK, Window::KeyS);
	window->MapActionButton(LEFT, Window::KeyA);
	window->MapActionButton(RIGHT, Window::KeyD);
	window->MapActionButton(ROTYAW, Window::KeyRight);
	window->MapActionButton(NROTYAW, Window::KeyLeft);
	window->MapActionButton(ROTPITCH, Window::KeyUp);
	window->MapActionButton(NROTPITCH, Window::KeyDown);

	auto grandFather = em.Create();
	tm.Create(grandFather);
	rm.CreateRenderableObject(grandFather, Block);
	rm.ToggleRenderableObject(grandFather, true);

	Entity seconds[10];
	for (int i = 0; i < 10; i++)
	{
		seconds[i] = em.Create();
		tm.Create(seconds[i], { (float)i, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });
		tm.BindChild(grandFather, seconds[i], true, true);
		rm.CreateRenderableObject(seconds[i], Block);
		rm.ToggleRenderableObject(seconds[i], true);
	}

	Entity thirds[10];
	for (int i = 0; i < 10; i++)
	{
		thirds[i] = em.Create();
		tm.Create(thirds[i], { (float)i, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.25f, 0.25f, 0.25f });
		tm.BindChild(seconds[i], thirds[i], true, false);
		rm.CreateRenderableObject(thirds[i], Block);
		rm.ToggleRenderableObject(thirds[i], true);
	}

	float speed = 1.0f;
	bool run = true;
	em.Destroy(seconds[4]);
	em.Destroy(seconds[7]);
	window->UpdateTime();
	while(run)
	{
		window->UpdateTime();
		float dt = window->GetDelta();
		if (window->ButtonPressed(QUIT))
			run = false;
		if (window->ButtonDown(FORWARD))
		{
			DirectX::XMFLOAT3 forward = tm.GetForward(camera);
			forward.x *= speed * dt;
			forward.y *= speed * dt;
			forward.z *= speed * dt;
			tm.Move(camera, forward);
		}
		if (window->ButtonDown(BACK))
		{
			DirectX::XMFLOAT3 forward = tm.GetForward(camera);
			forward.x *= -speed * dt;
			forward.y *= -speed * dt;
			forward.z *= -speed * dt;
			tm.Move(camera, forward);
		}
		if (window->ButtonDown(RIGHT))
		{
			DirectX::XMFLOAT3 right = tm.GetRight(camera);
			right.x *= speed * dt;
			right.y *= speed * dt;
			right.z *= speed * dt;
			tm.Move(camera, right);
		}
		if (window->ButtonDown(LEFT))
		{
			DirectX::XMFLOAT3 right = tm.GetRight(camera);
			right.x *= -speed * dt;
			right.y *= -speed * dt;
			right.z *= -speed * dt;
			tm.Move(camera, right);
		}
		if (window->ButtonDown(ROTYAW))
		{
			tm.Rotate(camera, 0.0f, 6.28f * dt, 0.0f);
		}
		if (window->ButtonDown(NROTYAW))
		{
			tm.Rotate(camera, 0.0f, -6.28f * dt, 0.0f);
		}
		if (window->ButtonDown(ROTPITCH))
		{
			tm.Rotate(camera, 0.25f * -6.28f * dt, 0.0f, 0.0f);
		}
		if (window->ButtonDown(NROTPITCH))
		{
			tm.Rotate(camera, 0.25f *6.28f * dt, 0.0f, 0.0f);
		}

		tm.Rotate(grandFather, dt * 3.14f*0.25f, 0.0f, 0.0f);
		for (int i = 0; i < 10; i++)
		{
			if(em.Alive(seconds[i]))
				tm.Rotate(seconds[i], dt * 3.14f * 0.5f, 0.0f, 0.0f);
		}
		for(int i = 0; i < 10; i++)
		{
			if(em.Alive(thirds[i]))
				tm.Rotate(thirds[i], 0.0f, dt*3.14f * 0.75f, 0.0f);
		}
		
		e.Frame(dt);
	}
	e.Release();
	return true;
}
