#ifndef SE_UTILZ_CONCURRENT_H_
#define SE_UTILZ_CONCURRENT_H_
#include <mutex>

namespace SE
{
	namespace Utilz
	{
		template<class T>
		class Concurrent
		{
			T object;
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

			Concurrent() {};
			Concurrent(const Concurrent& other) = delete;
			Concurrent(const Concurrent&& other) = delete;
			void operator=(const Concurrent& other) = delete;
			void operator=(const Concurrent&& other) = delete;
		};


		template<class CON, class F, class... ARGS>
		inline void Operate(CON& con, const F& cFunc, ARGS&&... args)
		{
			con.lock.lock();
			cFunc(con.object, std::forward<ARGS>(args)...);
			con.lock.unlock();
		}
		template<class CON, typename KEY, class F, class... ARGS>
		inline void OperateSingle(CON& con, KEY& key, const F& cFunc, ARGS&&... args)
		{
			con.lock.lock();
			cFunc(con.object[key], std::forward<ARGS>(args)...);
			con.lock.unlock();
		}
		template<class CON, class KEY, class F, class... ARGS>
		inline void OperateSingle(CON& con, KEY&& key, const F& cFunc, ARGS&&... args)
		{
			con.lock.lock();
			cFunc(con.object[key], std::forward<ARGS>(args)...);
			con.lock.unlock();
		}

		template<class CON, class OBJ>
		inline void Copy(CON& con, OBJ& copy_to)
		{
			con.lock.lock();
			copy_to.resize(con.object.size());
			std::copy(con.object.begin(), con.object.end(), copy_to.begin());
			con.lock.unlock();
		}
		template<class CON>
		inline void Clear(CON& con)
		{
			con.lock.lock();
			con.object.clear();
			con.lock.unlock();
		}
		template<class CON, class OBJ>
		inline void Move(CON& from, OBJ& copy_to)
		{
			from.lock.lock();
			copy_to = std::move(from.object);
			from.object.clear();
			from.lock.unlock();
		}

		template<class CON, class T>
		inline void Push_Back(CON& con, const T&& ele)
		{
			con.lock.lock();
			con.object.push_back(ele);
			con.lock.unlock();
		}
	}
}


#endif //SE_UTILZ_CONCURRENT_H_