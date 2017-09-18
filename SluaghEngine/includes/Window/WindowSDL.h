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

			virtual bool ButtonDown(uint32_t actionButton) const;
			virtual bool ButtonPressed(uint32_t actionButton) const;
			virtual bool ButtonUp(uint32_t actionButton) const;

			void MapActionButton(uint32_t actionButton, KeyCode key) override;
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

			

			/**<Maps generic keys defined in IWindow.h to key codes of SDL implementation*/
			std::map<KeyCode, uint32_t> keyMapping;
			/**<Maps actionButton -> generic keys*/
			std::map<uint32_t, KeyCode> actionMappings;

			enum KeyState : uint32_t
			{
				NIL = 0,
				DOWN = 1 << 1,
				PRESSED = 2,
				UP = 4
			};
			/**<Maps SDLK to keystate (up, down, pressed)*/
			std::map<uint32_t, uint32_t> keyStates;
			uint32_t GetKeyState(uint32_t actionButton) const;


			
		};


	}
}



#endif

