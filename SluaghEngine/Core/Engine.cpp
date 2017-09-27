#include "Core/Engine.h"
#include "Core/EntityManager.h"
#include <Graphics\Renderer.h>
#include <Window\WindowSDL.h>
#include <ResourceHandler\ResourceHandler.h>
#include <OBJParser\Parsers.h>
#include <Profiler.h>

#ifdef _DEBUG
#pragma comment(lib, "OBJParserD.lib")
#else
#pragma comment(lib, "OBJParser.lib")
#endif



SE::Core::Engine& SE::Core::Engine::GetInstance()
{
	static Engine instance;
	return instance;
}

int SE::Core::Engine::Init(const InitializationInfo& info)
{
	optionHandler = new OptionHandler();
	optionHandler->Initialize("Config.ini");

	entityManager = new EntityManager;
	window = new Window::WindowSDL();
	renderer = new Graphics::Renderer();
	resourceHandler = new ResourceHandler::ResourceHandler();
	audioManager = new AudioManager();
	

	auto r = resourceHandler->Initialize();
	if (r)
		return r;
	r = window->Initialize();
	if (r)
		return r;
	r = renderer->Initialize(window->GetHWND());
	if (r)
		return r;
	r = audioManager->Initialize();
	if (r)
		return r;

	transformManager = new TransformManager(entityManager);
	materialManager = new MaterialManager(resourceHandler, renderer, *entityManager);
	collisionManager = new CollisionManager(resourceHandler, *entityManager, transformManager);
	cameraManager = new CameraManager(renderer, *entityManager, transformManager);
	renderableManager = new RenderableManager(resourceHandler, renderer, *entityManager, transformManager, materialManager);
	debugRenderManager = new DebugRenderManager(renderer, resourceHandler, *entityManager, transformManager);
	perFrameStackAllocator = new Utilz::StackAllocator;
	perFrameStackAllocator->InitStackAlloc(1024U * 1024U * 5U);
	guiManager = new GUIManager(resourceHandler, renderer, *entityManager);
	//debugRenderManager = new DebugRenderManager(renderer, resourceHandler, *entityManager, transformManager);

	InitStartupOption();

	return 0;
}

int SE::Core::Engine::Frame(double dt)
{

	transformManager->Frame();
	renderableManager->Frame();
	debugRenderManager->Frame(*perFrameStackAllocator);
	materialManager->Frame();
	collisionManager->Frame();
	window->Frame();
	cameraManager->Frame();
	renderer->Render();
	return 0;
}

int SE::Core::Engine::Release()
{
	renderer->Shutdown();
	window->Shutdown();
	audioManager->Shutdown();
	resourceHandler->Shutdown();
	guiManager->Shutdown();
	optionHandler->UnloadOption("Config.ini");

	delete cameraManager;
	delete collisionManager;
	delete materialManager;
	delete renderableManager;
	delete debugRenderManager;
	delete renderer;
	delete window;
	delete resourceHandler;
	delete entityManager;
	delete transformManager;
	delete audioManager;
	delete optionHandler;
	delete perFrameStackAllocator;
	delete guiManager;
	entityManager = nullptr; //Just to make ReSharper stfu about function "possibly being const"
	return 0;
}

SE::Core::Engine::Engine()
{
	entityManager = nullptr;
	transformManager = nullptr;
}

SE::Core::Engine::~Engine()
{
	

}

void SE::Core::Engine::InitStartupOption()
{
	//Set Sound Vol
	audioManager->SetSoundVol(Audio::MasterVol ,optionHandler->GetOption("Audio", "masterVolume", 100));
	audioManager->SetSoundVol(Audio::EffectVol, optionHandler->GetOption("Audio", "effectVolume", 80));
	audioManager->SetSoundVol(Audio::BakgroundVol, optionHandler->GetOption("Audio", "bakgroundVolume", 50));
	
	//Set Window and graphic
	bool sizeChange = window->SetWindow(optionHandler->GetOption("Window", "height", 720), optionHandler->GetOption("Window", "width", 1280), (bool)optionHandler->GetOption("Window", "fullScreen", 0));
	
	if (sizeChange == true)
	{
		renderer->ResizeSwapChain(window->GetHWND());
	}

	optionHandler->Register(Utilz::Delegate<void()>::Make<Engine, &Engine::OptionUpdate>(this));
}

void SE::Core::Engine::OptionUpdate()
{
	StartProfile;
	//Set Sound Vol
	audioManager->SetSoundVol(Audio::MasterVol, optionHandler->GetOption("Audio", "masterVolume", 100));
	audioManager->SetSoundVol(Audio::EffectVol, optionHandler->GetOption("Audio", "effectVolume", 80));
	audioManager->SetSoundVol(Audio::BakgroundVol, optionHandler->GetOption("Audio", "bakgroundVolume", 50));
	
	//Set Window and graphic
	bool sizeChange = window->SetWindow(optionHandler->GetOption("Window", "height", 720), optionHandler->GetOption("Window", "width", 1280), (bool)optionHandler->GetOption("Window", "fullScreen", 0));

	if (sizeChange == true)
	{
		renderer->ResizeSwapChain(window->GetHWND());
	}
	
	ProfileReturnVoid;
}
