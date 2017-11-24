#include "Engine.h"
#include <Profiler.h>
#include <Utilz\Memory.h>
#include "ImGuiConsole.h"
using namespace SE::Utilz::Memory;



#ifdef _DEBUG
#pragma comment(lib, "ResourceHandlerD.lib")
#pragma comment(lib, "GraphicsD.lib")
#pragma comment(lib, "WindowD.lib")
#pragma comment(lib, "ImGuiDX11SDLD.lib")
#pragma comment(lib, "DevConsoleD.lib")
#else
#pragma comment(lib, "ResourceHandler.lib")
#pragma comment(lib, "Graphics.lib")
#pragma comment(lib, "Window.lib")
#pragma comment(lib, "ImGuiDX11SDL.lib")
#pragma comment(lib, "DevConsole.lib")
#endif

#ifdef __linux__ 
//linux code goes here
#elif _WIN32 || _WIN64
#include <Windows.h>
#else
#endif
int SE::Core::Engine::Init(const InitializationInfo& info)
{
	StartProfile;
	subSystems = info.subSystems;
	managers = info.managers;
	
	perFrameStackAllocator = new Utilz::StackAllocator;
	perFrameStackAllocator->InitStackAlloc(1024U * 1024U * 5U);



	try
	{
		InitSubSystems();
		InitManagers();
	}
	catch (const std::exception& e)
	{
		// A manager or sub system could not be initiated.
		printf("Error in engine init: %s", e.what());

#ifdef _WIN32 || _WIN64
		int msgboxID = MessageBox(
			NULL,
			e.what(),
			"Engine Init Error",
			MB_ICONERROR | MB_OK
		);
#endif
		this->Release();
		ProfileReturnConst(-1);
	}

	SetupDebugConsole();

	//default cameradamera
	auto defEntCam = managers.entityManager->Create();
	managers.cameraManager->Create(defEntCam);
	managers.cameraManager->SetActive(defEntCam);

	InitStartupOption();

	subSystems.devConsole->BeginFrame();
	subSystems.devConsole->EndFrame();
	ProfileReturnConst(0);
}

int SE::Core::Engine::BeginFrame()
{
	StartProfile;
	static int calls = 0;
	if (frameBegun)
		ProfileReturnConst( -1);

	frameBegun = true;
	timeClus.Start(("Frame"));
	subSystems.window->Frame();


	
	subSystems.renderer->BeginFrame();
	subSystems.devConsole->BeginFrame();

	for (auto& m : managersVec)
		m->Frame(&timeClus);

	calls++;
	subSystems.renderer->Render();
	subSystems.devConsole->Frame();

	ProfileReturnConst(0);
}


int SE::Core::Engine::EndFrame()
{
	StartProfile;
	if (!frameBegun)
		ProfileReturnConst(-1);

	subSystems.devConsole->EndFrame();
	subSystems.renderer->EndFrame();
	
	std::vector<std::string> errors;
	subSystems.resourceHandler->GetErrorMessages(errors);
	for (auto& r : errors)
		subSystems.devConsole->PrintChannel("ResourceHandler", "%s", r.c_str());

	timeClus.Stop(("Frame"));
	perFrameStackAllocator->ClearStackAlloc();
	frameBegun = false;
	ProfileReturnConst(0);
}

int SE::Core::Engine::Release()
{
	StartProfile;

	if (subSystems.resourceHandler)
		subSystems.resourceHandler->Shutdown();
	delete subSystems.resourceHandler;
	subSystems.resourceHandler = nullptr;

	for (auto rit = managersVec.rbegin(); rit != managersVec.rend(); ++rit)
		delete *rit;

	delete managers.entityManager;

	if(subSystems.devConsole)
		subSystems.devConsole->Shutdown();
	delete subSystems.devConsole;
	subSystems.devConsole = nullptr;

	if (subSystems.renderer)
		subSystems.renderer->Shutdown();
	delete subSystems.renderer;
	subSystems.renderer = nullptr;

	if (subSystems.window)
		subSystems.window->Shutdown();
	delete subSystems.window;
	subSystems.window = nullptr;

	

	if (subSystems.optionsHandler)
		subSystems.optionsHandler->UnloadOption("Config.ini");
	delete subSystems.optionsHandler;
	subSystems.optionsHandler = nullptr;

	if (subSystems.threadPool)
		delete subSystems.threadPool;
	subSystems.threadPool = nullptr;

	delete perFrameStackAllocator;
	ProfileReturnConst(0);
}

