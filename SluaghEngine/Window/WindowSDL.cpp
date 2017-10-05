#include "WindowSDL.h"
#include <SDL2/SDL_syswm.h>
#include <exception>
#include <Profiler.h>


#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")



SE::Window::WindowSDL::WindowSDL() : window(nullptr), width(1280), height(720), fullScreen(false), windowTitle(""), hwnd(nullptr), curMouseX(0), curMouseY(0), relMouseX(0), relMouseY(0)
{

}

SE::Window::WindowSDL::~WindowSDL()
{
}

int SE::Window::WindowSDL::Initialize(const InitializationInfo& info)
{
	StartProfile;
	width = info.width;
	height = info.height;
	fullScreen = info.fullScreen;
	windowTitle = info.windowTitle;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw std::exception("Failed to initialize SDL subsystem");
	uint32_t createFlags = SDL_WINDOW_SHOWN | (fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, createFlags);
	if (window == nullptr)
		throw std::exception("Failed to create window.");

	SDL_SysWMinfo sdlInfo;
	SDL_VERSION(&sdlInfo.version);
	SDL_GetWindowWMInfo(window, &sdlInfo);
	hwnd = sdlInfo.info.win.window;

	/* Annoying to type abstraction of SDL... */
	keyMapping = {
		{ KeyCode::Key0, SDLK_0},
		{ KeyCode::Key1, SDLK_1 },
		{ KeyCode::Key2, SDLK_2 },
		{ KeyCode::Key3, SDLK_3 },
		{ KeyCode::Key4, SDLK_4 },
		{ KeyCode::Key5, SDLK_5 },
		{ KeyCode::Key6, SDLK_6 },
		{ KeyCode::Key7, SDLK_7 },
		{ KeyCode::Key8, SDLK_8 },
		{ KeyCode::Key9, SDLK_9 },
		{ KeyCode::KeyA, SDLK_a },
		{ KeyCode::KeyB, SDLK_b },
		{ KeyCode::KeyC, SDLK_c },
		{ KeyCode::KeyD, SDLK_d },
		{ KeyCode::KeyE, SDLK_e },
		{ KeyCode::KeyF, SDLK_f },
		{ KeyCode::KeyG, SDLK_g },
		{ KeyCode::KeyH, SDLK_h },
		{ KeyCode::KeyI, SDLK_i },
		{ KeyCode::KeyJ, SDLK_j },
		{ KeyCode::KeyK, SDLK_k },
		{ KeyCode::KeyL, SDLK_l },
		{ KeyCode::KeyM, SDLK_m },
		{ KeyCode::KeyN, SDLK_n },
		{ KeyCode::KeyO, SDLK_o },
		{ KeyCode::KeyP, SDLK_p },
		{ KeyCode::KeyQ, SDLK_q },
		{ KeyCode::KeyR, SDLK_r },
		{ KeyCode::KeyS, SDLK_s },
		{ KeyCode::KeyT, SDLK_t },
		{ KeyCode::KeyU, SDLK_u },
		{ KeyCode::KeyV, SDLK_v },
		{ KeyCode::KeyW, SDLK_w },
		{ KeyCode::KeyX, SDLK_x },
		{ KeyCode::KeyY, SDLK_y },
		{ KeyCode::KeyEscape, SDLK_ESCAPE},
		{ KeyCode::KeyF1, SDLK_F1 },
		{ KeyCode::KeyF2, SDLK_F2 },
		{ KeyCode::KeyF3, SDLK_F3 },
		{ KeyCode::KeyF4, SDLK_F4 },
		{ KeyCode::KeyF5, SDLK_F5 },
		{ KeyCode::KeyF6, SDLK_F6 },
		{ KeyCode::KeyF7, SDLK_F7},
		{ KeyCode::KeyF8, SDLK_F8 },
		{ KeyCode::KeyF9, SDLK_F9 },
		{ KeyCode::KeyF10, SDLK_F10 },
		{ KeyCode::KeyF11, SDLK_F11 },
		{ KeyCode::KeyF12, SDLK_F12 },
		{ KeyCode::KeySpace, SDLK_SPACE },
		{ KeyCode::KeyLeft, SDLK_LEFT},
		{ KeyCode::KeyRight, SDLK_RIGHT },
		{ KeyCode::KeyUp, SDLK_UP },
		{ KeyCode::KeyDown, SDLK_DOWN },
		{ KeyCode::KeyBackSpace, SDLK_BACKSPACE},
		{ KeyCode::KeyTab ,SDLK_TAB},
		{ KeyCode::KeyReturn, SDLK_RETURN},
		{ KeyCode::KeyCapsLock, SDLK_CAPSLOCK},
		{ KeyCode::KeyShiftL,   SDLK_LSHIFT},
		{ KeyCode::KeyCtrlL,	   SDLK_LCTRL},
		{ KeyCode::KeyAltL,	   SDLK_LALT},
		{ KeyCode::KeyAltR,	   SDLK_RALT},
		{ KeyCode::KeyCtrlR,	   SDLK_RCTRL},
		{ KeyCode::KeyShiftR,   SDLK_RSHIFT},
		{ KeyCode::MouseLeft, SDL_BUTTON_LEFT},
		{ KeyCode::MouseRight, SDL_BUTTON_RIGHT}
	};

	if (info.winState == WindowState::Regular)
	{
		actualFrame = decltype(actualFrame)::Make<WindowSDL, &SE::Window::WindowSDL::RegFrame>(this);
	}
	else if (info.winState == WindowState::Record)
	{
		StartRecording();
	}
	else if (info.winState == WindowState::Playback)
	{
		LoadRecording();
	}

	frame = 0;
	arrayPos = 0;
	ProfileReturnConst(0);
}

