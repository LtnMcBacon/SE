#include <IWindow.h>
#include "WindowSDL.h"

DECLDIR SE::Window::IWindow * SE::Window::CreateNewWindow()
{
	return new WindowSDL;
}
