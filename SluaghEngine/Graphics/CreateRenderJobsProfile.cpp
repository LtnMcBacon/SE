#include <Window\IWindow.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Profiler.h>

#include "Renderer.h"


#pragma comment(lib, "Window.lib")

using namespace SE;


int main(int argc, char* argv[])
{
	StartProfile;
	
	auto rh = ResourceHandler::CreateResourceHandler();
	auto r = new Graphics::Renderer;
	auto w = Window::CreateNewWindow();
	w->Initialize(Window::InitializationInfo());
	r->Initialize(w->GetHWND());




	r->Render();


	r->Shutdown();
	w->Shutdown();
	rh->Shutdown();

	delete r;
	StopProfile;
	return 0;
}