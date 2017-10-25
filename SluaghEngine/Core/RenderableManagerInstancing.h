#ifndef SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_
#define SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_
#include <Entity.h>
#include <Graphics\RenderJob.h>
#include <unordered_map>
#include <DirectXMath.h>
namespace SE
{
	namespace Core
	{
		class RenderableManagerInstancing
		{
		public:
			RenderableManagerInstancing();
			~RenderableManagerInstancing();

			Utilz::GUID AddEntity(const Entity& entity, const Graphics::RenderJob& job);

		private:



			struct RenderBucket
			{
				Graphics::Pipeline pipeline;
				std::vector<DirectX::XMFLOAT4X4> transforms;
				std::vector<Entity> indexToEntity;
			};
	
			struct BucketAndID
			{
				Utilz::GUID bucket;
				size_t index;
			};

			std::unordered_map<Entity, BucketAndID, EntityHasher> entityToBucketAndIndexInBucket;
			std::unordered_map<Utilz::GUID, RenderBucket, Utilz::GUID::Hasher> pipelineToRenderBucket;
		};
	}
}

#endif //SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_