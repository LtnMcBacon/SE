#ifndef SE_UTILZ_COMPILE_TIME_STRING_HASHER_H_
#define SE_UTILZ_COMPILE_TIME_STRING_HASHER_H_
#include <iostream>
#include <string>
#include <vector>
#include "crc_table.h"
// This doesn't take into account the nul char
#define COMPILE_TIME_CRC32_STR(x) (SE::Utilz::crc32<sizeof(x)/sizeof(x[0]) - 2>(x) ^ 0xFFFFFFFF)

namespace SE
{
	namespace Utilz
	{

		template<size_t idx>
		constexpr uint32_t crc32(const char * str)
		{
			return (crc32<idx - 1>(str) >> 8) ^ crc_table[(crc32<idx - 1>(str) ^ str[idx]) & 0x000000FF];
		}

		// This is the stop-recursion function
		template<>
		constexpr uint32_t crc32<size_t(-1)>(const char * str)
		{
			return 0xFFFFFFFF;
		}

		struct IDHash
		{
			struct Compare
			{
				inline bool operator() (const IDHash& lhs, const IDHash& rhs) const
				{
					return lhs.hash < rhs.hash;
				}
			};

			constexpr IDHash(const char* str) : hash(COMPILE_TIME_CRC32_STR(str)), str(str) {};

			uint32_t hash;
			const char* str;
		};
	}
}

#endif //SE_UTILZ_COMPILE_TIME_STRING_HASHER_H_