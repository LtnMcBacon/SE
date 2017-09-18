#ifndef SE_WINDOW_WINDOWSDL_H_
#define SE_WINDOW_WINDOWSDL_H_
#include "IWindow.h"
#include <SDL2/SDL.h>
#include <Windows.h>
#include <map>
namespace SE
{
	namespace Window
	{
		
		class WindowSDL : public IWindow
		{
		public:
			WindowSDL();
			~WindowSDL();

			int Initialize(const InitializationInfo& info) override;
			void Shutdown() override;

			void Frame() override;

			void* GetHWND() override;

			bool ButtonDown(uint32_t actionButton) const override;
			bool ButtonPressed(uint32_t actionButton) const override;
			bool ButtonUp(uint32_t actionButton) const override;

			void MapActionButton(uint32_t actionButton, KeyCode key) override;


			void BindMouseClickCallback(uint32_t actionButton, const MouseClickCallBack& callback) override;
			void BindMouseMotionCallback(const MouseMotionCallBack& callback) override;
			void BindKeyCallback(uint32_t actionButton, const KeyCallback& callback) override;
		private:
			/*Window related things*/
			SDL_Window* window;
			uint32_t width;
			uint32_t height;
			bool fullScreen;
			std::string windowTitle;
			HWND hwnd;

			/*input related things*/
			int curMouseX;
			int curMouseY;
			int relMouseX;
			int relMouseY;

			bool mouseLeftDown;
			bool mouseRightDown;

			

			/**<Maps generic keys defined in IWindow.h to key codes of SDL implementation*/
			std::map<KeyCode, uint32_t> keyMapping;
			/**<Maps actionButton -> SDL_key*/
			std::map<uint32_t, uint32_t> actionMappings;

			enum KeyState : uint32_t
			{
				NIL = 0,
				DOWN = 1 << 1,
				PRESSED = 3,
				UP = 4
			};
			/**<Maps SDLK to keystate (up, down, pressed)*/
			std::map<uint32_t, uint32_t> keyStates;
			uint32_t GetKeyState(uint32_t actionButton) const;


			
		};


	}
}



#endif

