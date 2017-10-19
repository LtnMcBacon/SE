#ifndef SE_UTILZ_GUID_H_
#define SE_UTILZ_GUID_H_
#include <stdint.h>
//#include "CompileTimeStringHasher.h"
//#include "RunTimeStringHasher.h"
#include <string>
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

			GUID() : id(0) {};
		//	constexpr GUID(const char* str) : id(std::hash<std::string>{}(str)) {};
			GUID(const char* str) : id(std::hash<std::string>{}(str)) {};
			GUID(const std::string& str) : id(std::hash<std::string>{}(str)) {};
			//GUID(uint32_t id) :id(id) {};
			GUID(const GUID& other) : id(other.id) {}
			GUID(const GUID&& other) : id(other.id) {}
			GUID& operator=(const GUID& other) { this->id = other.id; return *this; }
			GUID& operator+(const GUID &other) const
			{
				GUID g;
				g.id = 0.5f*(id + other.id)*(id + other.id + 1) + other.id;
				return g;
			}
		private:
			size_t id;			
		};


	}
}

#endif //SE_UTILZ_GUID_H_