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
			void InitInput();
			void HandleMSG(const MSG & 	msg);
			void MapKeyToKeyboard(int p_actionKey, gainput::Key p_keyboardKey);
			void MapKeyToMouse(int p_actionKey, gainput::MouseButton p_mouseButton);
		private:
			int m_width = 640;
			int m_height = 480;
			gainput::InputManager m_manager;
			gainput::DeviceId m_keyboardId;
			gainput::DeviceId m_mouseId;
			gainput::InputMap* m_map;
		};
	}	//namespace Window
}	//namespace SE

#endif	//SE_WINDOW_DISPLAY_H