void SE::Window::WindowSDL::Shutdown()
{
	if (recording)
	{
		recording = false;
		recThread.join();
		recFile.close();
	}
	if (playback)
	{
		delete[] playbackData;
	}
	SDL_Quit();
}

void SE::Window::WindowSDL::Frame()
{
	actualFrame();
}

void SE::Window::WindowSDL::RegFrame()
{
	StartProfile;
	for(auto& ks : actionToKeyState)
	{
		ks.second = (ks.second & KeyState::DOWN);
	}
	static int evCount = 0;
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
		case SDL_KEYUP:
			{
				const auto state = keyToAction.find(ev.key.keysym.sym);
				if (state != keyToAction.end())
					actionToKeyState[state->second] = UP;
				break;
			}
		case SDL_KEYDOWN:
			{
				const auto state = keyToAction.find(ev.key.keysym.sym);
				if (state != keyToAction.end())
				{
					if (!(actionToKeyState[state->second] & DOWN))
					{
						actionToKeyState[state->second] = PRESSED;
						auto pressCallbacks = actionToKeyPressCallback.find(state->second);
						if(pressCallbacks != actionToKeyPressCallback.end())
						{
							for (auto& cb : pressCallbacks->second)
								cb();
						}
					}
					auto downCallbacks = actionToKeyDownCallback.find(state->second);
					if(downCallbacks != actionToKeyDownCallback.end())
					{
						for (auto& cb : downCallbacks->second)
							cb();
					}
				}
				break;
			}
		case SDL_MOUSEMOTION:
			{
				curMouseX = ev.motion.x;
				curMouseY = ev.motion.y;
				relMouseX = ev.motion.xrel;
				relMouseY = ev.motion.yrel;
				for (auto& cb : mouseMotionCallbacks)
					cb(relMouseX, relMouseY, curMouseX, curMouseY);
				break;
			}
		case SDL_MOUSEBUTTONDOWN:
			{
				if(ev.button.button == SDL_BUTTON_LEFT)
				{	
					
					mouseLeftDown = true;
				}
				else if(ev.button.button == SDL_BUTTON_RIGHT)
				{	
					mouseRightDown = true;
				}
				const auto state = keyToAction.find(ev.button.button);
				if (state != keyToAction.end())
				{
					if (!(actionToKeyState[state->second] & DOWN))
					{
						actionToKeyState[state->second] = PRESSED;
					}
				}
				break;
			}
		case SDL_MOUSEBUTTONUP:
			{
				const auto state = keyToAction.find(ev.button.button);
				if (state != keyToAction.end())
				{
					auto mouseClickCallbacks = actionToMouseClickCallback.find(state->second);
					if (mouseClickCallbacks != actionToMouseClickCallback.end())
					{
						for (auto& cb : mouseClickCallbacks->second)
							cb(curMouseX, curMouseY);
					}
					actionToKeyState[state->second] = UP;
				}
				if (ev.button.button == SDL_BUTTON_LEFT)
				{
					mouseLeftDown = false;
				}
				else if (ev.button.button == SDL_BUTTON_RIGHT)
				{
					mouseRightDown = false;
				}
				break;
			}
		default:
			{
			break;
			}
		}
	}
	StopProfile;
}

