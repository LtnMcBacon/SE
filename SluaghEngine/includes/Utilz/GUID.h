#ifndef SE_UTILZ_GUID_H_
#define SE_UTILZ_GUID_H_
#include <stdint.h>
#include "CompileTimeStringHasher.h"
#include "RunTimeStringHasher.h"
namespace SE
{
	namespace Utilz
	{
		class GUID
		{
		public:
			struct Compare
			{
				inline bool operator() (const GUID& lhs, const GUID& rhs) const
				{
					return lhs.id < rhs.id;
				}
			};

			constexpr GUID(const char* str) : id(COMPILE_TIME_CRC32_STR(str)) {};
			GUID(const std::string& str) : id(RUN_TIME_CRC32_STR(str.c_str())) {};
			GUID(uint32_t id) : id(id) {}
			GUID(const GUID& other) : id(other.id) {}
			GUID(const GUID&& other) : id(other.id) {}
			GUID& operator=(const GUID& other) { this->id = other.id; }
		private:
			uint32_t id;

			
		};


	}
}

#endif //SE_UTILZ_GUID_H_