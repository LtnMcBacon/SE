#include "InitGraphicsTest.h"
#include <Graphics\Renderer.h>
#include <Window\Window.h>

#ifdef _DEBUG
#pragma comment(lib, "GraphicsD.lib")
#pragma comment(lib, "WindowD.lib")
#else
#pragma comment(lib, "Graphics.lib")
#pragma comment(lib, "Window.lib")
#endif

SE::Test::InitGraphicsTest::InitGraphicsTest::InitGraphicsTest()
{
}


SE::Test::InitGraphicsTest::InitGraphicsTest::~InitGraphicsTest()
{
}

bool SE::Test::InitGraphicsTest::Run(Utilz::IConsoleBackend * console)
{
	Graphics::IRenderer* r = new Graphics::Renderer();
	Window::InterfaceWindow* w = new Window::Window();
	int result = w->Initialise();
	if (result)
	{
		console->Print("Could not initialize window. Error: %d\n", result);
		return false;
	}
	result = r->Initialize(w->GethWnd());
	if (result)
	{
		console->Print("Could not initialize renderer. Error: %d\n", result);
		return false;
	}
	r->Shutdown();

	w->Shutdown();
	return true;
}
