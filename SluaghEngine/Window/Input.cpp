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
			inputManager.SetDisplaySize(width, height);

			// Receive ID from input devices
			keyboardId = inputManager.CreateDevice<gainput::InputDeviceKeyboard>();  //.CreateDevice<InputDeviceKeyboard>();
			mouseId = inputManager.CreateDevice<gainput::InputDeviceMouse>();

			inputMap = new gainput::InputMap(inputManager);
		}

		void Input::HandleMSG(const MSG & 	msg)
		{
			return inputManager.HandleMessage(msg);
		}

		void Input::MapKeyToKeyboard(int actionKey, gainput::Key keyboardKey)
		{
			inputMap->MapBool(actionKey, keyboardId, keyboardKey);
		}

		void Input::MapKeyToMouse(int actionKey, gainput::MouseButton mouseButton)
		{
			inputMap->MapBool(actionKey, mouseId, mouseButton);
		}
	}	//namespace Window
}	//namespace SE