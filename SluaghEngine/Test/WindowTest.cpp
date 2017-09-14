#include "WindowTest.h"
#include <window/Window.h>
#include <window/InterfaceWindow.h>

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
	SE::Window::InterfaceWindow* window = new SE::Window::Window();
	
	//initiate window (display and input)
	window->Initialise();

	//bind keys
	window->MapKeyToKeyboard(0, SE::Window::KeyEscape);
	window->MapKeyToKeyboard(1, SE::Window::KeyW);
	window->MapKeyToKeyboard(2, SE::Window::KeyA);
	window->MapKeyToKeyboard(3, SE::Window::KeyD);
	window->MapKeyToKeyboard(4, SE::Window::KeyS);

	//bind mouse
	window->MapKeyToMouse(5, SE::Window::MouseButtonLeft);
	window->MapKeyToMouse(6, SE::Window::MouseButtonMiddle);
	window->MapKeyToMouse(7, SE::Window::MouseButtonRight);

	//set gameloop
	while (window->HandleMSG() == true && window->GetActionKeyState(0) == SE::Window::key_isUp)
	{	
		//check key
		if (window->GetActionKeyState(1) == SE::Window::key_isPressed)
		{
			console->Print("You pressed w \n");
		}
		if (window->GetActionKeyState(2) == SE::Window::key_isPressed)
		{
			console->Print("You pressed a \n");
		}
		if (window->GetActionKeyState(3) == SE::Window::key_isPressed)
		{
			console->Print("You pressed d \n");
		}
		if (window->GetActionKeyState(4) == SE::Window::key_isPressed)
		{
			console->Print("You pressed s \n");
		}

		//check mouse
		if (window->GetActionKeyState(5) == SE::Window::key_isPressed)
		{
			console->Print("You pressed left mouse button \n");
		}
		if (window->GetActionKeyState(6) == SE::Window::key_isPressed)
		{
			console->Print("You pressed middle mouse button \n");
		}
		if (window->GetActionKeyState(7) == SE::Window::key_isPressed)
		{
			console->Print("You pressed right mouse button \n");
		}
	}
	window->Shutdown();
	delete window;
	return true;
}