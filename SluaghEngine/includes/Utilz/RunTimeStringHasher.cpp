#include "RunTimeStringHasher.h"

uint32_t SE::Utilz::crc32_runtime(const char * str, size_t size)
{
	if (size != size_t(-1))
	{
		return (crc32_runtime(str, size_t(size - 1)) >> 8) ^ crc_table[(crc32_runtime(str, size_t(size - 1)) ^ str[size]) & 0x000000FF];
	}
	else
	{
		return 0xFFFFFFFF;
	}	
}
