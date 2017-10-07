#ifndef SE_IMGUI_DX11_SDL_H
#define SE_IMGUI_DX11_SDL_H

//Based on examples from https://github.com/ocornut/imgui

struct ID3D11Device;
struct ID3D11DeviceContext;
struct SDL_Window;
typedef union SDL_Event SDL_Event;

#include <Window\IWindow.h>
#include <Graphics\IRenderer.h>

IMGUI_API bool        ImGuiDX11SDL_Init(SE::Graphics::IRenderer* renderer, SE::Window::IWindow* window);
IMGUI_API bool		  ImGuiDX11SDL_ProcessEvent(SDL_Event* event);
IMGUI_API void        ImGuiDX11SDL_Shutdown();
IMGUI_API void        ImGuiDX11SDL_NewFrame();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplDX11_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplDX11_CreateDeviceObjects();

#endif //SE_IMGUI_DX11_SDL_H
