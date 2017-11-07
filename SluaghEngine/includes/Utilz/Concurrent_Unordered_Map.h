#ifndef SE_UTILZ_CONCURRENT_UNORDERED_MAP_H_
#define SE_UTILZ_CONCURRENT_UNORDERED_MAP_H_
#include <unordered_map>
#include <mutex>
#include "Concurrent.h"
namespace SE
{
	namespace Utilz
	{
		template<
			typename KEY,
			typename VAL,
			class Hash = std::hash<KEY>,
			class KeyEqual = std::equal_to<KEY>>
			class Concurrent_Unordered_Map
		{
			std::unordered_map<KEY, VAL, Hash, KeyEqual> object;
			std::mutex lock;
		public:
			template<class CON, class F, class... ARGS>
			friend void Operate(CON& con, const F& cFunc, ARGS&&... args);
			template<class CON, class KEY, class F, class... ARGS>
			friend void OperateSingle(CON& con, KEY& key, const F& cFunc, ARGS&&... args);
			template<class CON, class KEY, class F, class... ARGS>
			friend void OperateSingle(CON& con, KEY&& key, const F& cFunc, ARGS&&... args);
			template<class CON, class OBJ>
			friend void Copy(CON& con, OBJ& copy_to);
			template<class CON>
			friend void Clear(CON& con);
			template<class CON, class OBJ>
			friend void Move(CON& con, OBJ& copy_to);

			Concurrent_Unordered_Map() {};
			Concurrent_Unordered_Map(const Concurrent_Unordered_Map& other) = delete;
			Concurrent_Unordered_Map(const Concurrent_Unordered_Map&& other) = delete;
			void operator=(const Concurrent_Unordered_Map& other) = delete;
			void operator=(const Concurrent_Unordered_Map&& other) = delete;
		};
	}
}



#endif // SE_UTILZ_CONCURRENT_UNORDERED_MAP_H_