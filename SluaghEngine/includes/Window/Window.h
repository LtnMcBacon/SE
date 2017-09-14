#pragma once
#pragma once
#ifndef SE_WINDOW_WINDOW_H
#define SE_WINDOW_WINDOW_H
#include "InterfaceWindow.h"


namespace SE {
	namespace Window {
		class Input;
		class Display;

		class Window : public InterfaceWindow
		{
		public:
			Window();
			~Window();
			/**
			* @brief	Calls init functions in display and input
			*
			* @details	Calls and makes sure that both display and input gets initialized
			*
			* @retval 0 Tells that display creation was sucessful
			*
			*/
			int Initialise();
			/**
			* @brief	Loops through all messages
			*
			* @retval true All messages have been read
			*
			* @retval false Message is WM_QUIT
			*
			*/
			void Shutdown();

			bool HandleMSG();
			void* GethWnd();
			/**
			* @brief	Passes actionKey and keyboardKey for mapping to input
			*
			* @param[in] actionKey Value of the actionKey
			*
			* @param[in] keyboardKey Value of the declared keyboardKey
			*
			* @retval true Returns true if mapping suceded
			*
			* @retval false Returns false if mapping failed
			*
			*/
			bool MapKeyToKeyboard(int actionKey, Key keyboardKey);
			/**
			* @brief	Passes actionKey and mouseButton for mapping to input
			*
			* @param[in] actionKey Value of the actionKey
			*
			* @param[in] keyboardKey Value of the declared mouseButton
			*
			* @retval true Returns true if mapping suceded
			*
			* @retval false Returns false if mapping failed
			*
			*/
			bool MapKeyToMouse(int actionKey, MouseButton mouseButton);
			/**
			* @brief	Passes actionKey to get it's state
			*
			* @param[in] actionKey The game key which state we need
			*
			* @retval key_up Returns if key ain't pressed
			*
			* @retval key_pressed Returns if key has been pressed since last frame
			*
			* @retval key_down Returns if key have been pressed since last frame
			*
			*/
			keyState GetActionKeyState(int actionKey);
		private:
			SE::Window::Display* display;
			SE::Window::Input* input;
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_WINDOW_H