void SE::Window::WindowSDL::RecordFrame()
{
	StartProfile;
	for (auto& ks : actionToKeyState)
	{
		ks.second = (ks.second & KeyState::DOWN);
	}
	static int evCount = 0;
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
			case SDL_KEYUP:
			{
				const auto state = keyToAction.find(ev.key.keysym.sym);
				if (state != keyToAction.end())
					actionToKeyState[state->second] = UP;
				break;
			}
			case SDL_KEYDOWN:
			{
				const auto state = keyToAction.find(ev.key.keysym.sym);
				if (state != keyToAction.end())
				{
					if (!(actionToKeyState[state->second] & DOWN))
					{
						actionToKeyState[state->second] = PRESSED;
						auto pressCallbacks = actionToKeyPressCallback.find(state->second);
						if (pressCallbacks != actionToKeyPressCallback.end())
						{
							for (auto& cb : pressCallbacks->second)
								cb();
						}
					}
					auto downCallbacks = actionToKeyDownCallback.find(state->second);
					if (downCallbacks != actionToKeyDownCallback.end())
					{
						for (auto& cb : downCallbacks->second)
							cb();
					}
				}
				break;
			}
			case SDL_MOUSEMOTION:
			{
				curMouseX = ev.motion.x;
				curMouseY = ev.motion.y;
				relMouseX = ev.motion.xrel;
				relMouseY = ev.motion.yrel;
				for (auto& cb : mouseMotionCallbacks)
					cb(relMouseX, relMouseY, curMouseX, curMouseY);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (ev.button.button == SDL_BUTTON_LEFT)
				{

					mouseLeftDown = true;
				}
				else if (ev.button.button == SDL_BUTTON_RIGHT)
				{
					mouseRightDown = true;
				}
				const auto state = keyToAction.find(ev.button.button);
				if (state != keyToAction.end())
				{
					if (!(actionToKeyState[state->second] & DOWN))
					{
						actionToKeyState[state->second] = PRESSED;
					}
				}
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				const auto state = keyToAction.find(ev.button.button);
				if (state != keyToAction.end())
				{
					auto mouseClickCallbacks = actionToMouseClickCallback.find(state->second);
					if (mouseClickCallbacks != actionToMouseClickCallback.end())
					{
						for (auto& cb : mouseClickCallbacks->second)
							cb(curMouseX, curMouseY);
					}
					actionToKeyState[state->second] = UP;
				}
				if (ev.button.button == SDL_BUTTON_LEFT)
				{
					mouseLeftDown = false;
				}
				else if (ev.button.button == SDL_BUTTON_RIGHT)
				{
					mouseRightDown = false;
				}
				break;
			}
			default:
			{
				break;
			}
		}

		inputRecData inData;
		inData.recEvent = ev;
		inData.frame = frame;
		circFiFo.push(inData);
	}
	frame++;
	StopProfile;
}

