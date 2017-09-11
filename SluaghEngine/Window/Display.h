#pragma once
#ifndef SE_WINDOW_DISPLAY_H
#define SE_WINDOW_DISPLAY_H
#include "Windows.h"
#include "Windowsx.h"


namespace SE {
	namespace Window {

		class Display
		{
		public:
			Display();
			~Display();
			/**
			* @brief	Initiates the display
			*
			* @details	Gets application handle from the GetModuleHandle function and uses that to set up a display.
			*
			* @param[in] p_inputClass A referens to the input class
			*
			* @retval return_value_1 Tells if display creation was sucessful
			*
			*/
			bool InitDisplay(void* p_inputClass);
			const void* GethWnd();

		private:
			int m_width = 640, m_height = 480;
			HWND m_hWnd;
			static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_DISPLAY_H