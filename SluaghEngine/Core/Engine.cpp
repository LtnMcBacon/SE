#include "Engine.h"
#include <Profiler.h>
#include <Utilz\CPUTimeCluster.h>
#include <Utilz\Memory.h>
using namespace SE::Utilz::Memory;
#include "EntityManager.h"

//#include <Imgui\imgui.h>
//#include <ImGuiDX11SDL\ImGuiDX11SDL.h>
//

#ifdef _DEBUG
#pragma comment(lib, "ResourceHandlerD.lib")
#pragma comment(lib, "GraphicsD.lib")
#pragma comment(lib, "WindowD.lib")
#pragma comment(lib, "ImGuiDX11SDLD.lib")
#else
#pragma comment(lib, "ResourceHandler.lib")
#pragma comment(lib, "Graphics.lib")
#pragma comment(lib, "Window.lib")
#pragma comment(lib, "ImGuiDX11SDL.lib");
#endif


int SE::Core::Engine::Init(const InitializationInfo& info)
{
	StartProfile;
	subSystems = info.subSystems;
	managers = info.managers;
	
	InitSubSystems();
	InitManagers();


	cameraManager = new CameraManager(renderer, *entityManager, transformManager);
	renderableManager = new RenderableManager({ resourceHandler, renderer, *entityManager, transformManager });
	particleSystemManager = new ParticleSystemManager({ renderer, resourceHandler, *entityManager, *transformManager });
	animationManager = new AnimationManager(renderer, resourceHandler, *entityManager, renderableManager);
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

	timeClus = new Utilz::CPUTimeCluster;


	ProfileReturnConst(0);
}

int SE::Core::Engine::BeginFrame()
{
	StartProfile;
	if (frameBegun)
		ProfileReturnConst( -1);
	frameBegun = true;
	timeClus->Start("Frame");
	window->Frame();
	ImGuiDX11SDL_NewFrame();
	renderer->BeginFrame();
	ProfileReturnConst(0);
}

int SE::Core::Engine::Frame()
{
	StartProfile;
	if (!frameBegun)
		BeginFrame();
	
	for (auto& m : managersVec)
		m->Frame(&timeClus);
	//ImGui::Render();
	
	
	initInfo.renderer->Render();
	initInfo.renderer->EndFrame();
	perFrameStackAllocator->ClearStackAlloc();
	frameBegun = false;
	ProfileReturnConst(0);
}

int SE::Core::Engine::Release()
{
	StartProfile;

	delete timeClus;

	delete devConsole;
	ImGuiDX11SDL_Shutdown();
	
	renderer->Shutdown();
	window->Shutdown();
	audioManager->Shutdown();
	resourceHandler->Shutdown();
	optionHandler->UnloadOption("Config.ini");

	delete cameraManager;
	delete collisionManager;
	delete materialManager;
	delete particleSystemManager;
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
	timeClus = nullptr;
}

SE::Core::Engine::~Engine()
{
	

}

void SE::Core::Engine::InitSubSystems()
{
	if (!subSystems.optionsHandler)
	{
		subSystems.optionsHandler = CreateOptionsHandler();
		subSystems.optionsHandler->Initialize("Config.ini");
	}
	if (!subSystems.resourceHandler)
	{
		subSystems.resourceHandler = ResourceHandler::CreateResourceHandler();
		ResourceHandler::InitializationInfo info;
		info.maxMemory = subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxRAMUsage", 256_mb);
		info.unloadingStrat = ResourceHandler::UnloadingStrategy::Linear;
		subSystems.resourceHandler->Initialize(info);
	}
	if (!subSystems.window)
	{
		subSystems.window = Window::CreateNewWindow();
		Window::InitializationInfo info;
		info.fullScreen = false;
		info.width = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 800);
		info.height = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 640);
		info.windowTitle = "SluaghEngine";
		info.winState = Window::WindowState::Regular;
		subSystems.window->Initialize(info);
	}
	if (!subSystems.renderer)
	{
		subSystems.renderer = Graphics::CreateRenderer();
		Graphics::InitializationInfo info;
		info.window = subSystems.window->GetHWND();
		info.maxVRAMUsage = subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxVRAMUsage", 512_mb);
		subSystems.renderer->Initialize(info);
	}
}

void SE::Core::Engine::InitManagers()
{
	if (!managers.entityManager)
		managers.entityManager = CreateEntityManager();


	if (!managers.audioManager)
	{
		IAudioManager::InitializationInfo info;
		info.entityManager = managers.entityManager;
		info.resourceHandler = subSystems.resourceHandler;
		managers.audioManager = CreateAudioManager(info);
	}
	managersVec.push_back(managers.audioManager);


	if (!managers.transformManager)
	{
		ITransformManager::InitializationInfo info;
		info.entityManager = managers.entityManager;
		managers.transformManager = CreateTransformManager(info);
	}
	managersVec.push_back(managers.transformManager);
	
	if (!managers.collisionManager)
	{
		ICollisionManager::InitializationInfo info;
		info.resourceHandler = subSystems.resourceHandler;
		info.entityManager = managers.entityManager;
		info.transformManager = managers.transformManager;
		managers.collisionManager = CreateCollisionManger(info);
	}
	managersVec.push_back(managers.collisionManager);


	if (!managers.renderableManager)
	{
		IRenderableManager::InitializationInfo info;
		info.entityManager = managers.entityManager;
		info.transformManager = managers.transformManager;
		info.renderer = subSystems.renderer;
		info.resourceHandler = subSystems.resourceHandler;
		info.unloadingStrat = ResourceHandler::UnloadingStrategy::Linear;
		managers.renderableManager = CreateRenderableManager(info);

	}
	managersVec.push_back(managers.renderableManager);
}

void SE::Core::Engine::InitStartupOption()
{
	//Set Sound Vol
	audioManager->SetSoundVol(Audio::MasterVol, optionHandler->GetOptionUnsignedInt("Audio", "masterVolume", 5));
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
	audioManager->SetSoundVol(Audio::MasterVol, optionHandler->GetOptionUnsignedInt("Audio", "masterVolume", 5));

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
		guiManager->updateGUI();
	}
	
	ProfileReturnVoid;
}

void SE::Core::Engine::GatherErrors()
{
	auto& renderErrors = renderer->GetErrorLog();
	for(auto& err : renderErrors)
	{
		devConsole->Print(err, "Graphics Error");
	}
}
