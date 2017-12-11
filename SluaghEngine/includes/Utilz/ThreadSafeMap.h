#ifndef SE_UTILZ_THREADSAFE_MAP_H
#define SE_UTILZ_THREADSAFE_MAP_H
#include <shared_mutex>
#include <unordered_map>
namespace SE
{
	namespace Utilz
	{

		/*
		 * @brief An unordered map wrapped with a mutex lock.
		 * @details Allows for multiple readers, single writer. Two threads accessing
		 *  and changing the same element is not safe. Only guarantee is that reading one entry
		 *  from the map and writing another will not affect each other.
		 */
		template <typename Key, typename Value, typename Hasher>
		class ThreadSafeMap
		{
		public:
			Value& operator[](const Key& key)
			{
				std::unique_lock<std::shared_mutex> lock(mutex);
				return umap[key];
			}
			typename std::unordered_map<Key, Value, Hasher>::iterator find(const Key& key)
			{
				std::shared_lock<std::shared_mutex> lock(mutex);
				return umap.find(key);
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
				std::unique_lock<std::shared_mutex> lock(mutex);
				umap.erase(it);
			}
			void erase(const Key& key)
			{
				std::unique_lock<std::shared_mutex> lock(mutex);
				umap.erase(key);
			}
		private:
			std::shared_mutex mutex;
			std::unordered_map<Key, Value, Hasher> umap;
		};
	}
}


#endif

