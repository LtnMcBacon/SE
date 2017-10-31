#include "SkeletonAnimationTest.h"
#include <Core\IEngine.h>
#include <Profiler.h>

#include <Utilz\Timer.h>
#include <Utilz\Tools.h>

#include <Imgui\imgui.h>

#ifdef _DEBUG

#pragma comment(lib, "ImGuiDX11SDLD.lib")

#else

#pragma comment(lib, "ImGuiDX11SDL.lib");

#endif

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif
using namespace DirectX;
enum ActionButton
{
	Exit,
	Hide,
	Show,
	Up,
	Down,
	Left,
	Right,
	Fullscreen,
	Rise,
	Sink,
	Stop,
	Start
};


SE::Test::SkeletonAnimationTest::SkeletonAnimationTest()
{
}


SE::Test::SkeletonAnimationTest::~SkeletonAnimationTest()
{

}

bool SE::Test::SkeletonAnimationTest::Run(DevConsole::IConsole * console)
{
	StartProfile;
	auto engine = Core::CreateEngine();
	engine->Init();
	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());
	
	subSystem.devConsole->Toggle();


	auto& mainC = managers.entityManager->Create();

	managers.transformManager->Create(mainC);
	managers.transformManager->SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	managers.transformManager->SetRotation(mainC, 0.0f, 3.14f, 0.0f);
	
	Core::ICameraManager::CreateInfo cInfo;
	cInfo.aspectRatio = (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 640) / (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800);

	managers.cameraManager->Create(managers.cameraManager->GetActive(), cInfo);

	managers.transformManager->SetRotation(managers.cameraManager->GetActive(), 0.0f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(managers.cameraManager->GetActive(), { 0.0f, 1.0f, -5.0f });

	subSystem.window->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	subSystem.window->MapActionButton(ActionButton::Hide, Window::KeyO);
	subSystem.window->MapActionButton(ActionButton::Show, Window::KeyK);
	subSystem.window->MapActionButton(ActionButton::Up, Window::KeyW);
	subSystem.window->MapActionButton(ActionButton::Down, Window::KeyS);
	subSystem.window->MapActionButton(ActionButton::Left, Window::KeyA);
	subSystem.window->MapActionButton(ActionButton::Right, Window::KeyD);
	subSystem.window->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
	subSystem.window->MapActionButton(Window::MouseLeft, Window::MouseLeft);

	subSystem.window->MapActionButton(ActionButton::Stop, Window::KeyP);
	subSystem.window->MapActionButton(ActionButton::Start, Window::KeyO);


	subSystem.window->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
	subSystem.window->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

	Core::IMaterialManager::CreateInfo info;
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	auto material = Utilz::GUID("MCModell.mat");
	info.shader = shader;	
	info.materialFile = material;


	managers.materialManager->Create(mainC, info);
	Core::IAnimationManager::CreateInfo sai;
	sai.mesh = "Run.mesh";
	sai.skeleton = "Run.skel";
	sai.animationCount = 1;
	Utilz::GUID anims[] = { "RunAnimation_Run.anim" };
	sai.animations = anims;
	managers.animationManager->CreateAnimatedObject(mainC, sai);


	managers.collisionManager->CreateBoundingHierarchy(mainC, "Run.mesh");
	managers.animationManager->ToggleVisible(mainC, true);

	managers.animationManager->Start(mainC, "RunAnimation_Run.anim", 1.0f);

	auto& c2 = managers.entityManager->Create();
	//managers.transformManager->Create(c2, { 3.0f, 0.0f, 0.0f });
	//managers.materialManager->Create(c2, info);
	//managers.animationManager->CreateAnimation(c2, sai);
	//managers.animationManager->Start(c2, "RunAnimation_bakedTest.anim", 0.1f);
	//managers.renderableManager->CreateRenderableObject(c2, { "bakedTest.mesh" });

	//managers.collisionManager->CreateBoundingHierarchy(c2, "bakedTest.mesh");

	//managers.renderableManager->ToggleRenderableObject(c2, true);

	auto& l = managers.entityManager->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 5.0f, -5.0f };
	d.color = { 1, 1,1 };
	d.castShadow = false;
	d.isOrtographic = false;
	managers.lightManager->Create(l, d);
	managers.lightManager->ToggleLight(l, true);


	subSystem.window->MapActionButton(0, Window::KeyEscape);

	bool running = true;
	Utilz::Timer timer;
	float radians = (180 * 3.14) / 180;

	//managers.transformManager->Rotate(mainC, 0.0f, radians, 0.01f);
	static float keyframe = 0.0f;
	static float speed = 0.0f;

	int width = subSystem.optionsHandler->GetOptionInt("Window", "width", 800);
	int height = subSystem.optionsHandler->GetOptionInt("Window", "height", 640);
	auto entityToChange = c2;

	subSystem.window->BindMouseClickCallback(Window::MouseLeft, [&managers, &entityToChange, width ,height](auto x, auto y) {
		XMVECTOR o;
		XMVECTOR dir;
		float d;
		managers.cameraManager->WorldSpaceRayFromScreenPos(x, y, width, height, o, dir);
		managers.collisionManager->Pick(o, dir, entityToChange, d);
	});

	while (running)
	{
		if (subSystem.window->ButtonPressed(0))
			running = false;

		timer.Tick();
		float dt = timer.GetDelta();
	
		if (subSystem.window->ButtonDown(ActionButton::Up))
			managers.transformManager->Move(managers.cameraManager->GetActive(), DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Down))
			managers.transformManager->Move(managers.cameraManager->GetActive(), DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.01f*dt });
		if (subSystem.window->ButtonDown(ActionButton::Right))
			managers.transformManager->Move(managers.cameraManager->GetActive(), DirectX::XMFLOAT3{ 0.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Left))
			managers.transformManager->Move(managers.cameraManager->GetActive(), DirectX::XMFLOAT3{ -0.01f*dt, 0.0f, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Rise))
			managers.transformManager->Move(managers.cameraManager->GetActive(), DirectX::XMFLOAT3{ 0.0f, -0.01f*dt, 0.0f });
		if (subSystem.window->ButtonDown(ActionButton::Sink))
			managers.transformManager->Move(managers.cameraManager->GetActive(), DirectX::XMFLOAT3{ 0.0f, 0.01f*dt, 0.0f });

		if (subSystem.window->ButtonDown(ActionButton::Stop))
			managers.animationManager->Pause(c2);
		if (subSystem.window->ButtonDown(ActionButton::Start))
			managers.animationManager->Start(c2);

		engine->BeginFrame();
	
		ImGui::Begin("Animation Stuff");

		if(ImGui::SliderFloat("C2 Keyframe ", &keyframe, 0.0f, 60.0f))
			managers.animationManager->SetKeyFrame(entityToChange, keyframe);
		if (ImGui::SliderFloat("C2 Speed ", &speed, -1.0f, 1.0f))
			managers.animationManager->SetSpeed(entityToChange, speed);
		if (ImGui::Button("Start"))
			managers.animationManager->Start(entityToChange);
		if (ImGui::Button("Stop"))
			managers.animationManager->Pause(entityToChange);

		ImGui::TextUnformatted((std::string("Entity: ") + std::to_string( entityToChange.id)).c_str());
		
		ImGui::End();
		
		engine->EndFrame();
	}



	engine->Release(); delete engine;
	ProfileReturnConst(true);
}
