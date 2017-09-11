#include "Window.h"

namespace SE {
	namespace Window {
		bool Window::Initialise()
		{		
			bool l_initS = m_input.InitInput();
			if (l_initS == false)
				return l_initS;
			return l_initS;
			l_initS = m_display.InitDisplay();
			if (l_initS == false)
				return l_initS;
			return l_initS;
		}

		bool Window::MapKeyToKeyboard(int p_actionKey, gainput::Key p_keyboardKey)
		{
			return m_input.MapKeyToKeyboard(p_actionKey, p_keyboardKey);
		}

		bool Window::MapKeyToMouse(int p_actionKey, gainput::MouseButton p_mouseButton)
		{
			return m_input.MapKeyToMouse(p_actionKey, p_mouseButton);
		}

	}	//namespace Window
}	//namespace SE