SE::Core::Engine::Engine() : perFrameStackAllocator(nullptr)
{
}

SE::Core::Engine::~Engine()
{
}

void SE::Core::Engine::InitSubSystems()
{
	StartProfile;
	if (!subSystems.optionsHandler)
	{
		subSystems.optionsHandler = CreateOptionsHandler();
		auto res = subSystems.optionsHandler->Initialize("Config.ini");
		if (res < 0)
			throw std::exception("Could not initiate optionsHandler. Something went wrong with the Config.ini file");
	}
	if (!subSystems.resourceHandler)
	{
		subSystems.resourceHandler = ResourceHandler::CreateResourceHandler();
		ResourceHandler::InitializationInfo info;
		info.RAM.max = subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxRAMUsage", 256_mb);
		info.RAM.tryUnloadWhenOver = 0.5;
		info.RAM.getCurrentMemoryUsage = []() { return Utilz::Memory::GetVirtualProcessMemory(); };
		info.VRAM.max = subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxVRAMUsage", 512_mb);
		info.VRAM.tryUnloadWhenOver = 0.5;
		info.VRAM.getCurrentMemoryUsage = [this]() {return subSystems.renderer->GetVRam(); };
		auto res = subSystems.resourceHandler->Initialize(info);
		if (res < 0)
			throw std::exception("Could not initiate resourceHandler. Make sure you have run the fileparser. And in the right folder,etc.");
	}
	if (!subSystems.window)
	{
		subSystems.window = Window::CreateNewWindow();
		Window::InitializationInfo info;
		info.fullScreen = false;
		info.width = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 1280);
		info.height = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 720);
		info.windowTitle = "SluaghEngine";
		info.winState = Window::WindowState::Regular;
		auto res = subSystems.window->Initialize(info);
		if (res < 0)
			throw std::exception("Could not initiate window.");
	}
	if (!subSystems.renderer)
	{
		subSystems.renderer = Graphics::CreateRenderer();
		Graphics::InitializationInfo info;
		info.window = subSystems.window->GetHWND();
		info.maxVRAMUsage = subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxVRAMUsage", 512_mb);
		auto res = subSystems.renderer->Initialize(info);
		if (res < 0)
			throw std::exception("Could not initiate renderer.");
	}
	if (!subSystems.devConsole)
	{
		subSystems.devConsole = CreateConsole(subSystems.renderer, subSystems.window);
		auto res = subSystems.devConsole->Initialize();
		if (!res)
			throw std::exception("Could not initiate devConsole.");
	}
	if(!subSystems.threadPool)
	{
		subSystems.threadPool = new Utilz::ThreadPool(4);
	}
	StopProfile;
}

void SE::Core::Engine::InitManagers()
{
	StartProfile;
	if (!managers.entityManager)
		managers.entityManager = CreateEntityManager();

	InitDataManager();

	if (!managers.eventManager)
		managers.eventManager = CreateEventManager({ subSystems.window, managers.entityManager, managers.dataManager });
	managersVec.push_back(managers.eventManager);

	InitTransformManager();
	InitAudioManager();
	InitParticleSystemManager();
	InitCameraManager();
	InitCollisionManager();
	InitRenderableManager();
	InitAnimationManager();
	InitMaterialManager();
	InitLightManager();
	InitDebugRenderManager();
	InitTextManager();
	InitGUIManager();
	InitDecalManager();

	StopProfile;
}

void SE::Core::Engine::InitAudioManager()
{
	if (!managers.audioManager)
	{
		IAudioManager::InitializationInfo info;
		info.entityManager = managers.entityManager;
		info.transformManager = managers.transformManager;
		info.resourceHandler = subSystems.resourceHandler;
		info.console = subSystems.devConsole;
		managers.audioManager = CreateAudioManager(info);
	}
	managersVec.push_back(managers.audioManager);

}

void SE::Core::Engine::InitTransformManager()
{
	if (!managers.transformManager)
	{
		ITransformManager::InitializationInfo info;
		info.entityManager = managers.entityManager;
		info.threadPool = subSystems.threadPool;
		managers.transformManager = CreateTransformManager(info);
	}
	managersVec.push_back(managers.transformManager);

}

