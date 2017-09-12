#include "Input.h"
#include "gainput\GainputInputDeviceMouse.h"
#include "gainput\GainputInputDeviceKeyboard.h"
#include "gainput\gainputInputMap.h"
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

		void Input::HandleMSG(MSG	msg)
		{
			return inputManager.HandleMessage(msg);
		}

		bool Input::MapKeyToKeyboard(int actionKey, Key keyboardKey)
		{
			inputMap->MapBool(actionKey, keyboardId, (gainput::Key)keyboardKey);
			return inputMap->IsMapped(actionKey);
		}

		bool Input::MapKeyToMouse(int actionKey, MouseButton mouseButton)
		{
			inputMap->MapBool(actionKey, mouseId, (gainput::MouseButton)mouseButton);
			return inputMap->IsMapped(actionKey);
		}
		
		keyState Input::GetActionKeyState(int actionKey)
		{
			if (inputMap->GetBoolPrevious(actionKey) == false)
			{
				if (inputMap->GetBoolIsNew(actionKey) == false)
				{
					return key_up;
				}
				else
				{
					return key_pressed;
				}
			}
			else
			{
				if (inputMap->GetBool(actionKey) == true)
				{
					return key_down;
				}
				else
				{
					return key_up;
				}
			}
			return key_up;
		}

		void Input::Update()
		{
			inputManager.Update();
		}
	}	//namespace Window
}	//namespace SE