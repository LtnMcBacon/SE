#pragma once
#ifndef SE_WINDOW_INTERFACE_H
#define SE_WINDOW_INTERFACE_H
#include <Windows.h>
#include <windowsx.h>

namespace SE {
	namespace Window {
		class InterfaceWindow
		{
		public:
			InterfaceWindow() {};
			~InterfaceWindow() {};
			virtual bool InitDisplay() = 0;
			virtual const HWND& GethWnd() = 0;
		};
	}
}
#endif	//SE_WINDOW_INTERFACE_H