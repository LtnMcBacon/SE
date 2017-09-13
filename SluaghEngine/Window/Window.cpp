#include "Window.h"
#include "Display.h"
#include "Input.h"

namespace SE {
	namespace Window {
		bool Window::Initialise()
		{		
			display = new SE::Window::Display();
			input = new SE::Window::Input();
			input->InitInput();
			bool initS = display->InitDisplay();
			if (initS == false)
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

		const void* Window::GethWnd()
		{ 
			return display->GethWnd(); 
		}

		bool Window::HandleMSG()
		{ 
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
					return true;
				}
			}
			return false;
		}

		keyState Window::GetActionKeyState(int actionKey)
		{
			return input->GetActionKeyState(actionKey);
		}

	}	//namespace Window
}	//namespace SE