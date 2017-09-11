#ifndef SE_UTILZ_CONSOLE_H_
#define SE_UTILZ_CONSOLE_H_
#include <IConsoleBackend.h>
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

			static int Initialize(IConsoleBackend* backend);
			static void Shutdown();
			inline static void Show();
			inline static void Hide();


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