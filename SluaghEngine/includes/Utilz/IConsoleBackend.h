#ifndef SE_UTILZ_ICONSOLE_BACKEND_H_
#define SE_UTILZ_ICONSOLE_BACKEND_H_
#include <string>
namespace SE
{
	namespace Utilz
	{
		
		class IConsoleBackend
		{
		public:
			virtual ~IConsoleBackend() {};

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

			virtual void Print(const char* line, ...) = 0;
			virtual void VPrint(const char* line, va_list args) = 0;
			virtual void Getline(std::string* string) = 0;
			virtual size_t Getline(const char* buffer, size_t size) = 0;
		protected:
			IConsoleBackend() {};
			IConsoleBackend(const IConsoleBackend& other) = delete;
			IConsoleBackend(const IConsoleBackend&& other) = delete;
			IConsoleBackend& operator=(const IConsoleBackend& other) = delete;

		};
	}
}

#endif //SE_UTILZ_ICONSOLE_BACKEND_H_