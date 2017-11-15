#ifndef SE_CORE_ANIMATIONSHADOWSYSTEM_H_
#define SE_CORE_ANIMATIONSHADOWSYSTEM_H_

#include <unordered_map>

#include <DirectXMath.h>

#include <AnimationFileHeaders.h>
#include "AnimationStructs.h"
#include <Entity.h>
#include "RenderableManagerInstancing.h"

namespace SE
{
	namespace Core
	{
		class AnimationSystem;

		class AnimationShadowSystem : public  RenderableManagerInstancing
		{
		public:

			AnimationShadowSystem(Graphics::IRenderer* renderer, AnimationSystem* animSys);
			~AnimationShadowSystem();

			void AddEntity(const Entity& entity, Graphics::RenderJob& job, Graphics::RenderGroup group = Graphics::RenderGroup::RENDER_PASS_0) override;
			void RemoveEntity(const Entity& entity) override;
			void UpdateTransform(const Entity& entity, const DirectX::XMFLOAT4X4& transform) override;

		private:
			AnimationSystem* animSys;

			struct JointMatrices
			{
				DirectX::XMFLOAT4X4 jointMatrix[30];
			};
			static JointMatrices mats;

			struct AnimationBucket : public RenderBucket
			{
				AnimationBucket(const Graphics::Pipeline& p) : RenderableManagerInstancing::RenderBucket(p) {};
				std::vector<JointMatrices> matrices[2];
				virtual void AddEntity(const Entity& entity, const DirectX::XMFLOAT4X4& transform, RenderableManagerInstancing::BucketAndID& bucketAndID)override;
				virtual void RemoveFromBucket(RenderableManagerInstancing* rm, size_t index, DirectX::XMFLOAT4X4* transform)override;
			};
			uint8_t updateIndex = 0u;
			uint8_t mapingIndex = 1u;

			virtual RenderBucket* CreateBucket(Graphics::RenderJob & job)override;

		};

	}
}

#endif