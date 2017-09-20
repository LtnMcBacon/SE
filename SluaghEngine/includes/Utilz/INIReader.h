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

			int LoadINI(const std::string& filename, std::map<std::string, std::map<std::string, int>>& maps);

			int WriteToINI(const std::string& filename, std::map<std::string, std::map<std::string, int>>& maps);
		
		private:	
			std::string CreateKey(const std::string& section, const std::string& name);
		};
	}	//namespace SE
}	//namespace Utilz

#endif  // __SE_UTILZ_INIREADER_H__