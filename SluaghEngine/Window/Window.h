#pragma once
#pragma once
#ifndef SE_WINDOW_WINDOW_H
#define SE_WINDOW_WINDOW_H
#include "InterfaceWindow.h"
#include "Display.h"
#include "Input.h"

namespace SE {
	namespace Window {

		class Window : public InterfaceWindow
		{
		public:
			Window() {};
			~Window() {};
			/**
			* @brief	Calls init functions in display and input
			*
			* @details	Calls and makes sure that both display and input gets initialized
			*
			* @retval return_value_1 Tells if display and input creation was sucessful
			*
			*/
			bool Initialise();
			void HandleMSG(const MSG & 	msg) { m_input.HandleMSG(msg); };
			inline const void* GethWnd() { return m_display.GethWnd(); };
			bool MapKeyToKeyboard(int p_actionKey, gainput::Key p_keyboardKey);
			bool MapKeyToMouse(int p_actionKey, gainput::MouseButton p_mouseButton);

		private:
			SE::Window::Display m_display;
			SE::Window::Input m_input;
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_WINDOW_H