void SE::Window::WindowSDL::PlaybackFrame()
{
	StartProfile;
	while (playbackData[arrayPos].frame == frame)
	{
		switch (playbackData[arrayPos].recEvent.type)
		{
			case SDL_KEYUP:
			{
				const auto state = keyToAction.find(playbackData[arrayPos].recEvent.key.keysym.sym);
				if (state != keyToAction.end())
					actionToKeyState[state->second] = UP;
				break;
			}
			case SDL_KEYDOWN:
			{
				const auto state = keyToAction.find(playbackData[arrayPos].recEvent.key.keysym.sym);
				if (state != keyToAction.end())
				{
					if (!(actionToKeyState[state->second] & DOWN))
					{
						actionToKeyState[state->second] = PRESSED;
						auto pressCallbacks = actionToKeyPressCallback.find(state->second);
						if (pressCallbacks != actionToKeyPressCallback.end())
						{
							for (auto& cb : pressCallbacks->second)
								cb();
						}
					}
					auto downCallbacks = actionToKeyDownCallback.find(state->second);
					if (downCallbacks != actionToKeyDownCallback.end())
					{
						for (auto& cb : downCallbacks->second)
							cb();
					}
				}
				break;
			}
			case SDL_MOUSEMOTION:
			{
				curMouseX = playbackData[arrayPos].recEvent.motion.x;
				curMouseY = playbackData[arrayPos].recEvent.motion.y;
				relMouseX = playbackData[arrayPos].recEvent.motion.xrel;
				relMouseY = playbackData[arrayPos].recEvent.motion.yrel;
				for (auto& cb : mouseMotionCallbacks)
					cb(relMouseX, relMouseY, curMouseX, curMouseY);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (playbackData[arrayPos].recEvent.button.button == SDL_BUTTON_LEFT)
				{

					mouseLeftDown = true;
				}
				else if (playbackData[arrayPos].recEvent.button.button == SDL_BUTTON_RIGHT)
				{
					mouseRightDown = true;
				}
				const auto state = keyToAction.find(playbackData[arrayPos].recEvent.button.button);
				if (state != keyToAction.end())
				{
					if (!(actionToKeyState[state->second] & DOWN))
					{
						actionToKeyState[state->second] = PRESSED;
					}
				}
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				const auto state = keyToAction.find(playbackData[arrayPos].recEvent.button.button);
				if (state != keyToAction.end())
				{
					auto mouseClickCallbacks = actionToMouseClickCallback.find(state->second);
					if (mouseClickCallbacks != actionToMouseClickCallback.end())
					{
						for (auto& cb : mouseClickCallbacks->second)
							cb(curMouseX, curMouseY);
					}
					actionToKeyState[state->second] = UP;
				}
				if (playbackData[arrayPos].recEvent.button.button == SDL_BUTTON_LEFT)
				{
					mouseLeftDown = false;
				}
				else if (playbackData[arrayPos].recEvent.button.button == SDL_BUTTON_RIGHT)
				{
					mouseRightDown = false;
				}
				break;
			}
			default:
			{
				break;
			}
		}
		arrayPos++;
	}
	frame++;
	StopProfile;
}

void SE::Window::WindowSDL::StartRecording()
{
	StartProfile;
	actualFrame = decltype(actualFrame)::Make<WindowSDL, &SE::Window::WindowSDL::RecordFrame>(this);
	recFile.open("Recording.bin", std::ios::out | std::ios::binary | std::ios::trunc);
	recording = true;
	recThread = std::thread (&Window::WindowSDL::RecordToFile, this);
	StopProfile;
}

