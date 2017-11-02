#include "DecalTest.h"
#include <Core/IEngine.h>
#include <Imgui/imgui.h>
SE::Test::DecalTest::DecalTest()
{
}

SE::Test::DecalTest::~DecalTest()
{
}

bool SE::Test::DecalTest::Run(DevConsole::IConsole* console)
{
	auto engine = Core::CreateEngine();
	auto r = engine->Init();
	if (r < 0)
	{
		delete engine;
		return false;
	}

	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());

	auto em = managers.entityManager;
	auto rm = managers.renderableManager;
	auto cm = managers.cameraManager;
	auto dm = managers.decalManager;
	auto tm = managers.transformManager;
	auto window = subSystem.window;
	auto mm = managers.materialManager;
	auto lm = managers.lightManager;


	Core::Entity camera = em->Create();
	Core::ICameraManager::CreateInfo cmci;
	cmci.aspectRatio = 1280.0f / 720.0f;
	tm->Create(camera, { 0, 0, -10 });
	cm->Create(camera, cmci);
	cm->SetActive(camera);
	
	Core::IMaterialManager::CreateInfo mInfo;
	auto material = Utilz::GUID("MCModell.mat");
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	mInfo.shader = shader;
	mInfo.materialFile = material;
	Core::Entity box = em->Create();
	tm->Create(box, { 0,0,5 }, { 0,0,0 }, { 8,8,1 });
	mm->Create(box, mInfo);

	Core::IRenderableManager::CreateInfo rmci;
	rmci.meshGUID = "Placeholder_Block.mesh";
	rmci.transparent = false;
	rmci.wireframe = false;
	rm->CreateRenderableObject(box, rmci);
	rm->ToggleRenderableObject(box, true);

	Core::Entity decal = em->Create();
	tm->Create(decal, { 1.0f,0.0f, 4.5f },{0,0,0}, {1,1,10});
	dm->Create(decal, "ft_stone01_c.sei");

	Core::Entity decal2 = em->Create();
	tm->Create(decal2, { -2.0f,0.0f, 4.5f }, { 0,0,0 }, { 1,1,10 });
	dm->Create(decal2, "ft_stone01_c.sei");
	tm->BindChild(box, decal);
	tm->BindChild(box, decal2);
	auto l = em->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 5.0f, 2.0f };
	d.color = { 1, 1,1 };
	lm->Create(l, d);
	lm->ToggleLight(l, true);

	window->MapActionButton(Window::KeyEscape, Window::KeyEscape);

	window->MapActionButton(Window::KeyW, Window::KeyW);
	window->MapActionButton(Window::KeyA, Window::KeyA);
	window->MapActionButton(Window::KeyS, Window::KeyS);
	window->MapActionButton(Window::KeyD, Window::KeyD);
	window->MapActionButton(Window::KeyLeft, Window::KeyLeft);
	window->MapActionButton(Window::KeyRight, Window::KeyRight);
	window->MapActionButton(Window::KeyO, Window::KeyO);

	bool running = true;
	float dt = 1/60.0f * 5.0f;
	while(running)
	{
		engine->BeginFrame();
		if (window->ButtonPressed(Window::KeyEscape))
			running = false;
		auto forward = tm->GetForward(camera);
		auto right = tm->GetRight(camera);
		if (window->ButtonDown(Window::KeyW))
			tm->Move(camera, { forward.x * dt, forward.y * dt, forward.z * dt, 0.0f });
		if (window->ButtonDown(Window::KeyS))
			tm->Move(camera, { -forward.x * dt, -forward.y * dt, -forward.z * dt, 0.0f });
		if (window->ButtonDown(Window::KeyD))
			tm->Move(camera, { right.x * dt, right.y * dt, right.z * dt, 0.0f });
		if (window->ButtonDown(Window::KeyA))
			tm->Move(camera, { -right.x * dt, -right.y * dt, -right.z * dt, 0.0f });
		if (window->ButtonDown(Window::KeyLeft))
			tm->Rotate(camera, 0.0f, -3.14 / 10.0f* dt, 0.0f);
		if (window->ButtonDown(Window::KeyRight))
			tm->Rotate(camera, 0.0f, 3.14 / 10.0f* dt, 0.0f);

		if(window->ButtonPressed(Window::KeyO))
		{
			em->Destroy(decal);
			DirectX::XMFLOAT3 newPos = { (float)(rand() % 5), (float)(rand() % 5), 0.0f };
			DirectX::XMFLOAT3 newRot = { 0,0,0 };// tm->GetRotation(box);
			DirectX::XMFLOAT3 newScale = { 1.0f, 1.0f, 10.0f };
			decal = em->Create();
			tm->Create(decal, newPos, newRot, newScale);
			dm->Create(decal, "BlackPink.sei");
			tm->BindChild(box, decal, true, true);

		}

		//tm->Rotate(box, 0.0f, 3.14f * dt * 0.2f, 0.0f);
		engine->EndFrame();
	}
	engine->Release();
	delete engine;
	return true;
}
