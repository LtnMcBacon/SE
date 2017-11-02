#ifndef SE_CORE_PARTICLE_SYSTEM_MANAGER_H_
#define SE_CORE_PARTICLE_SYSTEM_MANAGER_H_

#include <IParticleSystemManager.h>
#include <map>
#include <vector>
#include <Utilz\CircularFiFo.h>
#include <unordered_map>
#include <random>
namespace SE
{
	namespace Core
	{
		class ParticleSystemManager : public IParticleSystemManager
		{
		public:

			ParticleSystemManager(const IParticleSystemManager::InitializationInfo& initInfo);
			~ParticleSystemManager();


			/**
			* @brief	Create a particle system.
			* @param [in] entity The entity to bind system to.
			* @param [in] info The info used when creating the system.
			* @param [in] async If the resource should be streamed.
			* @param [in] behavior The streaming behavior.
			* @sa CreateInfo
			*/
			void CreateSystem(const Entity& entity, const CreateInfo& info, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK)override;

			/**
			* @brief	Hide/Show a particle system.
			* @param [in] entity The entity to bind system to.
			* @param [in] visible If the resource should be streamed.
			*/
			void ToggleVisible(const Entity& entity, bool visible)override;

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;

		private:
			InitializationInfo initInfo;

			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index)override;
			/**
			* @brief	Remove an enitity
			*/
			void Destroy(const Entity& entity)override;

			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection()override;
			std::default_random_engine generator;

			struct ParticleSystemFileInfo
			{

			};
			struct ParticleSystemData
			{
				uint8_t visible;
				uint8_t loaded;
				ParticleSystemFileInfo particleFileInfo;
			};
			std::vector<ParticleSystemData> particleSystemData;
			std::vector<Entity> indexToEntity;
			std::unordered_map<Entity, size_t, Core::EntityHasher> entityToIndex;

			struct toUpdateStruct
			{
				Entity entity;
				ParticleSystemFileInfo info;
			};
			Utilz::CircularFiFo<toUpdateStruct> toUpdate;

		};
	}
}


#endif //SE_CORE_PARTICLE_SYSTEM_MANAGER_H_