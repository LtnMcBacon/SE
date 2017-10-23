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
			inline size_t GetVirtualProcessMemory()
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;		
				BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				if( res != 0)
					return static_cast<size_t>(pmc.PrivateUsage); 
				return ~size_t(0u);
			}
			inline size_t GetPhysicalProcessMemory()
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;
				BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				if (res != 0)
					return static_cast<size_t>(pmc.WorkingSetSize); // WorkingSetSize (for physical memory)
				return ~size_t(0u);
			}
			inline bool IsUnderLimit(const size_t max, const size_t extra)
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;
				BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				if ( res != 0)
					return (static_cast<size_t>(pmc.WorkingSetSize) + extra) <= max;
				return false;
			}

			inline bool IsUnderLimit(const size_t max)
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;
				BOOL res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				if (res != 0)
					return (static_cast<size_t>(pmc.WorkingSetSize)) <= max;
				return false;
			}
			constexpr size_t toKB(size_t bytes)
			{
				return bytes / size_t(1024u);
			}
			constexpr size_t toMB(size_t bytes)
			{
				return bytes / size_t(1024u) / size_t(1024u);
			}
			constexpr size_t toGB(size_t bytes)
			{
				return bytes / size_t(1024u) / size_t(1024u) / size_t(1024u);
			}
			constexpr size_t toTB(size_t bytes)
			{
				return bytes / size_t(1024u) / size_t(1024u) / size_t(1024u) / size_t(1024u);
			}

			constexpr size_t operator "" _kb(unsigned long long _Val)
			{
				return static_cast<size_t>(_Val) * size_t(1024u);
			}

			constexpr size_t operator "" _mb(unsigned long long _Val)
			{
				return static_cast<size_t>(_Val) * size_t(1024u) * size_t(1024u);
			}

			constexpr size_t operator "" _gb(unsigned long long _Val)
			{
				return static_cast<size_t>(_Val) * size_t(1024u) * size_t(1024u) * size_t(1024u);
			}

			constexpr size_t operator "" _tb(unsigned long long _Val)
			{
				return static_cast<size_t>(_Val) * size_t(1024u) * size_t(1024u) * size_t(1024u) * size_t(1024u);
			}
		}
			
	}
}
#endif //SE_UTILZ_MEMORY_H_