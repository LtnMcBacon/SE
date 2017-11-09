#ifndef SE_UTILZ_CONCURRENT_VECTOR_H_
#define SE_UTILZ_CONCURRENT_VECTOR_H_
#include <vector>
#include <mutex>
#include "Concurrent.h"
namespace SE
{
	namespace Utilz
	{
		template<typename T>
		class Concurrent_Vector
		{
			std::vector<T> object;
			std::mutex lock;
		public:
			template<class CON, class T>
			inline void Push_Back(const T&& ele)
			{
				lock.lock();
				object.push_back(ele);
				lock.unlock();
			}
			inline void Push_Back(const T& ele)
			{
				lock.lock();
				object.push_back(ele);
				lock.unlock();
			}
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

			Concurrent_Vector() {};
			Concurrent_Vector(const Concurrent_Vector& other) = delete;
			Concurrent_Vector(const Concurrent_Vector&& other) = delete;
			void operator=(const Concurrent_Vector& other) = delete;
			void operator=(const Concurrent_Vector&& other) = delete;
		};
	}
}



#endif // SE_UTILZ_CONCURRENT_VECTOR_H_