#ifndef SE_UTILZ_RUN_TIME_STRING_HASHER_H_
#define SE_UTILZ_RUN_TIME_STRING_HASHER_H_
#include <iostream>
#include <string>
#include <vector>
#include "crc_table.h"

namespace SE
{
	namespace Utilz
	{
		uint32_t crc32_runtime(const char * str, size_t size)
		{
			return size != size_t(-1) ? (crc32_runtime(str, size - 1) >> 8) ^ crc_table[(crc32_runtime(str, size -1) ^ str[size]) & 0x000000FF] : 0xFFFFFFFF;
		}
	}
}

#endif //SE_UTILZ_RUN_TIME_STRING_HASHER_H_