void SE::Window::WindowSDL::LoadRecording()
{
	StartProfile;
	playbackfile.open("Recording.bin", std::ios::in | std::ios::binary | std::ios::ate);
	if (playbackfile.is_open())
	{
		std::streampos size = playbackfile.tellg();
		playbackData = (inputRecData*)operator new(size);
		playbackfile.seekg(0, std::ios::beg);
		playbackfile.read((char*)playbackData, size);
		playbackfile.close();
		playback = true;
		actualFrame = decltype(actualFrame)::Make<WindowSDL, &SE::Window::WindowSDL::PlaybackFrame>(this);
	}
	StopProfile;
}

void* SE::Window::WindowSDL::GetHWND()
{
	return static_cast<void*>(hwnd);
}

bool SE::Window::WindowSDL::ButtonDown(uint32_t actionButton) const
{
	return GetKeyState(actionButton) & DOWN;
}

bool SE::Window::WindowSDL::ButtonPressed(uint32_t actionButton) const
{
	return !(GetKeyState(actionButton) ^ PRESSED);
}

bool SE::Window::WindowSDL::ButtonUp(uint32_t actionButton) const
{
	return GetKeyState(actionButton) & UP;
}

void SE::Window::WindowSDL::GetMousePos(int& x, int& y) const
{
	x = curMouseX;
	y = curMouseY;
}

void SE::Window::WindowSDL::MapActionButton(uint32_t actionButton, KeyCode key)
{
	keyToAction[keyMapping[key]] = actionButton;
}

void SE::Window::WindowSDL::BindMouseClickCallback(uint32_t actionButton, const MouseClickCallback& callback)
{
	actionToMouseClickCallback[actionButton].push_back(callback);
}

void SE::Window::WindowSDL::BindMouseMotionCallback(const MouseMotionCallback& callback)
{
	mouseMotionCallbacks.push_back(callback);
}

void SE::Window::WindowSDL::BindKeyPressCallback(uint32_t actionButton, const KeyCallback& callback)
{
	actionToKeyPressCallback[actionButton].push_back(callback);
}

void SE::Window::WindowSDL::BindKeyDownCallback(uint32_t actionButton, const KeyCallback& callback)
{
	actionToKeyDownCallback[actionButton].push_back(callback);
}

void SE::Window::WindowSDL::BindKeyUpCallback(uint32_t actionButton, const KeyCallback& callback)
{
	actionToKeyUpCallback[actionButton].push_back(callback);
}

void SE::Window::WindowSDL::UnbindCallbacks()
{
	actionToKeyPressCallback.clear();
	actionToKeyDownCallback.clear();
	actionToKeyUpCallback.clear();
	actionToMouseClickCallback.clear();
	mouseMotionCallbacks.clear();
}


uint32_t SE::Window::WindowSDL::GetKeyState(uint32_t actionButton) const
{
	//Find which KeyCode actionbutton is mapped to
	const auto k = actionToKeyState.find(actionButton);
	if (k == actionToKeyState.end())
		return 0;
	return k->second;
}

bool SE::Window::WindowSDL::SetWindow(int inHeight, int inWidth, bool inFullscreen)
{
	StartProfile;
	bool changed = false;
	if (inHeight != height)
	{
		height = inHeight;
		changed = true;
	}
	if (inWidth != width)
	{
		width = inWidth;
		changed = true;
	}
	fullScreen = inFullscreen;
	uint32_t createFlags = SDL_WINDOW_SHOWN | (fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	SDL_SetWindowFullscreen(window, createFlags);

	if (changed == true)
		SDL_SetWindowSize(window, width, height);

	ProfileReturn(changed);
}

void SE::Window::WindowSDL::RecordToFile()
{
	while (recording || !circFiFo.wasEmpty())
	{
		while (!circFiFo.wasEmpty())
		{
			inputRecData evData;
			circFiFo.top(evData);
			void* var = &evData;
			recFile.write((char*)var, sizeof(inputRecData));
			circFiFo.pop();
		}
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(100ms);
	}
}