#ifndef SE_UTILZ_THREADSAFE_MAP_H
#define SE_UTILZ_THREADSAFE_MAP_H
#include <mutex>
#include <unordered_map>
namespace SE
{
	namespace Utilz
	{


		template <typename Key, typename Value, typename Hasher>
		class ThreadSafeMap
		{
		public:
			Value& operator[](const Key& key)
			{
				lock.lock();
				auto& entry = umap[key];
				lock.unlock();
				return entry;
			}
			typename std::unordered_map<Key, Value, Hasher>::iterator find(const Key& key)
			{
				lock.lock();
				auto it = umap.find(key);
				lock.unlock();
				return it;
			}

			typename std::unordered_map<Key, Value, Hasher>::iterator end()
			{
				return umap.end();
			}

			typename std::unordered_map<Key, Value, Hasher>::iterator begin()
			{
				return umap.begin();
			}


			void erase(typename std::unordered_map<Key, Value, Hasher>::iterator it)
			{
				lock.lock();
				umap.erase(it);
				lock.unlock();
			}
			void erase(const Key& key)
			{
				lock.lock();
				umap.erase(key);
				lock.unlock();
			}
		private:
			std::mutex lock;
			std::unordered_map<Key, Value, Hasher> umap;
		};
	}
}


#endif

