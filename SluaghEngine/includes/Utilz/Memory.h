#ifndef SE_UTILZ_MEMORY_H_
#define SE_UTILZ_MEMORY_H_
#include "windows.h"
#include "psapi.h"
// https://github.com/grisumbras/enum-flags
namespace SE
{
	namespace Utilz
	{	
		namespace Memory
		{
			inline unsigned long long GetVirtualProcessMemory()
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;		
				BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				if( res != 0)
					return static_cast<unsigned long long>(pmc.PrivateUsage); // WorkingSetSize (for physical memory)
				return ~unsigned long long(0u);
			}
			inline unsigned long long GetPhysicalProcessMemory()
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;
				BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				if (res != 0)
					return static_cast<unsigned long long>(pmc.WorkingSetSize); // WorkingSetSize (for physical memory)
				return ~unsigned long long(0u);
			}
			inline bool IsUnderLimit(const unsigned long long max, const unsigned long long extra)
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;
				BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				if ( res != 0)
					return (static_cast<unsigned long long>(pmc.PrivateUsage) + extra) <= max; // WorkingSetSize (for physical memory)
				return false;
			}

			inline bool IsUnderLimit(const unsigned long long max)
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;
				BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				if (res != 0)
					return (static_cast<unsigned long long>(pmc.PrivateUsage)) <= max; // WorkingSetSize (for physical memory)
				return false;
			}
			constexpr unsigned long long toKB(unsigned long long bytes)
			{
				return bytes / unsigned long long(1024u);
			}
			constexpr unsigned long long toMB(unsigned long long bytes)
			{
				return bytes / unsigned long long(1024u) / unsigned long long(1024u);
			}
			constexpr unsigned long long toGB(unsigned long long bytes)
			{
				return bytes / unsigned long long(1024u) / unsigned long long(1024u) / unsigned long long(1024u);
			}
			constexpr unsigned long long toTB(unsigned long long bytes)
			{
				return bytes / unsigned long long(1024u) / unsigned long long(1024u) / unsigned long long(1024u) / unsigned long long(1024u);
			}

			constexpr unsigned long long operator "" _kb(unsigned long long _Val)
			{
				return _Val * unsigned long long(1024u);
			}

			constexpr unsigned long long operator "" _mb(unsigned long long _Val)
			{
				return _Val * unsigned long long(1024u) * unsigned long long(1024u);
			}

			constexpr unsigned long long operator "" _gb(unsigned long long _Val)
			{
				return _Val * unsigned long long(1024u) * unsigned long long(1024u) * unsigned long long(1024u);
			}

			constexpr unsigned long long operator "" _tb(unsigned long long _Val)
			{
				return _Val * unsigned long long(1024u) * unsigned long long(1024u) * unsigned long long(1024u) * unsigned long long(1024u);
			}
		}
			
	}
}
#endif //SE_UTILZ_MEMORY_H_