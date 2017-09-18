#include "InitGraphicsTest.h"
#include <Graphics\Renderer.h>
#include <Window\Window.h>
#include <Profiler.h>

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
	StartProfile;
	Graphics::IRenderer* r = new Graphics::Renderer();
	Window::InterfaceWindow* w = new Window::Window();
	int result = w->Initialise();
	if (result)
	{
		console->Print("Could not initialize window. Error: %d\n", result);
		ProfileReturnConst( false);
	}
	result = r->Initialize(w->GethWnd());
	if (result)
	{
		console->Print("Could not initialize renderer. Error: %d\n", result);
		ProfileReturnConst(false);
  }

	for (UINT i = 0; i < 200; i++) {
		r->Render();
	}

	r->Shutdown();

	w->Shutdown();
	ProfileReturnConst(true);
}
