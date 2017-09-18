#include "WindowTest.h"
#include <window/WindowSDL.h>
#include <window/IWindow.h>

#ifdef _DEBUG
#pragma comment(lib, "WindowD.lib")
#else
#pragma comment(lib, "Window.lib")
#endif

//using namespace SE::Window;
using namespace SE::Test;

WindowTest::WindowTest()
{

}

WindowTest::~WindowTest()
{

}

bool WindowTest::Run(SE::Utilz::IConsoleBackend* console)
{
	//create a window pointer
	SE::Window::IWindow* window = new SE::Window::WindowSDL();
	
	//initiate window (display and input)
	window->Initialize();

	//bind keys
	window->MapActionButton(0, SE::Window::KeyEscape);
	window->MapActionButton(1, SE::Window::KeyW);
	window->MapActionButton(2, SE::Window::KeyA);
	window->MapActionButton(3, SE::Window::KeyD);
	window->MapActionButton(4, SE::Window::KeyS);

	bool running = true;
	while(running)
	{
		window->Frame();

		if (window->ButtonPressed(1))
			console->Print("Action button %d pressed\n", 1);
		if (window->ButtonPressed(0))
			running = false;
		if (window->ButtonDown(3))
			console->Print("Action button %d down.\n", 3);
	}

	
	
	window->Shutdown();
	delete window;
	return true;
}