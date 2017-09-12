#include "Input.h"
#include "gainput\GainputInputDeviceMouse.h"
#include "gainput\GainputInputDeviceKeyboard.h"
namespace SE {
	namespace Window {
		Input::Input()
		{

		}

		Input::~Input()
		{
			delete inputMap;
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
		
		keyState Input::GetActionKeyState(int actionKey)
		{
			if (inputMap->GetBool(actionKey) == false)
			{
				return key_up;
			}
			else
			{
				if (inputMap->GetBoolIsNew(actionKey) == true)
				{
					return key_pressed;
				}
				else if (inputMap->GetBoolPrevious(actionKey) == true)
				{
					return key_down;
				}
			}
			return key_up;
		}
	}	//namespace Window
}	//namespace SE