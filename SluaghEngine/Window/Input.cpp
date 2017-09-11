#include "Input.h"
#include "gainput\GainputInputDeviceMouse.h"
#include "gainput\GainputInputDeviceKeyboard.h"
namespace SE {
	namespace Window {
		Input::Input()
		{

		}

		void Input::InitInput()
		{
			// Set display size
			m_manager.SetDisplaySize(m_width, m_height);

			// Receive ID from input devices
			m_keyboardId = m_manager.CreateDevice<gainput::InputDeviceKeyboard>();  //.CreateDevice<InputDeviceKeyboard>();
			m_mouseId = m_manager.CreateDevice<gainput::InputDeviceMouse>();

			m_map = new gainput::InputMap(m_manager);
		}

		void Input::HandleMSG(const MSG & 	msg)
		{
			return m_manager.HandleMessage(msg);
		}

		void Input::MapKeyToKeyboard(int p_actionKey, gainput::Key p_keyboardKey)
		{
			m_map->MapBool(p_actionKey, m_keyboardId, p_keyboardKey);
		}

		void Input::MapKeyToMouse(int p_actionKey, gainput::MouseButton p_mouseButton)
		{
			m_map->MapBool(p_actionKey, m_mouseId, p_mouseButton);
		}
	}	//namespace Window
}	//namespace SE