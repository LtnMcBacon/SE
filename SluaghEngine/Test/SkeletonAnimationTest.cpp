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
	Increase,
	Lower
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
	auto& mainC2 = managers.entityManager->Create();
	auto& attachable = managers.entityManager->Create();

	managers.transformManager->Create(mainC);
	managers.transformManager->Create(mainC2);
	managers.transformManager->Create(attachable);

	Core::IRenderableManager::CreateInfo sword;
	sword.meshGUID = "Sword.mesh";
	managers.renderableManager->CreateRenderableObject(attachable, sword);
	managers.renderableManager->ToggleRenderableObject(attachable, true);

	float scale[3] = { 0.15f, 0.15f, 0.15f };
	float pos[3] = { 0,0,0};
	float arot[3] = { 0,0,0 };


	managers.transformManager->SetScale(attachable, DirectX::XMFLOAT3(scale));
	managers.transformManager->SetPosition(attachable, DirectX::XMFLOAT3(pos));
	managers.transformManager->SetRotation(attachable, arot[0], arot[1], arot[2]);

	managers.transformManager->SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	managers.transformManager->SetRotation(mainC, 0.0f, 3.14f, 0.0f);

	managers.transformManager->SetPosition(mainC2, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f));
	managers.transformManager->SetRotation(mainC2, 0.0f, 3.14f, 0.0f);
	
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

	subSystem.window->MapActionButton(ActionButton::Increase, Window::KeyUp);
	subSystem.window->MapActionButton(ActionButton::Lower, Window::KeyDown);


	subSystem.window->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
	subSystem.window->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

	Core::IMaterialManager::CreateInfo info;
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	auto material = Utilz::GUID("MCModell.mat");
	info.shader = shader;	
	info.materialFile = material;

	managers.materialManager->Create(mainC, info);
	managers.materialManager->Create(mainC2, info);

	Core::IAnimationManager::CreateInfo sai;
	sai.mesh = "MCModell.mesh";
	sai.skeleton = "MCModell.skel";
	sai.animationCount = 4;
	Utilz::GUID anims[] = { "TopRunAnim_MCModell.anim", "BottomRunAnim_MCModell.anim", "DeathAnim_MCModell.anim", "TopSwordAttackAnim_MCModell.anim" };
	sai.animations = anims;
	managers.animationManager->CreateAnimatedObject(mainC, sai);
	managers.animationManager->CreateAnimatedObject(mainC2, sai);

	managers.animationManager->ToggleVisible(mainC, true);
	managers.animationManager->ToggleVisible(mainC2, true);

	managers.animationManager->AttachToEntity(mainC, attachable, "LHand", 0);

	managers.collisionManager->CreateBoundingHierarchy(mainC, "Bodach.mesh");
	managers.collisionManager->CreateBoundingHierarchy(mainC2, "Bodach.mesh");

	Utilz::GUID mainAnim[] = { "TopRunAnim_MCModell.anim", "BottomRunAnim_MCModell.anim" };
	managers.animationManager->Start(mainC, mainAnim, 2, 2.0f, Core::AnimationFlags::IMMEDIATE | Core::AnimationFlags::LOOP);
	managers.animationManager->Start(mainC2, mainAnim, 2, 2.0f, Core::AnimationFlags::IMMEDIATE);

	auto& l = managers.entityManager->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 5.0f, 10.0f, -5.0f };
	d.color = { 1, 1, 1 };
	managers.lightManager->Create(l, d);
	managers.lightManager->ToggleLight(l, true);

	subSystem.window->MapActionButton(0, Window::KeyEscape);

	bool running = true;
	Utilz::Timer timer;
	float radians = (180 * 3.14) / 180;

	// managers.transformManager->Rotate(mainC, 0.0f, radians, 0.01f);
	static float keyframe = 0.0f;
	static float speed = 0.0f;
	static float blendFactorSpeed = 0.00f;
	static float rot = 0.0f;
	static float localRot[3] = { 0 };

	int width = subSystem.optionsHandler->GetOptionInt("Window", "width", 800);
	int height = subSystem.optionsHandler->GetOptionInt("Window", "height", 640);
	auto main1 = mainC;

	subSystem.window->BindMouseClickCallback(Window::MouseLeft, [&managers, &main1, width ,height](auto x, auto y) {
		XMVECTOR o;
		XMVECTOR dir;
		float d;
		managers.cameraManager->WorldSpaceRayFromScreenPos(x, y, width, height, o, dir);
		managers.collisionManager->Pick(o, dir, main1, d);
	});

	while (running)
	{
		if (subSystem.window->ButtonPressed(0))
			running = false;

		timer.Tick();
		float dt = timer.GetDelta();

	//	rot += 0.001;
		//managers.transformManager->SetRotation(mainC, 0.0f, rot, 0.0f);
	
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
		
		engine->BeginFrame();
	
		ImGui::Begin("Animation Stuff");
	
		ImGui::SliderFloat("Rot", &rot, -3.14f, 3.14f);

		if(ImGui::SliderFloat("C1 Keyframe ", &keyframe, 0.0f, 60.0f)){
			managers.animationManager->SetKeyFrame(mainC, keyframe);
			managers.animationManager->SetKeyFrame(mainC2, keyframe);

		}

		if (ImGui::SliderFloat("C1 Speed ", &speed, -10.0f, 10.0f)){
			
			managers.animationManager->SetSpeed(mainC, speed);
			managers.animationManager->SetSpeed(mainC2, speed);

		}

		if (ImGui::SliderFloat("C1 Blend Speed ", &blendFactorSpeed, -10.0f, 10.0f)) {

			managers.animationManager->SetBlendSpeed(mainC, 2, blendFactorSpeed);

		}

		if (ImGui::Button("BlendToAndBack")) {

			Utilz::GUID deathAnim = "TopSwordAttackAnim_MCModell.anim";
			managers.animationManager->Start(mainC, &deathAnim, 1, 2.0f, Core::AnimationFlags::BLENDTOANDBACK);
		}

		
		if (ImGui::SliderFloat3("Object Local Rot", arot, -1.8f, 1.8f)) {

			managers.transformManager->SetRotation(attachable, arot[0], arot[1], arot[2]);
		}
		if (ImGui::SliderFloat3("Object Local Scale", scale, 0.0f, 1.0f)) {

			managers.transformManager->SetScale(attachable, DirectX::XMFLOAT3(scale));
		}
		if (ImGui::SliderFloat3("Object Local Pos", pos, -1, 1)) {

			managers.transformManager->SetPosition(attachable, DirectX::XMFLOAT3(pos));
		}
		if (ImGui::Button("C1 Start")){

			managers.animationManager->Start(mainC, true);
			managers.animationManager->Start(mainC2, true);

		}

		if (ImGui::Button("C1 Stop")){

			managers.animationManager->Pause(mainC);
			managers.animationManager->Pause(mainC2);

		}

		ImGui::TextUnformatted((std::string("Entity: ") + std::to_string(main1.id)).c_str());
		
		ImGui::End();
		
		engine->EndFrame();
	}



	engine->Release(); delete engine;
	ProfileReturnConst(true);
}
