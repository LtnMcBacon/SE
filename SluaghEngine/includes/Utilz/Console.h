#ifndef SE_UTILZ_CONSOLE_H_
#define SE_UTILZ_CONSOLE_H_
#include "IConsoleBackend.h"
#include <mutex>
#include <thread>
#include <map>
#include <functional>
namespace SE
{
	namespace Utilz
	{
		
		class Console
		{
		public:
			static const int MAX_ARGUMENTS = 20;
			typedef std::function<void(IConsoleBackend* backend, int argc, char** argv)> DevConsole_Command;

			~Console();

			/**
			* @brief	Initialize the console
			*
			* @param[in] backend A pointer to a IConsoleBackend object.
			*
			* @retval 0 On success
			* 
			*/
			static int Initialize(IConsoleBackend* backend);
			static void Shutdown();
			static void Show();
			static void Hide();

			/**
			* @brief Add a command to the console.
			*
			* @details	Here you can add a command that can be used in the console
			* A command is a lambda with a name and description.
			*
			* @param[in] commandFunction The lambda.
			* @param[in] name Name of the command.
			* @param[in] description A short description of the command.
			*
			* @retval 0 On success
			*
			*/

			static int AddCommand(const DevConsole_Command& commandFunction, char* name, char* description);
		private:
			struct Command_Structure
			{
				DevConsole_Command commandFunction;
				char* name;
				char* description;
			};

			static Console* console;
			IConsoleBackend* backend;

			std::mutex lock;
			std::thread thread;

			std::map<uint32_t, Command_Structure> commands;

			bool running;

			Console();
			void Run();
			void InterpretCommand(char* command);
			void ParseCommandString(char* command, int* argc, char** argv);
		};

	}
}
#endif // SE_UTILZ_CONSOLE_H_