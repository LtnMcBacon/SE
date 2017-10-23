#include <IEngine.h>
#include "Engine.h"
#include "ImGuiConsole.h"

DECLDIR_CORE SE::Core::IEngine * SE::Core::CreateEngine()
{
	return new Engine;
}

DECLDIR_CORE SE::DevConsole::IConsole * SE::Core::CreateConsole(Graphics::IRenderer* renderer, Window::IWindow* window)
{
	return new ImGuiConsole(renderer, window);
}
