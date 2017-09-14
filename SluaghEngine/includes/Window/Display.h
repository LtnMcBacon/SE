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
			* @retval 0 On success.
			*
			*/
			int InitDisplay();
			void* GethWnd();

		private:
			int width = 640; 
			int height = 480;
			HWND hWnd;
			static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_DISPLAY_H