#include "WindowTest.h"


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
	window->MapKeyToMouse(5, SE::Window::MouseButtonMiddle);
	window->MapKeyToMouse(5, SE::Window::MouseButtonRight);

	//set gameloop
	while (window->HandleMSG() == true && window->GetActionKeyState(0) == SE::Window::key_up)
	{
		//check keys
		if (window->GetActionKeyState(1) == SE::Window::key_pressed)
		{
			std::cout << "You pressed w" << std::endl;
		}
		else if(window->GetActionKeyState(1) == SE::Window::key_down)
		{
			std::cout << "You are holding w" << std::endl;
		}
		if (window->GetActionKeyState(2) == SE::Window::key_pressed)
		{
			std::cout << "You pressed a" << std::endl;
		}
		else if (window->GetActionKeyState(2) == SE::Window::key_down)
		{
			std::cout << "You are holding a" << std::endl;
		}
		if (window->GetActionKeyState(3) == SE::Window::key_pressed)
		{
			std::cout << "You pressed d" << std::endl;
		}
		else if (window->GetActionKeyState(3) == SE::Window::key_down)
		{
			std::cout << "You are holding d" << std::endl;
		}
		if (window->GetActionKeyState(4) == SE::Window::key_pressed)
		{
			std::cout << "You pressed s" << std::endl;
		}
		else if (window->GetActionKeyState(4) == SE::Window::key_down)
		{
			std::cout << "You are holding s" << std::endl;
		}

		//check mouse
		if (window->GetActionKeyState(5) == SE::Window::key_pressed)
		{
			std::cout << "You pressed left mouse button" << std::endl;
		}
		else if (window->GetActionKeyState(5) == SE::Window::key_down)
		{
			std::cout << "You are holding left mouse button" << std::endl;
		}
		if (window->GetActionKeyState(6) == SE::Window::key_pressed)
		{
			std::cout << "You pressed middle mouse button" << std::endl;
		}
		else if (window->GetActionKeyState(6) == SE::Window::key_down)
		{
			std::cout << "You are holding middle mouse button" << std::endl;
		}
		if (window->GetActionKeyState(7) == SE::Window::key_pressed)
		{
			std::cout << "You pressed right mouse button" << std::endl;
		}
		else if (window->GetActionKeyState(7) == SE::Window::key_down)
		{
			std::cout << "You are holding right mouse button" << std::endl;
		}
	}
}