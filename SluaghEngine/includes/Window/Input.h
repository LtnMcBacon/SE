#pragma once
#pragma once
#ifndef SE_WINDOW_INPUT_H
#define SE_WINDOW_INPUT_H
#include "Windows.h"
#include "Windowsx.h"

#ifdef _DEBUG
#pragma comment (lib, "GainputD.lib")
#else
#pragma comment (lib, "Gainput.lib")
#endif

#include "gainput\gainput.h"
#include "KeyState.h"

namespace SE {
	
	namespace Window {
		

		class Input
		{
		public:

			Input();
			~Input();
			/**
			* @brief	Initiates the input
			*
			* @details	Set window height and width to the inputManager, recives input id for keyboard and mouse. 
			*
			*/
			void InitInput();
			/**
			* @brief	Uses inputManager to handle windows message
			*
			* @param[in] msg Windows message 
			*
			*/
			void HandleMSG(MSG msg);
			/**
			* @brief	Maps keyboardKey to actionKey
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
			bool MapKeyToKeyboard(int actionKey, Key keyboardKey);
			/**
			* @brief	Maps mouseButton to actionKey
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
			/**
			* @brief	Updates the inputManager
			*
			*/
			void Update();
		private:
			int width = 640;
			int height = 480;
			gainput::InputManager inputManager;
			gainput::DeviceId keyboardId;
			gainput::DeviceId mouseId;
			gainput::InputMap* inputMap = nullptr;
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_DISPLAY_H