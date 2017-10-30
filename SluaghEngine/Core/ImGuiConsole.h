#ifndef SE_CORE_DEV_CONSOLE_H_
#define SE_CORE_DEV_CONSOLE_H_
#include <DevConsole\IConsole.h>
#include <Imgui\imgui.h>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <chrono>
#include <Graphics/IRenderer.h> //In order to plot VRAM usage.
#include <Window/IWindow.h>

namespace SE
{
	namespace Core
	{
		class ImGuiConsole : public DevConsole::IConsole
		{
		public:
			ImGuiConsole(SE::Graphics::IRenderer* renderer, SE::Window::IWindow* window);
			~ImGuiConsole();


			/**
			* @brief	Initialize the console backend
			*
			* @retval 0 On success
			*
			*/
			int Initialize() override;
			void Shutdown() override;

			/**
			* @brief	Add a command that can be called from the console.
			*
			* @details	A command is a lambda, a name, and a short description.
			*
			* @param[in] commandFunction The command to be called.
			* @param[in] name Name of the command.
			* @param[in] description A short description of the command.
			*
			* @retval 0 On success.
			*
			* @warning The commandFunction must follow the function declaration DevConsole_Command precisely.
			*
			* Example code:
			* @code
			* console->AddCommand([this](IConsoleBackend* backend, int argc, char** argv)
			* {
			*  backend->Print("Printing %d\n", this->d);
			* },
			* "write",
			* "prints d to the console");
			* @endcode
			*/
			int AddCommand(const DevConsole::DevConsole_Command& commandFunction, char* name, char* description) override;
			
			void PrintChannel(const char* channel, const char* line, ...) override;
			void Print(const char* line, ...) override;
			void VPrint(const char* line, va_list args) override;
			void VPrint(const char* channel, const char* line, va_list args) override;

			void Getline(std::string& string) override;
			size_t Getline(const char* buffer, size_t size) override;

			/*
			* @brief Shows the console. Does nothing if console is already visible.
			*/
			void Show()override;
			/*
			* @brief Hides the console. Does nothing if console is already hidden.
			*/
			void Hide()override;

			/*
			* @brief Toggles the console on or off.
			*/
			void Toggle()override;
			/*
			* @brief Start the frame.
			*/
			void BeginFrame()override;
			/*
			* @brief Draws the console if the console is visible.
			*/
			void Frame()override;
			/*
			* @brief Ends the frame.
			*/
			void EndFrame() override;
			/*
			 * @brief Clears messages printed in the console.
			 */
			void Clear()override;

			void* GetContext() override;

			/**
			* @brief	Add a callback that will be called each frame.
			*
			**/
			void AddFrameCallback(const std::function<void()>& frameCallback) override;

		private:
			ImGuiTextFilter filter;
			bool scrollToBottom = false;

			struct Message
			{
				std::chrono::system_clock::time_point timestamp;
				std::string message;
				std::string channel;
			};

			/**<The maximum number of messages to be visible in the console.*/
			static const size_t maxMessages = 1024;

			SE::Graphics::IRenderer* renderer;
			SE::Window::IWindow* window;

			ImGuiConsole() = delete;
			ImGuiConsole(const ImGuiConsole& other) = delete;
			ImGuiConsole(ImGuiConsole&& other) = delete;
			ImGuiConsole& operator=(const ImGuiConsole& other) = delete;

			bool showConsole = false;
			std::vector<std::string> commandHistory;
			int commandHistoryIndex;

			std::vector<Message> messages;
			std::vector<char> inputBuffer;
			std::vector<std::function<void()>> frameCallbacks;

			static int TextEditCallbackStub(ImGuiTextEditCallbackData* data);
			int TextEditCallback(ImGuiTextEditCallbackData* data);


			DevConsole::Commands commands;
		};
	}
}




#endif //SE_CORE_DEV_CONSOLE_H_
