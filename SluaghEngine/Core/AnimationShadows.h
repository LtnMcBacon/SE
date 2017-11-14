#ifndef SE_CORE_ANIMATIONSYSTEM_H_
#define SE_CORE_ANIMATIONSYSTEM_H_

#include <unordered_map>

#include <DirectXMath.h>

#include <AnimationFileHeaders.h>
#include "AnimationStructs.h"
#include <Entity.h>
#include "RenderableManagerInstancing.h"
#include "AnimationSystem.h"

namespace SE
{
	namespace Core
	{

		class AnimationShadowSystem : public  RenderableManagerInstancing
		{
		public:

			AnimationShadowSystem(Graphics::IRenderer* renderer);
			~AnimationShadowSystem();

		

		private:
			AnimationSystem* dicks;
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

			void ReturnFirstFrameMatrix(const JointKeyFrame& joint, DirectX::XMMATRIX& out) const;
			void ReturnLastFrameMatrix(const JointKeyFrame& joint, const Animation& animation, DirectX::XMMATRIX& out) const;
			void Interpolate(const JointKeyFrame& joint, float animTimePos, DirectX::XMMATRIX& out) const;

			std::unordered_map<Utilz::GUID, Skeleton, Utilz::GUID::Hasher> skeletons;
			std::unordered_map<Utilz::GUID, Animation, Utilz::GUID::Hasher> animations;
		};

	}
}

#endif