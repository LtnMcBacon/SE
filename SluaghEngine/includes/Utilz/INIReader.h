#pragma once
#ifndef __SE_UTILZ_INIREADER_H__
#define __SE_UTILZ_INIREADER_H__

#include <map>
#include <string>

namespace SE
{
	namespace Utilz
	{

		class INIReader
		{
		public:
			INIReader();

			~INIReader();
			/**
			* @brief Loads from requested .ini file
			*
			* @param[in] filename The name of the .ini file
			* @param[in] maps Reference to a map holding maps
			*
			* @retval 0 On success.
			* @retval -1 On unsuccessful read.
			*
			* @endcode
			*/
			int LoadINI(const std::string& filename, std::map<std::string, std::map<std::string, std::string>>& maps);
			/**
			* @brief Writes to requested .ini file
			*
			* @param[in] filename The name of the .ini file
			* @param[in] maps Reference to a map holding maps
			*
			* @retval 0 On success.
			* @retval -1 On unsuccessful read.
			*
			* @endcode
			*/
			int WriteToINI(const std::string& filename, std::map<std::string, std::map<std::string, std::string>>& maps);
		
		};
	}	//namespace SE
}	//namespace Utilz

#endif  // __SE_UTILZ_INIREADER_H__