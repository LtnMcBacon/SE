#ifndef SE_DEV_CONSOLE_CMD_CONSOLE_H_
#define SE_DEV_CONSOLE_CMD_CONSOLE_H_
#include "IConsole.h"
#include <mutex>
#include <thread>
namespace SE
{
	namespace DevConsole
	{
		/**
		*
		* @brief A console backend that starts a seperate CMD window.
		*
		* @warning Show/Hide functions may not function as intended.
		*
		**/
		class CMDConsole : public IConsole
		{
		public:
			CMDConsole();
			~CMDConsole();

			int Initialize()override;
			void Shutdown()override;

			void Show()override;
			void Hide()override;

			/*
			* @brief Toggles the console on or off.
			*/
			void Toggle() override;

			/*
			* @brief Draws the console if the console is visible.
			*/
			void Frame() override;

			/*
			* @brief Clears messages printed in the console.
			*/
			void Clear() override;


			void PrintChannel(const char* line, const char* channel = "Global", ...)override;
			void Print(const char* line, ...)override;
			void VPrint(const char* line, va_list args)override;
			void VPrint(const char* line, va_list args, const char* channel)override;

			void Getline(std::string& string)override;
			size_t Getline(const char* buffer, size_t size)override;

			int AddCommand(const DevConsole_Command& commandFunction, char* name, char* description)override;
		private:
			void Run();
			bool running;

			std::thread myThread;

			Commands commands;
		};
	}
}
#endif //SE_DEV_CONSOLE_CMD_CONSOLE_H_