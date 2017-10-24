#ifndef SE_INTERFACE_OPTIONS_HANDLER_H_
#define SE_INTERFACE_OPTIONS_HANDLER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include <string>
#include <Utilz\Delegate.h>

namespace SE
{
	namespace Core
	{
		class IOptionsHandler
		{
		public:
			virtual ~IOptionsHandler() {};

			/**
			* @brief Loads options from file
			*
			* @param[in] filename The file to load from
			*
			* @retval 0 On success.
			* @retval -1 On unsuccessful.
			*
			* @endcode
			*/
			virtual int Initialize(const char* filename) = 0;
			/**
			* @brief Return the value of requested option as int
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] defaultVal A default value to return if value can't be found
			*
			* @retval valueOfOption On success.
			* @retval defaultVal On unsuccessful.
			*
			* @endcode
			*/
			virtual int GetOptionInt(const std::string& section, const std::string& optionName, int defaultVal) = 0;
			/**
			* @brief Return the value of requested option as unsigned int
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] defaultVal A default value to return if value can't be found
			*
			* @retval valueOfOption On success.
			* @retval defaultVal On unsuccessful.
			*
			* @endcode
			*/
			virtual size_t GetOptionUnsignedInt(const std::string& section, const std::string& optionName, size_t defaultVal) = 0;
			/**
			* @brief Return the value of requested option as bool
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] defaultVal A default value to return if value can't be found
			*
			* @retval valueOfOption On success.
			* @retval defaultVal On unsuccessful.
			*
			* @endcode
			*/
			virtual bool GetOptionBool(const std::string& section, const std::string& optionName, bool defaultVal) = 0;
			/**
			* @brief Return the value of requested option as string
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] defaultVal A default value to return if value can't be found
			*
			* @retval valueOfOption On success.
			* @retval defaultVal On unsuccessful.
			*
			* @endcode
			*/
			virtual std::string GetOptionString(const std::string& section, const std::string& optionName, const std::string& defaultVal) = 0;
			/**
			* @brief Return the value of requested option as double
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] defaultVal A default value to return if value can't be found
			*
			* @retval valueOfOption On success.
			* @retval defaultVal On unsuccessful.
			*
			* @endcode
			*/
			virtual double GetOptionDouble(const std::string& section, const std::string& optionName, double defaultVal) = 0;


			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			virtual void SetOptionInt(const std::string& section, const std::string& optionName, int newValue) = 0;
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			virtual void SetOptionUnsignedInt(const std::string& section, const std::string& optionName, size_t newValue) = 0;
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			virtual void SetOptionBool(const std::string& section, const std::string& optionName, bool newValue) = 0;
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			virtual void SetOptionString(const std::string& section, const std::string& optionName, std::string newValue) = 0;
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			virtual void SetOptionDouble(const std::string& section, const std::string& optionName, double newValue) = 0;

			/**
			* @brief Return the value of requested option
			*
			* @param[in] filename The file to write to
			*
			* @retval 0 On success.
			* @retval -1 On unsuccessful.
			*
			* @endcode
			*/
			virtual int UnloadOption(const std::string& filename) = 0;
			/**
			* @brief Register deleget to event
			*
			* @param[in] delegat The Delegat to register
			*
			* @endcode
			*/
			virtual void Register(const Utilz::Delegate<void()>& delegat) = 0;
			/**
			* @brief Trigger optionEvent if it was a function
			*
			* @endcode
			*/
			virtual void Trigger() = 0;
		
		protected:
			IOptionsHandler() {};
	
			IOptionsHandler(const IOptionsHandler& other) = delete;
			IOptionsHandler(const IOptionsHandler&& other) = delete;
			IOptionsHandler& operator=(const IOptionsHandler& other) = delete;
			IOptionsHandler& operator=(const IOptionsHandler&& other) = delete;

		};


		/**
		* @brief Create an instance of the OptionsHandler
		**/
		DECLDIR_CORE IOptionsHandler* CreateOptionsHandler();
	}
}

#endif //SE_INTERFACE_OPTIONS_HANDLER_H_