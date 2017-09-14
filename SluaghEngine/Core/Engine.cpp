#include "Core/Engine.h"
#include "Core/EntityManager.h"
#include <Graphics\Renderer.h>
#include <Window\Window.h>
#include <ResourceHandler\ResourceHandler.h>
#include <OBJParser\Parsers.h>


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


	return 0;
}

int SE::Core::Engine::Release()
{
	renderer->Shutdown();
	window->Shutdown();
	resourceHandler->Shutdown();

	delete renderer;
	delete window;
	delete resourceHandler;
	delete entityManager;
	return 0;
}

SE::Core::EntityManager& SE::Core::Engine::GetEntityManager() const
{
	return *entityManager;
}

SE::Graphics::IRenderer * SE::Core::Engine::GetRenderer() const
{
	return renderer;
}

SE::Window::InterfaceWindow * SE::Core::Engine::GetWindow() const
{
	return window;
}

SE::ResourceHandler::IResourceHandler * SE::Core::Engine::GetResourceHandler() const
{
	return resourceHandler;
}

SE::Core::Engine::Engine()
{
	
}

SE::Core::Engine::~Engine()
{

}
