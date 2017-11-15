#ifndef SE_INTERFACE_MANAGER_H_
#define SE_INTERFACE_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "IEntityManager.h"
#include <Utilz\TimeCluster.h>

namespace SE
{
	namespace Core
	{
		class IManager
		{
		public:
			virtual ~IManager() {};
			virtual void Frame(Utilz::TimeCluster* timer) = 0;

			/**
			* @brief	Remove an enitity
			*/
			virtual void Destroy(const Entity& entity) = 0;
		protected:
			IManager() {};
	
			IManager(const IManager& other) = delete;
			IManager(const IManager&& other) = delete;
			IManager& operator=(const IManager& other) = delete;
			IManager& operator=(const IManager&& other) = delete;

			/**
			* @brief	Remove an enitity entry
			*/
			virtual void Destroy(size_t index) = 0;

			/**
			* @brief	Look for dead entities.
			*/
			virtual void GarbageCollection() = 0;
		};
	}
}

#endif //SE_INTERFACE_MANAGER_H_