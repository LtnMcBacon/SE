#ifndef SE_UTILZ_RUN_TIME_STRING_HASHER_H_
#define SE_UTILZ_RUN_TIME_STRING_HASHER_H_
#include <iostream>
#include <string>
#include <vector>
#include "crc_table.h"
#define RUN_TIME_CRC32_STR(x) (SE::Utilz::crc32_runtime(x, sizeof(x)-2) ^0xFFFFFFFF)

namespace SE
{
	namespace Utilz
	{
		uint32_t crc32_runtime(const char * str, size_t size);
		//{
		//	return size != size_t(-1) ? (crc32_runtime(str, size - 1) >> 8) ^ crc_table[(crc32_runtime(str, size -1) ^ str[size]) & 0x000000FF] : 0xFFFFFFFF;
		//}
	}
}

#endif //SE_UTILZ_RUN_TIME_STRING_HASHER_H_