#pragma once
#ifndef SE_WINDOW_DISPLAY_H
#define SE_WINDOW_DISPLAY_H
#include "InterfaceWindow.h"

namespace SE {
	namespace Window {

		class Display : public InterfaceWindow
		{
		public:
			Display();
			~Display();
			/**
			* @brief	Initiates the display
			*
			* @details	Gets application handle from the GetModuleHandle function and uses that to set up a display.
			* 
			* @retval return_value_1 Tells if display creation was sucessful
			*
			*/
			bool InitDisplay();
			const HWND& GethWnd();

		private:
			int m_width = 640, m_height = 480;
			HWND m_hWnd;
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_DISPLAY_H