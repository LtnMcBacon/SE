#ifndef SE_UTILZ_PATH_PARSING_H_
#define SE_UTILZ_PATH_PARSING_H_

#include <string>

namespace SE
{
	namespace Utilz
	{
		std::string getExtension(const std::string& path)
		{
			const size_t period_idx = path.find_last_of('.');
			if (std::string::npos != period_idx)
			{
				return path.substr(period_idx + 1);
			}
		}
		std::string getFilename(std::string const& path)
		{
			const size_t last_slash_idx = path.find_last_of("\\/");
			if (std::string::npos != last_slash_idx)
			{
				return path.substr(last_slash_idx + 1);
			}
		}

	}
}

#endif SE_UTILZ_PATH_PARSING_H_