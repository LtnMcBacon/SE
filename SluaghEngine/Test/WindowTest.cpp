#include "WindowTest.h"
#include <window/IWindow.h>
#include <Graphics/IRenderer.h>
#include "Utilz/Console.h"
#include <Profiler.h>

#ifdef _DEBUG
#pragma comment(lib, "WindowD.lib")
#pragma comment(lib, "GraphicsD.lib")
#else
#pragma comment(lib, "Window.lib")
#pragma comment(lib, "Graphics.lib")
#endif


//using namespace SE::Window;
using namespace SE::Test;

WindowTest::WindowTest()
{

}

WindowTest::~WindowTest()
{

}

static void KeyCall()
{
	SE::Utilz::Console::Print("Callback called.\n");
}

static void MouseCall(int x, int y)
{
	SE::Utilz::Console::Print("Clicked at %d, %d.\n", x, y);
}

static void MouseMotionCall(int rx, int ry, int x, int y)
{
	SE::Utilz::Console::Print("Moved mouse %d, %d pixels.\n", rx, ry);
}

bool WindowTest::Run(SE::Utilz::IConsoleBackend* console)
{
	StartProfile;
	//create a window pointer
	SE::Window::IWindow* window = Window::CreateNewWindow();
	
	//initiate window (display and input)
	window->Initialize();

	//bind keys
	window->MapActionButton(0, SE::Window::KeyEscape);
	window->MapActionButton(1, SE::Window::KeyW);
	window->MapActionButton(2, SE::Window::KeyA);
	window->MapActionButton(3, SE::Window::KeyD);
	window->MapActionButton(1, SE::Window::KeyS);
	window->MapActionButton(7, Window::KeyCode::KeyU);
	window->MapActionButton(5, Window::KeyCode::MouseLeft);
	window->MapActionButton(6, Window::KeyCode::MouseRight);
	window->BindKeyPressCallback(2, &KeyCall);
	window->BindMouseClickCallback(5, &MouseCall);
	window->BindMouseMotionCallback(&MouseMotionCall);
	window->MapActionButton(8, SE::Window::KeyO);

	

	bool running = true;
	while(running)
	{
		window->Frame();
		if (window->ButtonDown(6))
			console->Print("Mouse right down\n");
		if (window->ButtonPressed(1))
			console->Print("Action button %d pressed\n", 1);
		if (window->ButtonPressed(0))
			running = false;
		if (window->ButtonDown(3))
			console->Print("Action button %d down.\n", 3);
		if (window->ButtonPressed(4))
			console->Print("Action button %d pressed\n", 4);
		if (window->ButtonPressed(7))
			window->UnbindCallbacks();
		if (window->ButtonPressed(8))
		{
			int x, y;
			window->GetMousePos(x, y);
			console->Print("Mouse at: %d, %d\n", x, y);
		}
	}

	
	Graphics::IRenderer* renderer = Graphics::CreateRenderer();
	renderer->Initialize({ window->GetHWND() });

	uint8_t* fakeTexture = new uint8_t[256 * 256 * 4];

	for(int i = 0; i < 256*256*4; i++)
	{
		fakeTexture[i] = 255;
	}

	Graphics::TextureDesc td;
	td.width = 256;
	td.height = 256;

	renderer->CreateTexture(fakeTexture, td);

	
	renderer->Shutdown();
	window->Shutdown();
	delete renderer;
	delete window;
	delete[] fakeTexture;
	ProfileReturnConst(true);
}