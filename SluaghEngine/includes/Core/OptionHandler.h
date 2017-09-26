#pragma once
#ifndef SE_CORE_OPTIONHANDLER_H
#define SE_CORE_OPTIONHANDLER_H
#include <map>
#include <string>
#include <Utilz\GUID.h>
#include <Utilz\Event.h>

namespace SE {
	namespace Core {

		class OptionHandler
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
			int Initialize(const std::string& filename);
			/**
			* @brief Return the value of requested option
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
			int GetOption(const std::string& section, const std::string& optionName, int defaultVal);
			/**
			* @brief Sets the value of requested option
			*
			* @param[in] section The section in which to search
			* @param[in] optionName The name of the option
			* @param[in] newValue The new value to set to the option
			*
			* @endcode
			*/
			void SetOption(const std::string& section, const std::string& optionName, int newValue);
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
			int UnloadOption(const std::string& filename);
			/**
			* @brief Register deleget to event
			*
			* @param[in] delegat The Delegat to register
			*
			* @endcode
			*/
			void Register(const Utilz::Delegate<void()>& delegat);
			/**
			* @brief Trigger optionEvent if it was a function
			*
			* @endcode
			*/
			void Trigger();
		private:
			std::map<std::string, std::map<std::string, int>> optionMap;
			Utilz::Event<void()> optionEvent;
		};
	}	//namespace Core
}	//namespace SE

#endif	//SE_CORE_OPTIONHANDLER_H