void SE::Core::Engine::InitParticleSystemManager()
{
	if (!managers.particleSystemManager)
	{
		IParticleSystemManager::InitializationInfo info;
		info.renderer = subSystems.renderer;
		info.resourceHandler = subSystems.resourceHandler;
		info.entityManager = managers.entityManager;
		info.transformManager = managers.transformManager;
		info.console = subSystems.devConsole;
		info.eventManager = managers.eventManager;
		managers.particleSystemManager = CreateParticleSystemManager(info);
	}
	managersVec.push_back(managers.particleSystemManager);
}

void SE::Core::Engine::InitCameraManager()
{
	if (!managers.cameraManager)
	{
		ICameraManager::InitializationInfo info;
		info.renderer = subSystems.renderer;
		info.entityManager = managers.entityManager;
		info.transformManager = managers.transformManager;
		managers.cameraManager = CreateCameraManager(info);
	}
	managersVec.push_back(managers.cameraManager);
}

void SE::Core::Engine::InitCollisionManager()
{
	if (!managers.collisionManager)
	{
		ICollisionManager::InitializationInfo info;
		info.resourceHandler = subSystems.resourceHandler;
		info.entityManager = managers.entityManager;
		info.transformManager = managers.transformManager;
		info.console = subSystems.devConsole;
		managers.collisionManager = CreateCollisionManger(info);
	}
	managersVec.push_back(managers.collisionManager);

}

void SE::Core::Engine::InitRenderableManager()
{
	if (!managers.renderableManager)
	{
		IRenderableManager::InitializationInfo info;
		info.entityManager = managers.entityManager;
		info.eventManager = managers.eventManager;
		info.transformManager = managers.transformManager;
		info.renderer = subSystems.renderer;
		info.resourceHandler = subSystems.resourceHandler;
		info.console = subSystems.devConsole;
		managers.renderableManager = CreateRenderableManager(info);

	}
	managersVec.push_back(managers.renderableManager);
}

void SE::Core::Engine::InitAnimationManager()
{
	if (!managers.animationManager)
	{
		IAnimationManager::InitializationInfo info;
		info.renderer = subSystems.renderer;
		info.resourceHandler = subSystems.resourceHandler;
		info.window = subSystems.window;
		info.entityManager = managers.entityManager;
		info.eventManager = managers.eventManager;
		info.transformManager = managers.transformManager;
		info.console = subSystems.devConsole;
		info.threadPool = subSystems.threadPool;
		managers.animationManager = CreateAnimationManager(info);
	}
	managersVec.push_back(managers.animationManager);
}

void SE::Core::Engine::InitMaterialManager()
{
	if (!managers.materialManager)
	{
		IMaterialManager::InitializationInfo info;
		info.renderer = subSystems.renderer;
		info.optionsHandler = subSystems.optionsHandler;
		info.resourceHandler = subSystems.resourceHandler;
		info.entityManager = managers.entityManager;
		info.eventManager = managers.eventManager;
		info.console = subSystems.devConsole;
		managers.materialManager = CreateMaterialManager(info);
	}
	managersVec.push_back(managers.materialManager);
}

void SE::Core::Engine::InitLightManager()
{
	if (!managers.lightManager)
	{
		ILightManager::InitializationInfo info;
		info.renderer = subSystems.renderer;
		info.entityManager = managers.entityManager;
		info.transformManager = managers.transformManager;
		info.eventManager = managers.eventManager;
		managers.lightManager = CreateLightManager(info);
	}
	managersVec.push_back(managers.lightManager);
}

void SE::Core::Engine::InitDebugRenderManager()
{
	if (!managers.debugRenderManager)
	{
		IDebugRenderManager::InitializationInfo info;
		info.renderer = subSystems.renderer;
		info.resourceHandler = subSystems.resourceHandler;
		info.entityManager = managers.entityManager;
		info.transformManager = managers.transformManager;
		info.collisionManager = managers.collisionManager;
		info.perFrameStackAllocator = perFrameStackAllocator;
		managers.debugRenderManager = CreateDebugRenderManager(info);
	}
	managersVec.push_back(managers.debugRenderManager);
}

void SE::Core::Engine::InitTextManager()
{
	if (!managers.textManager)
	{
		ITextManager::InitializationInfo info;
		info.renderer = subSystems.renderer;
		info.resourceHandler = subSystems.resourceHandler;
		info.entityManager = managers.entityManager;
		managers.textManager = CreateTextManager(info);
	}
	managersVec.push_back(managers.textManager);
}

