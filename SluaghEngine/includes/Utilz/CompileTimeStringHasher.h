#ifndef SE_UTILZ_COMPILE_TIME_STRING_HASHER_H_
#define SE_UTILZ_COMPILE_TIME_STRING_HASHER_H_
#include <iostream>
#include <string>
#include <vector>
#include "crc_table.h"
// This doesn't take into account the nul char

// This don't take into account the nul char
#define COMPILE_TIME_CRC32_STR(x) (SE::Utilz::MM<sizeof(x)-1>::crc32(x))
namespace SE
{
	namespace Utilz
	{

		template<int size, int idx = 0, class dummy = void>
		struct MM {
			static constexpr unsigned int crc32(const char * str, unsigned int prev_crc = 0xFFFFFFFF)
			{
				return MM<size, idx + 1>::crc32(str, (prev_crc >> 8) ^ crc_table[(prev_crc ^ str[idx]) & 0xFF]);
			}
		};

		// This is the stop-recursion function
		template<int size, class dummy>
		struct MM<size, size, dummy> {
			static constexpr unsigned int crc32(const char * str, unsigned int prev_crc = 0xFFFFFFFF)
			{
				return prev_crc ^ 0xFFFFFFFF;
			}
		};

		struct IDHash
		{
			struct Hasher
			{
				inline uint32_t operator()(const IDHash& g) const
				{
					return g.hash;
				}
			};



			bool operator==(const IDHash& other)const { return this->hash == other.hash; };
			constexpr IDHash(const char* str) : hash(COMPILE_TIME_CRC32_STR(str)), str(str) {};

			uint32_t hash;
			const char* str;
		};
	}
}

#endif //SE_UTILZ_COMPILE_TIME_STRING_HASHER_H_