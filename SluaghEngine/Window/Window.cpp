#include "Window.h"

namespace SE {
	namespace Window {
		bool Window::Initialise()
		{		
			m_input.InitInput();
			bool l_initS = m_display.InitDisplay();
			if (l_initS == false)
				return l_initS;
			return l_initS;
		}

		void Window::MapKeyToKeyboard(int p_actionKey, gainput::Key p_keyboardKey)
		{
			m_input.MapKeyToKeyboard(p_actionKey, p_keyboardKey);
		}

		void Window::MapKeyToMouse(int p_actionKey, gainput::MouseButton p_mouseButton)
		{
			m_input.MapKeyToMouse(p_actionKey, p_mouseButton);
		}

	}	//namespace Window
}	//namespace SE