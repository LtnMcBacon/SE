#include "Core/Engine.h"
#include "Core/EntityManager.h"
#include <Graphics\Renderer.h>
#include <Window\Window.h>
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

	entityManager = new EntityManager;
	window = new Window::Window();
	renderer = new Graphics::Renderer();
	resourceHandler = new ResourceHandler::ResourceHandler();
	

	auto r = resourceHandler->Initialize();
	if (r)
		return r;
	r = window->Initialise();
	if (r)
		return r;
	r = renderer->Initialize(window->GethWnd());
	if (r)
		return r;

	transformManager = new TransformManager(entityManager);
	renderableManager = new RenderableManager(*entityManager, renderer, resourceHandler);

	return 0;
}

int SE::Core::Engine::Frame(double dt)
{
	return 0;
}

int SE::Core::Engine::Release()
{
	renderer->Shutdown();
	window->Shutdown();
	resourceHandler->Shutdown();

	delete renderableManager;
	delete renderer;
	delete window;
	delete resourceHandler;
	delete entityManager;
	delete transformManager;
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
