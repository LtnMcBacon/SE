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
			* @retval true Tells that display creation was sucessful
			*
			* @retval false Tells that display creation was unsucessful
			*
			*/
			bool Initialise();
			/**
			* @brief	Passes down windows message to input
			*
			* @param[in] msg Windows message 
			*
			*/
			void HandleMSG(const MSG & 	msg);
			const void* GethWnd();
			/**
			* @brief	Passes actionKey and keyboardKey for mapping to input
			*
			* @param[in] actionKey Value of the actionKey
			*
			* @param[in] keyboardKey Value of the declared keyboardKey
			*
			*/
			void MapKeyToKeyboard(int actionKey, gainput::Key keyboardKey);
			/**
			* @brief	Passes actionKey and mouseButton for mapping to input
			*
			* @param[in] actionKey Value of the actionKey
			*
			* @param[in] keyboardKey Value of the declared mouseButton
			*
			*/
			void MapKeyToMouse(int actionKey, gainput::MouseButton mouseButton);

		private:
			SE::Window::Display display;
			SE::Window::Input input;
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_WINDOW_H