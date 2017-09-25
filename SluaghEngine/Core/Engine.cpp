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

	return 0;
}

int SE::Core::Engine::Frame(double dt)
{

	transformManager->Frame();
	renderableManager->Frame();
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
	optionHandler->UnloadOption("Config.ini");

	delete cameraManager;
	delete collisionManager;
	delete materialManager;
	delete renderableManager;
	delete renderer;
	delete window;
	delete resourceHandler;
	delete entityManager;
	delete transformManager;
	delete audioManager;
	delete optionHandler;
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
