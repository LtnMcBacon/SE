#pragma once
#ifndef SE_CORE_OPTIONHANDLER_H
#define SE_CORE_OPTIONHANDLER_H
#include <IOptionsHandler.h>
#include <map>
#include <string>
#include <Utilz\Event.h>

namespace SE {
	namespace Core {

		class OptionHandler : public IOptionsHandler
		{
		public:
			OptionHandler();
			~OptionHandler();
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
			int Initialize(const char* filename)override;
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
			int GetOptionInt(const std::string& section, const std::string& optionName, int defaultVal)override;
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
			size_t GetOptionUnsignedInt(const std::string& section, const std::string& optionName, size_t defaultVal)override;
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
			bool GetOptionBool(const std::string& section, const std::string& optionName, bool defaultVal)override;
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
			std::string GetOptionString(const std::string& section, const std::string& optionName, const std::string& defaultVal)override;
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
			double GetOptionDouble(const std::string& section, const std::string& optionName, double defaultVal)override;
			
			
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			void SetOptionInt(const std::string& section, const std::string& optionName, int newValue)override;
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			void SetOptionUnsignedInt(const std::string& section, const std::string& optionName, size_t newValue)override;
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			void SetOptionBool(const std::string& section, const std::string& optionName, bool newValue)override;
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			void SetOptionString(const std::string& section, const std::string& optionName, const std::string & newValue)override;
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			void SetOptionDouble(const std::string& section, const std::string& optionName, double newValue)override;
			
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
			int UnloadOption(const std::string& filename)override;
			/**
			* @brief Register deleget to event
			*
			* @param[in] delegat The Delegat to register
			*
			* @endcode
			*/
			void Register(const Utilz::Delegate<void()>& delegat)override;
			/**
			* @brief Trigger optionEvent if it was a function
			*
			* @endcode
			*/
			void Trigger();
		private:
			std::map<std::string, std::map<std::string, std::string>> optionMap;
			Utilz::Event<void()> optionEvent;
		};
	}	//namespace Core
}	//namespace SE

#endif	//SE_CORE_OPTIONHANDLER_H