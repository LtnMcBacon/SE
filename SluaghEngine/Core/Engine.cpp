#include "Core/Engine.h"
#include "Core/EntityManager.h"
#include <Graphics\Renderer.h>
#include <Window\Window.h>
#include <ResourceHandler\ResourceHandler.h>

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

	resourceHandler->Initialize();
	window->Initialise();
	renderer->Initialize(window->GethWnd());

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