void SE::Core::Engine::InitGUIManager()
{
	if (!managers.guiManager)
	{
		IGUIManager::InitializationInfo info;
		info.renderer = subSystems.renderer;
		info.resourceHandler = subSystems.resourceHandler;
		info.entityManager = managers.entityManager;
		managers.guiManager = CreateGUIManager(info);
	}
	managersVec.push_back(managers.guiManager);
}

void SE::Core::Engine::InitDecalManager()
{
	if(!managers.decalManager)
	{
		IDecalManager::InitializationInfo info;
		info.transformManager = managers.transformManager;
		info.entityManager = managers.entityManager;
		info.renderer = subSystems.renderer;
		info.resourceHandler = subSystems.resourceHandler;
		info.cameraManager = managers.cameraManager;
		info.eventManager = managers.eventManager;
		managers.decalManager = CreateDecalManager(info);
	}
	managersVec.push_back(managers.decalManager);
}

void SE::Core::Engine::InitDataManager()
{
	if (!managers.dataManager)
	{
		IDataManager::InitializationInfo info;
		info.entityManager = managers.entityManager;
		managers.dataManager = CreateDataManager(info);
	}
	managersVec.push_back(managers.dataManager);
}

void SE::Core::Engine::SetupDebugConsole()
{
	subSystems.devConsole->AddFrameCallback([this]()
	{

		static bool plot_memory_usage;
		static bool show_gpu_timings;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Debugging"))
			{
				ImGui::MenuItem("Plot memory usage", nullptr, &plot_memory_usage);
				ImGui::MenuItem("Show frame timings", nullptr, &show_gpu_timings);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if(plot_memory_usage)
		{
		using namespace Utilz::Memory;
		static const int samples = 256;
		static float vram_usage[samples];
		static float ram_usage[samples];
		static int offset = 0;

		vram_usage[offset] = ((float)subSystems.renderer->GetVRam()) / (1024.0f * 1024.0f);
		ram_usage[offset] = ((float)Utilz::Memory::GetVirtualProcessMemory()) / (1024.0f * 1024.0f);
		offset = (offset + 1) % samples;
		ImGui::PlotLines("VRAM", vram_usage, samples, offset, nullptr, 0.0f, 512.0f, { 0, 80 });
		if (subSystems.renderer->GetVRam() >= subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxVRAMUsage", 512_mb))
		{
		ImGui::PushStyleColor(ImGuiCol_Text, { 0.8f, 0.0f, 0.0f , 1.0f});
		ImGui::TextUnformatted((std::string("To much VRAM USAGE!!!!!!!!!!!!! Max usage is ") + std::to_string(toMB(subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxVRAMUsage", 512_mb))) + "mb").c_str());
		ImGui::PopStyleColor();
		}
		ImGui::PlotLines("RAM", ram_usage, samples, offset, nullptr, 0.0f, 512.0f, { 0, 80 });
		if (!Utilz::Memory::IsUnderLimit(subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxRAMUsage", 512_mb)))
		{
		ImGui::PushStyleColor(ImGuiCol_Text, { 0.8f, 0.0f, 0.0f , 1.0f });
		ImGui::TextUnformatted((std::string("To much RAM USAGE!!!!!!!!!!!!! Max usage is ") + std::to_string(toMB(subSystems.optionsHandler->GetOptionUnsignedInt("Memory", "MaxRAMUsage", 512_mb))) + "mb").c_str());
		ImGui::PopStyleColor();
		}
		ImGui::Separator();
		}

		if (show_gpu_timings)
		{
			SE::Utilz::TimeMap map;
			this->GetProfilingInformation(map);
			static float maxFrameTime = 0.0f;
			static float minFrameTime = 999999999.0f;
			static float avg100Frames = 0.0f;
			const auto frame = map.find(("Frame"));
			if (frame != map.end())
			{
				static float runningSum = 0.0f;
				runningSum += frame->second;
				if (frame->second < minFrameTime)
					minFrameTime = frame->second;
				if (frame->second > maxFrameTime)
					maxFrameTime = frame->second;
				static size_t frameCounter = 0;
				if (frameCounter >= 100)
				{
					avg100Frames = runningSum / (float)frameCounter;
					frameCounter = 0;
					runningSum = 0.0f;
				}
				frameCounter++;
			}
			ImGui::TextUnformatted("Avg frame time:"); ImGui::SameLine(0, 10); ImGui::TextUnformatted(std::to_string(avg100Frames).c_str());
			ImGui::TextUnformatted("Min frame time:"); ImGui::SameLine(0, 10); ImGui::TextUnformatted(std::to_string(minFrameTime).c_str());
			ImGui::TextUnformatted("Max frame time:"); ImGui::SameLine(0, 10); ImGui::TextUnformatted(std::to_string(maxFrameTime).c_str());
			for (auto& m : map)
			{
				ImGui::TextUnformatted(m.first); ImGui::SameLine(0, 10); ImGui::TextUnformatted(std::to_string(m.second).c_str()); ImGui::SameLine(); ImGui::TextUnformatted("ms");
			}


		}
	});
	
}

void SE::Core::Engine::InitStartupOption()
{
	StartProfile;
	//Set Sound Vol
	managers.audioManager->SetSoundVol(Audio::MasterVol, subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "masterVolume", 5));
	managers.audioManager->SetSoundVol(Audio::EffectVol, subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "effectVolume", 80));
	managers.audioManager->SetSoundVol(Audio::VoiceVol, subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "voiceVolume", 10));
	managers.audioManager->SetSoundVol(Audio::BakgroundVol, subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "bakgroundVolume", 50));

	//Set Camera
	size_t height = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 720);
	size_t width = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 1280);
	ICameraManager::CreateInfo camInfo;
	camInfo.aspectRatio = subSystems.optionsHandler->GetOptionDouble("Camera", "aspectRatio", (static_cast<float>(width) / static_cast<float>(height)));
	camInfo.fov = subSystems.optionsHandler->GetOptionDouble("Camera", "fov", 1.570796);
	camInfo.nearPlane = subSystems.optionsHandler->GetOptionDouble("Camera", "nearPlane", 0.01);
	camInfo.farPlance = subSystems.optionsHandler->GetOptionDouble("Camera", "farPlance", 100.0);
	managers.cameraManager->UpdateCamera(camInfo);

	//Set Window and graphic
	bool sizeChange = subSystems.window->SetWindow(height, width, subSystems.optionsHandler->GetOptionBool("Window", "fullScreen", false));

	if (sizeChange == true)
	{
		subSystems.renderer->ResizeSwapChain(subSystems.window->GetHWND());
	}

	subSystems.optionsHandler->Register({ this, &Engine::OptionUpdate });
	ProfileReturnVoid;
}

void SE::Core::Engine::OptionUpdate()
{
	StartProfile;
	//Set Sound Vol
	managers.audioManager->SetSoundVol(Audio::MasterVol, subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "masterVolume", 5));
	managers.audioManager->SetSoundVol(Audio::EffectVol, subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "effectVolume", 80));
	managers.audioManager->SetSoundVol(Audio::VoiceVol, subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "voiceVolume", 10));
	managers.audioManager->SetSoundVol(Audio::BakgroundVol, subSystems.optionsHandler->GetOptionUnsignedInt("Audio", "bakgroundVolume", 50));

	//Set Camera
	size_t height = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 720);
	size_t width = subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 1280);
	ICameraManager::CreateInfo camInfo;
	subSystems.optionsHandler->SetOptionDouble("Camera", "aspectRatio", (static_cast<float>(width) / static_cast<float>(height)));
	camInfo.aspectRatio = subSystems.optionsHandler->GetOptionDouble("Camera", "aspectRatio", (static_cast<float>(width) / static_cast<float>(height)));
	camInfo.fov = subSystems.optionsHandler->GetOptionDouble("Camera", "fov", 1.570796);
	camInfo.nearPlane = subSystems.optionsHandler->GetOptionDouble("Camera", "nearPlane", 0.01);
	camInfo.farPlance = subSystems.optionsHandler->GetOptionDouble("Camera", "farPlance", 100.0);
	managers.cameraManager->UpdateCamera(camInfo);


	//Set Window and graphic
	bool sizeChange = subSystems.window->SetWindow(height, width, subSystems.optionsHandler->GetOptionBool("Window", "fullScreen", false));

	if (sizeChange == true)
	{
		subSystems.renderer->ResizeSwapChain(subSystems.window->GetHWND());
	//	ImGuiDX11SDL_Shutdown();
	//	ImGuiDX11SDL_Init(subSystems.renderer, subSystems.window);
		managers.guiManager->updateGUI();
		managers.textManager->updateText();
	}
	
	ProfileReturnVoid;
}

void SE::Core::Engine::GatherErrors()
{
	auto& renderErrors = subSystems.renderer->GetErrorLog();
	for(auto& err : renderErrors)
	{
		//devConsole->Print(err, "Graphics Error");
	}
}
