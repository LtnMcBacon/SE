#pragma once
#ifndef SE_WINDOW_INTERFACE_H
#define SE_WINDOW_INTERFACE_H
#include "KeyState.h"

namespace SE {
	namespace Window {

		class InterfaceWindow
		{
		public:
			InterfaceWindow() {};
			~InterfaceWindow() {};
			/**
			* @brief	Virtual call supposed to init functions in display and input
			*
			* @details	Calls and makes sure that both display and input gets initialized
			*
			* @retval true Tells that display creation was sucessful
			*
			* @retval false Tells that display creation was unsucessful
			*
			*/
			virtual bool Initialise() = 0;
			/**
			* @brief	Loops through all messages
			*
			* @retval true All messages have been read
			*
			* @retval false Message is WM_QUIT
			*
			*/
			virtual bool HandleMSG() = 0;
			virtual const void* GethWnd() = 0;
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
			virtual bool MapKeyToKeyboard(int actionKey, Key keyboardKey) = 0;
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
			virtual bool MapKeyToMouse(int actionKey, MouseButton mouseButton) = 0;
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
			virtual keyState GetActionKeyState(int actionKey) = 0;
		};

	}	//namespace Window
}	//namespace SE
#endif	//SE_WINDOW_INTERFACE_H