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
			if(inputMap != nullptr)
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
			if (inputMap->GetBool(actionKey) == false)
			{
				return key_isUp;
			}
			else
			{
				if (inputMap->GetBoolIsNew(actionKey) == false && inputMap->GetBoolPrevious(actionKey) == true)
				{
					return key_isDown;
				}
				else
				{
					return key_isPressed;
				}
			}
			return key_isError;
		}

		void Input::Update()
		{
			inputManager.Update();
		}
	}	//namespace Window
}	//namespace SE