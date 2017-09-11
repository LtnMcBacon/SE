#pragma once
#ifndef SE_WINDOW_INTERFACE_H
#define SE_WINDOW_INTERFACE_H

namespace SE {
	namespace Window {
		class InterfaceWindow
		{
		public:
			InterfaceWindow() {};
			~InterfaceWindow() {};
			virtual bool Initialise() = 0;
			virtual const void* GethWnd() = 0;
		};
	}	//namespace Window
}	//namespace SE
#endif	//SE_WINDOW_INTERFACE_H