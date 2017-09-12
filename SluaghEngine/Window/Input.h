#pragma once
#pragma once
#ifndef SE_WINDOW_INPUT_H
#define SE_WINDOW_INPUT_H


#if defined(DEBUG)
#pragma comment (lib, "GainputD.lib")
#else
#pragma comment (lib, "Gainput.lib")
#endif

#include "gainput\gainput.h"


namespace SE {
	
	namespace Window {
		

		class Input
		{
		public:
			Input();
			~Input() {};
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
			void HandleMSG(const MSG & 	msg);
			/**
			* @brief	Maps keyboardKey to actionKey
			*
			* @param[in] actionKey Value of the actionKey
			*
			* @param[in] keyboardKey Value of the declared mouseButton
			*
			*/
			void MapKeyToKeyboard(int actionKey, gainput::Key keyboardKey);
			/**
			* @brief	Maps mouseButton to actionKey
			*
			* @param[in] actionKey Value of the actionKey
			*
			* @param[in] keyboardKey Value of the declared mouseButton
			*
			*/
			void MapKeyToMouse(int actionKey, gainput::MouseButton mouseButton);
		private:
			int width = 640;
			int height = 480;
			gainput::InputManager inputManager;
			gainput::DeviceId keyboardId;
			gainput::DeviceId mouseId;
			gainput::InputMap* inputMap;
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_DISPLAY_H