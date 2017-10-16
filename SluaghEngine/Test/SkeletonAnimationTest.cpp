#include "SkeletonAnimationTest.h"
#include <Core\Engine.h>
#include <Profiler.h>

#include <Utilz\Timer.h>
#include <Utilz\Tools.h>

#include <Imgui\imgui.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

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

bool SE::Test::SkeletonAnimationTest::Run(Utilz::IConsoleBackend * console)
{
	StartProfile;
	auto& e = Core::Engine::GetInstance();
	auto re = e.Init(Core::Engine::InitializationInfo());
	e.GetWindow();
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		ProfileReturnConst(false);
	}

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();
	auto& cm = e.GetCameraManager();
	auto& am = e.GetAnimationManager();
	auto& mm = e.GetMaterialManager();


	auto& om = e.GetOptionHandler();
	auto handle = e.GetWindow();
	auto& col = e.GetCollisionManager();

	auto& mainC = em.Create();

	tm.Create(mainC);
	tm.SetPosition(mainC, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	tm.SetRotation(mainC, 0.0f, 3.14f, 0.0f);
	
	Core::CameraBindInfoStruct cInfo;
	cInfo.aspectRatio = (float)om.GetOptionUnsignedInt("Window", "height", 640) / (float)om.GetOptionUnsignedInt("Window", "width", 800);

	cm.Bind(cm.GetActive(), cInfo);

	tm.SetRotation(cm.GetActive(), 0.0f, 0.0f, 0.0f);
	tm.SetPosition(cm.GetActive(), { 0.0f, 1.0f, -5.0f });

	handle->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	handle->MapActionButton(ActionButton::Hide, Window::KeyO);
	handle->MapActionButton(ActionButton::Show, Window::KeyK);
	handle->MapActionButton(ActionButton::Up, Window::KeyW);
	handle->MapActionButton(ActionButton::Down, Window::KeyS);
	handle->MapActionButton(ActionButton::Left, Window::KeyA);
	handle->MapActionButton(ActionButton::Right, Window::KeyD);
	handle->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);
	handle->MapActionButton(Window::MouseLeft, Window::MouseLeft);

	handle->MapActionButton(ActionButton::Stop, Window::KeyP);
	handle->MapActionButton(ActionButton::Start, Window::KeyO);


	handle->MapActionButton(ActionButton::Rise, Window::KeyShiftL);
	handle->MapActionButton(ActionButton::Sink, Window::KeyCtrlL);

	Core::MaterialManager::CreateInfo info;
	Utilz::GUID textures[] = { Utilz::GUID("TestDiffuse.sei"), Utilz::GUID("purewhite.sei") };
	Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex"), Utilz::GUID("diffuseTexSec") };
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.shaderResourceNames = resourceNames;
	info.textureFileNames = textures;
	info.textureCount = 2;

	mm.Create(mainC, info);
	Core::CreateAnimationInfo sai;
	sai.skeleton = "bakedTest.skel";
	sai.animationCount = 1;
	Utilz::GUID anims[] = { "SwingAnimation_bakedTest.anim" };
	sai.animations = anims;
	am.CreateAnimation(mainC, sai);
	am.Start(mainC, "SwingAnimation_bakedTest.anim", 1.0f);


	rm.CreateRenderableObject(mainC, Utilz::GUID("bakedTest.mesh"));

	col.CreateBoundingHierarchy(mainC, "bakedTest.mesh");

	rm.ToggleRenderableObject(mainC, true);


	auto& c2 = em.Create();
	tm.Create(c2, { 3.0f, 0.0f, 0.0f });
	mm.Create(c2, info);
	am.CreateAnimation(c2, sai);
	am.Start(c2, "RunAnimation_bakedTest.anim", 0.1f);
	rm.CreateRenderableObject(c2, "bakedTest.mesh");

	col.CreateBoundingHierarchy(c2, "bakedTest.mesh");

	rm.ToggleRenderableObject(c2, true);




	e.GetWindow()->MapActionButton(0, Window::KeyEscape);

	bool running = true;
	Utilz::Timer timer;
	float radians = (180 * 3.14) / 180;

	//tm.Rotate(mainC, 0.0f, radians, 0.01f);
	static float keyframe = 0.0f;
	static float speed = 0.0f;

	int width = om.GetOptionInt("Window", "width", 800);
	int height = om.GetOptionInt("Window", "height", 640);
	auto entityToChange = c2;

	handle->BindMouseClickCallback(Window::MouseLeft, [&cm, &col, &entityToChange, width ,height](auto x, auto y) {
		XMVECTOR o;
		XMVECTOR dir;
		cm.WorldSpaceRayFromScreenPos(x, y, width, height, o, dir);
		col.Pick(o, dir, entityToChange);
	});

	while (running)
	{
		if (e.GetWindow()->ButtonPressed(0))
			running = false;

		timer.Tick();
		float dt = timer.GetDelta();

		if (handle->ButtonDown(ActionButton::Up))
			tm.Move(cm.GetActive(), DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.01f*dt });
		if (handle->ButtonDown(ActionButton::Down))
			tm.Move(cm.GetActive(), DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.01f*dt });
		if (handle->ButtonDown(ActionButton::Right))
			tm.Move(cm.GetActive(), DirectX::XMFLOAT3{ 0.01f*dt, 0.0f, 0.0f });
		if (handle->ButtonDown(ActionButton::Left))
			tm.Move(cm.GetActive(), DirectX::XMFLOAT3{ -0.01f*dt, 0.0f, 0.0f });
		if (handle->ButtonDown(ActionButton::Rise))
			tm.Move(cm.GetActive(), DirectX::XMFLOAT3{ 0.0f, -0.01f*dt, 0.0f });
		if (handle->ButtonDown(ActionButton::Sink))
			tm.Move(cm.GetActive(), DirectX::XMFLOAT3{ 0.0f, 0.01f*dt, 0.0f });

		if (handle->ButtonDown(ActionButton::Stop))
			am.Pause(c2);
		if (handle->ButtonDown(ActionButton::Start))
			am.Start(c2);




		e.BeginFrame();
		if(ImGui::SliderFloat("C2 Keyframe ", &keyframe, 0.0f, 60.0f))
			am.SetKeyFrame(entityToChange, keyframe);
		if (ImGui::SliderFloat("C2 Speed ", &speed, -1.0f, 1.0f))
			am.SetSpeed(entityToChange, speed);
		if (ImGui::Button("Start"))
			am.Start(entityToChange);
		if (ImGui::Button("Stop"))
			am.Pause(entityToChange);

		ImGui::TextUnformatted((std::string("Entity: ") + std::to_string( entityToChange.id)).c_str());

		e.Frame(dt);
	}



	e.Release();
	ProfileReturnConst(true);
}
