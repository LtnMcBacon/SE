#include "RunTimeStringHasher.h"

uint32_t SE::Utilz::crc32_runtime(const char * str, size_t size)
{
	return size != size_t(-1) ? (crc32_runtime(str, size - 1) >> 8) ^ crc_table[(crc32_runtime(str, size - 1) ^ str[size]) & 0x000000FF] : 0xFFFFFFFF;
}
