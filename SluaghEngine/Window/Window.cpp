#include "Window.h"

namespace SE {
	namespace Window {
		bool Window::Initialise()
		{		
			bool l_initS = m_display.InitDisplay(&m_input);
			if (l_initS == false)
				return l_initS;
			return l_initS;
		}

	}	//namespace Window
}	//namespace SE