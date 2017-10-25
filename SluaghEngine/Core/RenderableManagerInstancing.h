#ifndef SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_
#define SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_
#include <Entity.h>
#include <Graphics\RenderJob.h>
#include <unordered_map>
#include <DirectXMath.h>
#include <Graphics\IRenderer.h>
namespace SE
{
	namespace Core
	{
		class RenderableManagerInstancing
		{
		public:
			RenderableManagerInstancing(Graphics::IRenderer* renderer);
			~RenderableManagerInstancing();

			void AddEntity(const Entity& entity, Graphics::RenderJob& job);
			void RemoveEntity(const Entity& entity);
			void UpdateTransform(const Entity& entity, const DirectX::XMFLOAT4X4& transform);
		private:
			Graphics::IRenderer* renderer;

			struct BucketAndID
			{
				Utilz::GUID bucket;
				size_t index;
			};
			struct RenderBucket
			{
				RenderBucket(const Graphics::Pipeline& p) : pipeline(p) {};
				Graphics::Pipeline pipeline;
				uint32_t jobID;
				std::vector<DirectX::XMFLOAT4X4> transforms;
				std::vector<Entity> indexToEntity;
			};

			void RemoveFromBucket(const BucketAndID& bucketAndID);

		

			std::unordered_map<Entity, BucketAndID, EntityHasher> entityToBucketAndIndexInBucket;
			std::unordered_map<Utilz::GUID, RenderBucket*, Utilz::GUID::Hasher> pipelineToRenderBucket;
		};
	}
}

#endif //SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_