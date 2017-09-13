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
		/**
		*
		* @brief This is a singleton class that you can use for a command console.
		*
		* @details Initialize console with Console::Initialize and give it a backend.
		*
		* @sa IConsoleBackend
		*
		**/
		class Console
		{
		public:
			static const int MAX_ARGUMENTS = 20;
			typedef std::function<void(IConsoleBackend* backend, int argc, char** argv)> DevConsole_Command;

			~Console();

			/**
			* @brief	Initializes the console
			*
			* @details	Initialize a console with the given backend.
			* The console is a singleton so use Console::"The function" directly.
			* You need to Initialize the console and use Show/Hide to display it.
			*
			* This is how you make a new line in the description.
			*
			*
			* @param[in] backend The backend implementation, this can be your own implementation to render the console.
			*
			* @retval 0 On success.
			* @retval rest See your backend implementation.
			*
			* @warning Dont forget to call shutdown at end.
			**/
			static int Initialize(IConsoleBackend* backend);
			static void Shutdown();
			/**
			* @brief	Shows the console.
			**/
			static void Show();
			/**
			* @brief	Hides the console.
			**/
			static void Hide();

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
			* Console::AddCommand([this](IConsoleBackend* backend, int argc, char** argv)
			* {
			*  backend->Print("Printing %d\n", this->d);
			* },
			* "write",
			* "prints d to the console");
			* @endcode
			*/
			static int AddCommand(const DevConsole_Command& commandFunction, char* name, char* description);

			static void Print(const char* str, ...);

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