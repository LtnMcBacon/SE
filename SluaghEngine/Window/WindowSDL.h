#ifndef SE_WINDOW_WINDOWSDL_H_
#define SE_WINDOW_WINDOWSDL_H_
#include "IWindow.h"
#include <SDL2/SDL.h>
#include <Windows.h>
#include <map>
#include <vector>
#include <Utilz\CircularFiFo.h>
#include <fstream>
#include <thread>
#include <Utilz\Timer.h>

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
			void RegFrame();
			void RecordFrame();
			void PlaybackFrame();

			

			void* GetHWND() override;
			/**
			* @brief Returns a pointer to the SDL_Window
			* @param[in] implementation The implementation to get the window pointer from.
			* @retval ptr A pointer to the SDL_Window in use.
			* @warning Aborts if implementation is not WINDOW_IMPLEMENTATION_SDL
			*/
			void* GetWindowImplementation(WindowImplementation implementation) override;
			bool ButtonDown(uint32_t actionButton) const override;
			bool ButtonPressed(uint32_t actionButton) const override;
			bool ButtonUp(uint32_t actionButton) const override;

			void GetMousePos(int& x, int& y) const override;

			void MapActionButton(uint32_t actionButton, KeyCode key) override;


			void BindMouseClickCallback(uint32_t actionButton, const MouseClickCallback& callback) override;
			void BindMouseMotionCallback(const MouseMotionCallback& callback) override;
			void BindKeyPressCallback(uint32_t actionButton, const KeyCallback& callback) override;
			void BindKeyDownCallback(uint32_t actionButton, const KeyCallback& callback) override;
			void BindKeyUpCallback(uint32_t actionButton, const KeyCallback& callback) override;
			void UnbindCallbacks() override;

			/**
			* @brief Calls the callback whenever an event, any event, happens. Calls the callback with a pointer to the SDL_Event as the parameter.
			* @param[in] callback A delegate that is called when an event occurs
			* @sa OnEventCallback
			*/
			bool RegisterOnEventCallback(const OnEventCallback& callback) override;


			/*
			* @brief Returns the width of the window.
			*/
			int Width() const override;
			/*
			* @brief Returns the height of the window.
			*/
			int Height() const override;
		
			bool SetWindow(int inHeight, int inWidth, bool inFullscreen) override;
			/**
			* @brief Tells window to update time
			*/
			inline void UpdateTime() override {
				if (!playback)
					time.Tick();
				else
					recordDelta;
			};
			/**
			* @brief Gets delta time from window
			*
			* @retval float Returns time int milliseconds
			*/
			inline float GetDelta() const override
			{
				if (!playback)
					return time.GetDelta();
				else
					return recordDelta;
			};
		private:
			void EventSwitch(SDL_Event ev);

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

			bool mouseLeftDown = false;
			bool mouseRightDown = false;

			
			std::map<uint32_t, uint32_t> keyToAction;
			/**<Maps generic keys defined in IWindow.h to key codes of SDL implementation*/
			std::map<KeyCode, uint32_t> keyMapping;
			
			std::map<uint32_t, std::vector<KeyCallback>> actionToKeyPressCallback;
			std::map<uint32_t, std::vector<KeyCallback>> actionToKeyDownCallback;
			std::map<uint32_t, std::vector<KeyCallback>> actionToKeyUpCallback;
			std::map<uint32_t, std::vector<MouseClickCallback>> actionToMouseClickCallback;
			std::vector<MouseMotionCallback> mouseMotionCallbacks;
			std::vector<OnEventCallback> onEventCallbacks;

			enum KeyState : uint32_t
			{
				NIL = 0,
				DOWN = 1 << 1,
				PRESSED = 3,
				UP = 4
			};
			/**<Maps action button to keystate (up, down, pressed)*/
			std::map<uint32_t, uint32_t> actionToKeyState;
			uint32_t GetKeyState(uint32_t actionButton) const;

			//record stuff
			struct inputRecData
			{
				float dTime;
				size_t nrOfEvent;
				std::vector<SDL_Event> events;
			};


			void RecordToFile();
			std::ofstream recFile;
			std::ifstream playbackfile;
			Utilz::CircularFiFo<inputRecData, 256> circFiFo;
			bool recording = false;
			bool playback = false;
			float recordDelta = 0.0;
			std::vector<inputRecData> playbackData;
			std::thread recThread;
			size_t frame = 0;


			typedef void(WindowSDL::*FrameStrategy)();

			FrameStrategy currentFrameStrategy;

			void StartRecording();
			void LoadRecording();

			Utilz::Timer time;
		};


	}
}



#endif

