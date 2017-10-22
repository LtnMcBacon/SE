#ifndef SE_DEV_CONSOLE_ICONSOLE_BACKEND_H_
#define SE_DEV_CONSOLE_ICONSOLE_BACKEND_H_
#include <string>
#include "Commands.h"
namespace SE
{
	namespace DevConsole
	{
		
		class IConsole
		{
		public:
			virtual ~IConsole() {};

			/**
			* @brief	Initialize the console backend
			*
			* @retval 0 On success
			*
			*/
			virtual int Initialize() = 0;
			virtual void Shutdown() = 0;

			virtual void Show() = 0;
			virtual void Hide() = 0;

			virtual void PrintChannel(const char* line, const char* channel = "Global", ...) = 0;
			virtual void Print(const char* line, ...) = 0;
			virtual void VPrint(const char* line, va_list args) = 0;
			virtual void VPrint(const char* line, va_list args, const char* channel) = 0;

			virtual void Getline(std::string& string) = 0;
			virtual size_t Getline(const char* buffer, size_t size) = 0;

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
			virtual int AddCommand(const DevConsole_Command& commandFunction, char* name, char* description) = 0;
		protected:
			IConsole() {};
			IConsole(const IConsole& other) = delete;
			IConsole(const IConsole&& other) = delete;
			IConsole& operator=(const IConsole& other) = delete;

		};
	}
}

#endif //SE_UTILZ_ICONSOLE_BACKEND_H_