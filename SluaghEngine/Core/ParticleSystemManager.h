#ifndef SE_CORE_PARTICLE_SYSTEM_MANAGER_H_
#define SE_CORE_PARTICLE_SYSTEM_MANAGER_H_

#include <IParticleSystemManager.h>
#include <map>
#include <vector>
#include <Utilz\CircularFiFo.h>
#include <Utilz\Timer.h>
#include <unordered_map>
#include <random>
#include <Particle_Editor\ParticleEmitter.h>
#include <Utilz\Sofa.h>

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
			void CreateSystem(const Entity& entity, const CreateInfo& info)override;

			/**
			 * @brief Update the start and end positions of a system.
			 */
			void UpdateSystemEndPosition(const Entity& entity, float endPos[3]) override;

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
			/**
			* @brief	Remove an enitity
			*/
			void Destroy(const Entity& entity)override;

		private:
			InitializationInfo initInfo;
			Graphics::Pipeline updatePipeline;
			Utilz::Timer time;
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index)override;
		
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection()override;
			/**
			* @brief	Updates emit position for the particles
			*/
			void UpdateDirtyPos(const Entity& entity, size_t index);

			std::default_random_engine generator;

			struct ParticleSystemFileInfo
			{
	
				float vel[3];
				float pad;
				float emitPos[3];
				float pad1;
				float emitRange[3];
				float pad5;
				float gravity[3];
				float pad4;
				float startPos[3];
				float pad6;
				float endPos[3];
				float pad7;
				float speed;
				float emitRate;
				float lifeTime;
				float tangentValue;
				float radialValue;
				float gravityValue;
				float pSize;
				float dt;
				unsigned int circular;
				unsigned int gravityCheck;
				unsigned int emit;
				unsigned int particlePath;
			};
			struct ParticleSystemData
			{
				bool firstRun;
				DirectX::XMFLOAT4X4 transform;
				uint8_t visible;
				uint8_t loaded;
				ParticleSystemFileInfo particleFileInfo;
				bool randVelocity;
				Utilz::GUID textureName;
				Graphics::RenderJob updateJob;
				Graphics::RenderJob renderJob;
				uint32_t updateJobID;
				uint32_t renderJobID;
				//Each index contains a XMFLOAT2 with min and max values for velocity and emit in the order of x, y and z. X and Y is Max and Min respectively in each vector
				DirectX::XMFLOAT2 velocityRange[3]; 
				DirectX::XMFLOAT2 emitRange[3];
			};

			struct DirtyEntityInfo {
				size_t transformIndex;
				Entity entity;
			};
			std::vector<DirtyEntityInfo> dirtyEntites;
			std::vector<ParticleSystemData> particleSystemData;
			std::vector<Entity> indexToEntity;
			std::unordered_map<Entity, size_t, Core::EntityHasher> entityToIndex;

			struct VelocityEmitRange
			{
				DirectX::XMFLOAT2 velocity[3];
				DirectX::XMFLOAT2 emit[3];
			};
			Utilz::Sofa<Entity, EntityHasher,
				DirectX::XMFLOAT4X4, // transform
				uint8_t, // visible
				ParticleSystemFileInfo, // FileInfo
				bool, // randVelocity
				Utilz::GUID, // textureName
				//Graphics::RenderJob//, // Updatejob
				//Graphics::RenderJob, // renderJob
				uint32_t, // UpdatejobId
				uint32_t, // renderJobId
				VelocityEmitRange // VelocityEmitRange
			> entries;

			enum Entries
			{
				entity,
				transform,
				visible,
				fileInfo,
				randVelocity,
				textureName,
				updateJob,
				renderJob,
				updateJobID,
				renderJobID,
				velocityEmitRange,
			};

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