#ifndef SE_CORE_DECAL_MANAGER_H_
#define SE_CORE_DECAL_MANAGER_H_

#include "IDecalManager.h"
#include <unordered_map>
#include <random>

namespace SE
{
	namespace Core
	{
		
		class DecalManager : public IDecalManager
		{
		public:
			DecalManager(const IDecalManager::InitializationInfo& initInfo);
			~DecalManager();
			void Frame(Utilz::TimeCluster* timer) override;

			/*@brief See IDecalManager
			*/
			int Create(const Entity& entity, const DecalCreateInfo& createInfo) override;

			/*@brief See IDecalManager
			 */
			int SetLocalTransform(const Entity& entity, float* transform16rowMajor) override;
			
			/*@brief See IDecalManager
			*/
			int SetOpacity(const Entity& entity, float opacity) override;

			/*@brief See IDecalManager
			*/
			int ModifyOpacity(const Entity& entity, float amount) override;
			
			/*
			* @brief See IDecalManager
			*/
			int Remove(const Entity& entity) override;

		private:
			IDecalManager::InitializationInfo initInfo;

			Utilz::GUID vertexShader;
			Utilz::GUID pixelShader;
			Utilz::GUID worldConstantBuffer;
			Utilz::GUID inverseViewProj;
			Utilz::GUID inverseWorld;
			Utilz::GUID opacities;
			Utilz::GUID blendState;
			Utilz::GUID textureBindName;
			Utilz::GUID rasterizerState;
			/**<The base pipeline, entities will have different textures but the rest is the same*/
			Graphics::Pipeline defaultPipeline;
			struct DecalTransforms
			{
				std::vector<DirectX::XMFLOAT4X4> localTransform;
				std::vector<DirectX::XMFLOAT4X4> world;
				std::vector<DirectX::XMFLOAT4X4> inverseWorld;
				std::vector<Entity> owners;
				std::vector<float> opacity;
			};
			DirectX::XMMATRIX cachedViewProj;
			std::unordered_map<Entity, Utilz::GUID, EntityHasher> entityToTextureGuid;
			std::unordered_map<Entity, uint32_t, EntityHasher> entityToTransformIndex;
			std::unordered_map<Utilz::GUID, DecalTransforms, Utilz::GUID::Hasher> decalToTransforms;
			std::unordered_map<Utilz::GUID, uint32_t, Utilz::GUID::Hasher> decalToJobID;
			std::vector<Entity> entities;
			std::default_random_engine generator;

			void SetDirty(const Entity& entity, size_t index);
		protected:

			void Destroy(size_t index) override;

			void Destroy(const Entity& entity) override;

			void GarbageCollection() override;
		};


	}
}


#endif
