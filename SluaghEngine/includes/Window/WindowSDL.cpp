#include "WindowSDL.h"
#include <SDL2/SDL_syswm.h>
#include <exception>
#include <Core\Engine.h>

SE::Window::WindowSDL::WindowSDL() : window(nullptr), width(1280), height(720), fullScreen(false), windowTitle(""), hwnd(nullptr), curMouseX(0), curMouseY(0), relMouseX(0), relMouseY(0)
{

}

SE::Window::WindowSDL::~WindowSDL()
{
}

int SE::Window::WindowSDL::Initialize(const InitializationInfo& info)
{
	//width = info.width;
	//height = info.height;
	auto& optHandler = Core::Engine::GetInstance().GetOptionHandler();
	width = optHandler.GetOption("Window", "width", 1280);
	height = optHandler.GetOption("Window", "height", 720);
	optHandler.Register(Utilz::Delegate<void()>::Make<WindowSDL, &WindowSDL::OptionUpdate>(this));
	fullScreen = info.fullScreen;
	windowTitle = info.windowTitle;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw std::exception("Failed to initialize SDL subsystem");
	window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
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

	

	return 0;
}

void SE::Window::WindowSDL::Shutdown()
{
	SDL_Quit();
}

void SE::Window::WindowSDL::Frame()
{
	for(auto& ks : actionToKeyState)
	{
		ks.second = (ks.second & KeyState::DOWN);
	}
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

void SE::Window::WindowSDL::OptionUpdate()
{
	auto& optHandler = Core::Engine::GetInstance().GetOptionHandler();
	width = optHandler.GetOption("Window", "width", 1280);
	height = optHandler.GetOption("Window", "height", 720);
	SDL_SetWindowSize(window, width, height);
}
