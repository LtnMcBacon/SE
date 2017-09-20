#include "Core/Engine.h"
#include "Core/EntityManager.h"
#include <Graphics\Renderer.h>
#include <Window\WindowSDL.h>
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
	window = new Window::WindowSDL();
	renderer = new Graphics::Renderer();
	resourceHandler = new ResourceHandler::ResourceHandler();
	

	auto r = resourceHandler->Initialize();
	if (r)
		return r;
	r = window->Initialize();
	if (r)
		return r;
	r = renderer->Initialize(window->GetHWND());
	if (r)
		return r;

	transformManager = new TransformManager(entityManager);
	renderableManager = new RenderableManager(*entityManager);
	materialManager = new MaterialManager(*entityManager);

	return 0;
}

int SE::Core::Engine::Release()
{
	renderer->Shutdown();
	window->Shutdown();
	resourceHandler->Shutdown();

	delete materialManager;
	delete renderableManager;
	delete renderer;
	delete window;
	delete resourceHandler;
	delete entityManager;
	delete transformManager;
	entityManager = nullptr; //Just to make ReSharper stfu about function "possibly being const"
	return 0;
}

void SE::Core::Engine::Frame()
{
	transformManager->Frame();
	renderableManager->Frame();
	window->Frame();
	renderer->Render();
}

SE::Core::EntityManager& SE::Core::Engine::GetEntityManager() const
{
	return *entityManager;
}

SE::Core::RenderableManager& SE::Core::Engine::GetRenderableManager() const
{
	return *renderableManager;
}

SE::Graphics::IRenderer * SE::Core::Engine::GetRenderer() const
{
	return renderer;
}

SE::Window::IWindow * SE::Core::Engine::GetWindow() const
{
	return window;
}

SE::ResourceHandler::IResourceHandler * SE::Core::Engine::GetResourceHandler() const
{
	return resourceHandler;
}

SE::Core::TransformManager& SE::Core::Engine::GetTransformManager() const
{
	return *transformManager;
}

SE::Core::MaterialManager & SE::Core::Engine::GetMaterialManager() const
{
	return *materialManager;
}

SE::Core::Engine::Engine()
{
	entityManager = nullptr;
	transformManager = nullptr;
}

SE::Core::Engine::~Engine()
{

}
