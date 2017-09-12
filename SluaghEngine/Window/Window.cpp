#include "Window.h"

namespace SE {
	namespace Window {
		bool Window::Initialise()
		{		
			input.InitInput();
			bool initS = display.InitDisplay();
			if (initS == false)
				return initS;
			return initS;
		}

		void Window::MapKeyToKeyboard(int actionKey, gainput::Key keyboardKey)
		{
			input.MapKeyToKeyboard(actionKey, keyboardKey);
		}

		void Window::MapKeyToMouse(int actionKey, gainput::MouseButton mouseButton)
		{
			input.MapKeyToMouse(actionKey, mouseButton);
		}

		const void* Window::GethWnd()
		{ 
			return display.GethWnd(); 
		}

		void Window::HandleMSG(const MSG & 	msg)
		{ 
			input.HandleMSG(msg); 
		}

		int Window::GetActionKeyState(int actionKey)
		{
			return input.GetActionKeyState(actionKey);
		}

	}	//namespace Window
}	//namespace SE