#ifndef SE_UTILZ_CMD_CONSOLE_H_
#define SE_UTILZ_CMD_CONSOLE_H_
#include "IConsoleBackend.h"
namespace SE
{
	namespace Utilz
	{
		/**
		*
		* @brief A console backend that starts a seperate CMD window.
		*
		* @warning Show/Hide functions may not function as intended.
		*
		**/
		class CMDConsole : public IConsoleBackend
		{
		public:
			CMDConsole();
			~CMDConsole();

			virtual int Initialize();
			virtual void Shutdown();

			virtual void Show();
			virtual void Hide();

			virtual void Print(const char* line, ...);
			virtual void Getline(std::string* string);
			virtual size_t Getline(const char* buffer, size_t size);
		};
	}
}
#endif //SE_UTILZ_CMD_CONSOLE_H_