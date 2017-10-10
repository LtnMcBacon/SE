#include "MaterialManagerTest.h"
#include <Core\Engine.h>

#include <Utilz\Timer.h>

SE::Test::MaterialManagerTest::MaterialManagerTest()
{
}


SE::Test::MaterialManagerTest::~MaterialManagerTest()
{
}
enum ActionButton
{
	Exit,
	Hide,
	Show,
	Up,
	Down,
	Left,
	Right,
	Fullscreen
};
bool SE::Test::MaterialManagerTest::Run(Utilz::IConsoleBackend * console)
{
	auto& engine = Core::Engine::GetInstance();
	engine.Init(Core::Engine::InitializationInfo());
	auto& em = engine.GetEntityManager();
	auto& mm = engine.GetMaterialManager();
	auto& tm = engine.GetTransformManager();
	auto& cm = engine.GetCameraManager();
	Core::Entity entity = em.Create();

	Core::MaterialManager::CreateInfo info;
	Utilz::GUID textures[] = { Utilz::GUID("Diffuse.sei"), Utilz::GUID("purewhite.sei") };
	Utilz::GUID resourceNames[] = { Utilz::GUID("diffuseTex"), Utilz::GUID("diffuseTexSec") };
	auto shader = Utilz::GUID("SimpleTexPS.hlsl");
	info.shader = shader;
	info.shaderResourceNames = resourceNames;
	info.textureFileNames = textures;
	info.textureCount = 2;


	
	mm.Create(entity, info);
	auto& camera = em.Create();
	cm.Bind(camera);
	cm.SetActive(camera);
	tm.SetRotation(camera, 0.9f, 0.0f, 0.0f);
	tm.SetPosition(camera, { 0.0f, 10.0f, -20.0f });

	auto& rm = engine.GetRenderableManager();

	tm.Create(entity);

	rm.CreateRenderableObject(entity, Utilz::GUID("MCModell.mesh"));
	rm.ToggleRenderableObject(entity, true);

	auto w = engine.GetWindow();


	w->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	w->MapActionButton(ActionButton::Hide, Window::KeyO);
	w->MapActionButton(ActionButton::Show, Window::KeyK);
	w->MapActionButton(ActionButton::Up, Window::KeyW);
	w->MapActionButton(ActionButton::Down, Window::KeyS);
	w->MapActionButton(ActionButton::Left, Window::KeyA);
	w->MapActionButton(ActionButton::Right, Window::KeyD);
	w->MapActionButton(ActionButton::Fullscreen, Window::KeyF10);

	bool running = true;
	Utilz::Timer timer;
	auto& oh = engine.GetOptionHandler();

	bool full = oh.GetOptionBool("Window", "fullScreen", 0);
	while (running)
	{
		timer.Tick();
		float dt = timer.GetDelta();
		if (w->ButtonPressed(ActionButton::Exit))
			running = false;
		
		if (w->ButtonPressed(ActionButton::Hide))
			rm.ToggleRenderableObject(entity, false);
		if (w->ButtonPressed(ActionButton::Show))
			rm.ToggleRenderableObject(entity, true);

		if (w->ButtonPressed(ActionButton::Fullscreen))
		{
			full = full ? 0 : 1;
			oh.SetOptionBool("Window", "fullScreen", full);
			oh.Trigger();
		}

		if (w->ButtonDown(ActionButton::Up))
			tm.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.01f*dt });
		if (w->ButtonDown(ActionButton::Down))
			tm.Move(camera, DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.01f*dt });
		if (w->ButtonDown(ActionButton::Right))
			tm.Move(camera, DirectX::XMFLOAT3{ 0.01f*dt, 0.0f, 0.0f });
		if (w->ButtonDown(ActionButton::Left))
			tm.Move(camera, DirectX::XMFLOAT3{ -0.01f*dt, 0.0f, 0.0f });

		engine.Frame(0.01f);	
	}


	engine.Release();

	return true;
}
