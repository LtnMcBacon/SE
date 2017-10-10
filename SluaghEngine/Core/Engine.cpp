#include "Core/Engine.h"
#include "Core/EntityManager.h"
#include <Graphics\IRenderer.h>
#include <Window\IWindow.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Profiler.h>

#include <Imgui\imgui.h>
#include <ImGuiDX11SDL\ImGuiDX11SDL.h>


#ifdef _DEBUG
#pragma comment(lib, "ResourceHandlerD.lib")
#pragma comment(lib, "WindowD.lib")
#pragma comment(lib, "ImGuiDX11SDLD.lib")
#else
#pragma comment(lib, "ResourceHandler.lib")
#pragma comment(lib, "Window.lib")
#pragma comment(lib, "ImGuiDX11SDL.lib");
#endif




SE::Core::Engine& SE::Core::Engine::GetInstance()
{
	static Engine instance;
	return instance;
}

int SE::Core::Engine::Init(const InitializationInfo& info)
{
	StartProfile;
	optionHandler = new OptionHandler();
	optionHandler->Initialize("Config.ini");

	entityManager = new EntityManager;
	window = Window::CreateNewWindow();
	renderer = Graphics::CreateRenderer();
	resourceHandler = ResourceHandler::CreateResourceHandler();
	audioManager = new AudioManager(resourceHandler, *entityManager);
	

	auto r = resourceHandler->Initialize();
	if (r)
		ProfileReturnConst( r);
	r = window->Initialize();
	if (r)
		ProfileReturnConst(r);
	r = renderer->Initialize(window->GetHWND());
	if (r)
		ProfileReturnConst(r);
	r = audioManager->Initialize();
	if (r)
		ProfileReturnConst(r);


	transformManager = new TransformManager(entityManager);
	
	collisionManager = new CollisionManager(resourceHandler, *entityManager, transformManager);
	cameraManager = new CameraManager(renderer, *entityManager, transformManager);
	animationManager = new AnimationManager(renderer, resourceHandler, *entityManager);
	renderableManager = new RenderableManager(resourceHandler, renderer, *entityManager, transformManager, animationManager);
	materialManager = new MaterialManager(resourceHandler, renderer, *entityManager, renderableManager);
	debugRenderManager = new DebugRenderManager(renderer, resourceHandler, *entityManager, transformManager, collisionManager);
	perFrameStackAllocator = new Utilz::StackAllocator;
	perFrameStackAllocator->InitStackAlloc(1024U * 1024U * 5U);
	guiManager = new GUIManager(resourceHandler, renderer, *entityManager);
	lightManager = new LightManager(renderer, *entityManager, transformManager);

	//default camera
	auto defEntCam = entityManager->Create();
	cameraManager->Bind(defEntCam);
	cameraManager->SetActive(defEntCam);

	InitStartupOption();

	ImGuiDX11SDL_Init(renderer, window);
	devConsole = new DevConsole(renderer);
	ProfileReturnConst(0);
}

int SE::Core::Engine::BeginFrame()
{
	StartProfile;
	if (frameBegun)
		ProfileReturnConst( -1);
	frameBegun = true;
	window->Frame();
	ImGuiDX11SDL_NewFrame();
	renderer->BeginFrame();
	ProfileReturnConst(0);
}

int SE::Core::Engine::Frame(double dt)
{
	StartProfile;
	if (!frameBegun)
		BeginFrame();
	guiManager->Frame();
	lightManager->Frame();
	transformManager->Frame();
	renderableManager->Frame();
	debugRenderManager->Frame(*perFrameStackAllocator);
	audioManager->Frame();
	animationManager->Frame();
	materialManager->Frame();
	collisionManager->Frame();
	cameraManager->Frame();
	renderer->Render();
	devConsole->Frame();
	ImGui::Render();
	renderer->EndFrame();
	perFrameStackAllocator->ClearStackAlloc();
	frameBegun = false;
	ProfileReturnConst(0);
}

int SE::Core::Engine::Release()
{
	StartProfile;
	delete devConsole;
	ImGuiDX11SDL_Shutdown();
	
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
	delete animationManager;
	delete renderer;
	delete window;
	delete resourceHandler;
	delete entityManager;
	delete transformManager;
	delete audioManager;
	delete optionHandler;
	delete perFrameStackAllocator;
	delete guiManager;
	delete lightManager;
	entityManager = nullptr; //Just to make ReSharper stfu about function "possibly being const"
	ProfileReturnConst(0);
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
	audioManager->SetSoundVol(Audio::MasterVol, optionHandler->GetOptionUnsignedInt("Audio", "masterVolume", 100));
	audioManager->SetSoundVol(Audio::EffectVol, optionHandler->GetOptionUnsignedInt("Audio", "effectVolume", 80));
	audioManager->SetSoundVol(Audio::BakgroundVol, optionHandler->GetOptionUnsignedInt("Audio", "bakgroundVolume", 50));

	//Set Camera
	size_t height = optionHandler->GetOptionUnsignedInt("Window", "height", 720);
	size_t width = optionHandler->GetOptionUnsignedInt("Window", "width", 1280);
	CameraBindInfoStruct camInfo;
	camInfo.aspectRatio = optionHandler->GetOptionDouble("Camera", "aspectRatio", (width / height));
	camInfo.fov = optionHandler->GetOptionDouble("Camera", "fov", 1.570796);
	camInfo.nearPlane = optionHandler->GetOptionDouble("Camera", "nearPlane", 0.01);
	camInfo.farPlance = optionHandler->GetOptionDouble("Camera", "farPlance", 100.0);
	cameraManager->UpdateCamera(camInfo);

	//Set Window and graphic
	bool sizeChange = window->SetWindow(height, width, optionHandler->GetOptionBool("Window", "fullScreen", false));

	if (sizeChange == true)
	{
		renderer->ResizeSwapChain(window->GetHWND());
	}

	optionHandler->Register({ this, &Engine::OptionUpdate });
}

void SE::Core::Engine::OptionUpdate()
{
	StartProfile;
	//Set Sound Vol
	audioManager->SetSoundVol(Audio::EffectVol, optionHandler->GetOptionUnsignedInt("Audio", "effectVolume", 80));
	audioManager->SetSoundVol(Audio::BakgroundVol, optionHandler->GetOptionUnsignedInt("Audio", "bakgroundVolume", 50));
	audioManager->SetSoundVol(Audio::MasterVol, optionHandler->GetOptionUnsignedInt("Audio", "masterVolume", 100));

	//Set Camera
	size_t height = optionHandler->GetOptionUnsignedInt("Window", "height", 720);
	size_t width = optionHandler->GetOptionUnsignedInt("Window", "width", 1280);
	CameraBindInfoStruct camInfo;
	camInfo.aspectRatio = optionHandler->GetOptionDouble("Camera", "aspectRatio", (width / height));
	camInfo.fov = optionHandler->GetOptionDouble("Camera", "fov", 1.570796);
	camInfo.nearPlane = optionHandler->GetOptionDouble("Camera", "nearPlane", 0.01);
	camInfo.farPlance = optionHandler->GetOptionDouble("Camera", "farPlance", 100.0);
	cameraManager->UpdateCamera(camInfo);

	//Set Window and graphic
	bool sizeChange = window->SetWindow(height, width, optionHandler->GetOptionBool("Window", "fullScreen", false));

	if (sizeChange == true)
	{
		renderer->ResizeSwapChain(window->GetHWND());
		ImGuiDX11SDL_Shutdown();
		ImGuiDX11SDL_Init(renderer, window);
	}
	
	ProfileReturnVoid;
}