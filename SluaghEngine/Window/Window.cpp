#include "Window.h"
#include "Display.h"
#include "Input.h"
#include <Profiler.h>

namespace SE {
	namespace Window {
		Window::Window()
		{
			
		}

		Window::~Window()
		{
			
		}

		int Window::Initialise()
		{		
			display = new SE::Window::Display();
			input = new SE::Window::Input();

			input->InitInput();
			int initS = display->InitDisplay();
			if (initS)
				return initS;
			return initS;
		}

		bool Window::MapKeyToKeyboard(int actionKey, Key keyboardKey)
		{
			return input->MapKeyToKeyboard(actionKey, keyboardKey);
		}

		bool Window::MapKeyToMouse(int actionKey, MouseButton mouseButton)
		{
			return input->MapKeyToMouse(actionKey, mouseButton);
		}

		void* Window::GethWnd()
		{ 
			return display->GethWnd(); 
		}

		void Window::Shutdown()
		{
			delete display;
			delete input;
		}

		bool Window::HandleMSG()
		{ 
			StartProfile;
			MSG windowMessage;
			windowMessage.message = WM_NULL;
			while (windowMessage.message != WM_QUIT)
			{

				// Call every frame
				input->Update();

				if (PeekMessage(&windowMessage, NULL, NULL, NULL, PM_REMOVE)) {

					TranslateMessage(&windowMessage);
					DispatchMessage(&windowMessage);

					input->HandleMSG(windowMessage);
				}

				// If there are no messages to handle, the application will continue by running a frame
				else 
				{
					ProfileReturnConst(true);
				}
			}
			ProfileReturnConst(false);
		}

		keyState Window::GetActionKeyState(int actionKey)
		{
			return input->GetActionKeyState(actionKey);
		}

	}	//namespace Window
}	//namespace SE