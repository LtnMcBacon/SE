#include "BoundingTest.h"
#include <Core\IEngine.h>
//#include <Core\managers.transformManager->h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

SE::Test::BoundingTest::BoundingTest()
{
}


SE::Test::BoundingTest::~BoundingTest()
{
}


static bool test = false;
static bool test2 = false;
static SE::Core::IEngine* e;
void Collide2(const SE::Core::Entity& hit, const SE::Core::Entity& hitter)
{
	test2 = true;
}

void Collide1(const SE::Core::Entity& hit, const SE::Core::Entity& hitter)
{
	if (!test)
	{
		test = true;
		auto managers = e->GetManagers();
		managers.collisionManager->SetCollideWithAnyCallback(&Collide2);

		auto& block2 = managers.entityManager->Create();
		managers.collisionManager->CreateBoundingHierarchy(block2,  {"MCModell.mesh"} );
		managers.renderableManager->CreateRenderableObject(block2, { {"MCModell.mesh"} });
		managers.renderableManager->ToggleRenderableObject(block2, true);
		managers.transformManager->SetPosition(block2, DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));

		managers.transformManager->SetPosition(hit, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

		managers.entityManager->Destroy(hitter);
	}
		
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
	Fullscreen,
	Rise,
	Sink
};


bool SE::Test::BoundingTest::Run(DevConsole::IConsole * console)
{
	e = Core::CreateEngine();
	e->Init();
	auto managers = e->GetManagers();
	auto subSystem = e->GetSubsystems();

	managers.collisionManager->SetCollideWithAnyCallback(&Collide1);

	Core::ICameraManager::CreateInfo cInfo;
	cInfo.aspectRatio = (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "width", 800) / (float)subSystem.optionsHandler->GetOptionUnsignedInt("Window", "height", 640);
	cInfo.posistion = { 0.0f, 0.0f, -2.0f };
	auto camera = managers.entityManager->Create();
	managers.cameraManager->Create(camera, cInfo);
	managers.cameraManager->SetActive(camera);

	auto block1 = managers.entityManager->Create();
	managers.collisionManager->CreateBoundingHierarchy(block1, {"MCModell.mesh"});
	managers.renderableManager->CreateRenderableObject(block1, {"MCModell.mesh"});
	managers.renderableManager->ToggleRenderableObject(block1, true);
	managers.transformManager->SetPosition(block1, DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));

	auto block2 = managers.entityManager->Create();
	managers.collisionManager->CreateBoundingHierarchy(block2, {"MCModell.mesh"});	
	managers.renderableManager->CreateRenderableObject(block2, {"MCModell.mesh"});
	managers.renderableManager->ToggleRenderableObject(block2, true);
	managers.transformManager->SetPosition(block2, DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));

	managers.collisionManager->BindOnCollideWithAny(block1);
	
	managers.debugRenderManager->ToggleDebugRendering(block1, true);
	managers.debugRenderManager->ToggleDebugRendering(block2, true);

	subSystem.window->MapActionButton(ActionButton::Exit, Window::KeyEscape);
	subSystem.window->MapActionButton(ActionButton::Left, Window::KeyD);
	test = false;
	test2 = false;
	bool su = false;
	for(int i = 0; i < 500; i++)
	{
		if (subSystem.window->ButtonPressed(ActionButton::Exit))
			break;

		if(!test)
			managers.transformManager->Move(block1, DirectX::XMFLOAT3(0.01f, 0.0f, 0.0f));
		else if(test)
		{
			if(!test2)
				managers.transformManager->Move(block1, DirectX::XMFLOAT3(-0.01f, 0.0f, 0.0f));
		}
		
		e->BeginFrame();
		e->EndFrame();
		
		managers.debugRenderManager->ToggleDebugRendering(block1, true);
		managers.debugRenderManager->ToggleDebugRendering(block2, true);
		if (test && test2)
		{
			su = true;
		}
			
	}

	e->Release();
	delete e;
	return su;
}
