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

			void AddEntity(const Entity& entity, const Graphics::RenderJob& job);

		private:
			struct RenderBucket
			{
				Graphics::Pipeline pipeline;
				std::vector<DirectX::XMFLOAT4X4> transforms;
			};

			struct RenderBucketAndID
			{
				size_t bucket;
				size_t ID;
			};

			std::vector<RenderBucket> renderBuckets;
			std::unordered_map<Entity, RenderBucketAndID, Core::EntityHasher> entityToRenderBucketAndID;
		};
	}
}

#endif //SE_CORE_RENDERABLE_MANAGER_INSTANCING_H_