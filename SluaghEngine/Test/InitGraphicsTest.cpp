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
	bool result = w->Initialise();
	if (!result)
	{
		console->Print("Could not initialize window.\n");
		return false;
	}
	r->Initialize(w->GethWnd());

	r->Shutdown();

	w->Shutdown();
	return true;
}
