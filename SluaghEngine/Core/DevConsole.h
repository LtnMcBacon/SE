#ifndef SE_CORE_DEV_CONSOLE_H_
#define SE_CORE_DEV_CONSOLE_H_
#include <Imgui\imgui.h>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <chrono>
#include <Graphics/IRenderer.h> //In order to plot VRAM usage.
namespace SE
{
	namespace Core
	{
		typedef std::function<void(int argc, char** argv)> ConsoleCommand;
		class DevConsole
		{
		public:
			DevConsole(SE::Graphics::IRenderer* renderer);
			~DevConsole();

			/*
			* @brief Adds a command to the console.
			* @details Adds a command that can be executed to the console. If a command with the same name already exists, the command will not be added.
			* @retval 0 On success.
			* @retval -1 On failure
			*/
			int AddCommand(const ConsoleCommand& command, const std::string& name, const std::string& description);
			
			/*
			* @brief Prints a message to the specified channel in the console. If the channel does not exist, it will be created.
			*/
			void Print(const std::string& message, const std::string& channel = "Default");

			/*
			* @brief Shows the console. Does nothing if console is already visible.
			*/
			void Show();
			/*
			* @brief Hides the console. Does nothing if console is already hidden.
			*/
			void Hide();

			/*
			* @brief Toggles the console on or off.
			*/
			void Toggle();

			/*
			* @brief Draws the console if the console is visible.
			*/
			void Frame();

			/*
			 * @brief Clears messages printed in the console.
			 */
			void Clear();
		private:
			struct CommandData
			{
				std::string name;
				std::string description;
				ConsoleCommand command;
			};
			struct Message
			{
				std::chrono::system_clock::time_point timestamp;
				std::string message;
				std::string channel;
			};

			/**<The maximum number of messages to be visible in the console.*/
			static const size_t maxMessages = 1024;

			SE::Graphics::IRenderer* renderer;

			DevConsole() = delete;
			DevConsole(const DevConsole& other) = delete;
			DevConsole(DevConsole&& other) = delete;
			DevConsole& operator=(const DevConsole& other) = delete;

			bool showConsole = false;
			std::vector<std::string> commandHistory;
			int commandHistoryIndex;
			std::map<std::string, CommandData> nameToCommand;

			std::vector<Message> messages;
			std::vector<char> inputBuffer;

			static int TextEditCallbackStub(ImGuiTextEditCallbackData* data);
			int TextEditCallback(ImGuiTextEditCallbackData* data);


			void ExecuteCommand(std::string name);
		};
	}
}




#endif //SE_CORE_DEV_CONSOLE_H_
