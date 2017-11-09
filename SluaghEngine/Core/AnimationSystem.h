#ifndef SE_CORE_ANIMATIONSYSTEM_H_
#define SE_CORE_ANIMATIONSYSTEM_H_

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

		class AnimationSystem : public  RenderableManagerInstancing 
		{
		public:

			AnimationSystem(Graphics::IRenderer* renderer);
			~AnimationSystem();

			int AddSkeleton(const Utilz::GUID& guid, JointAttributes* jointData, size_t nrOfJoints);
			bool IsSkeletonLoaded(const Utilz::GUID& guid)const;

			int AddAnimation(const Utilz::GUID& guid, DirectX::XMFLOAT4X4* matrices, uint32_t* joints, size_t nrOfKeyframes, size_t nrOfJoints);
			bool IsAnimationLoaded(const Utilz::GUID& guid)const;

			void CalculateMatrices(const Entity& entity, AnimationInfo& info, bool threaded);

			void CalculateBlendMatrices(const DirectX::XMMATRIX& matrix1, const DirectX::XMMATRIX& matrix2, float blendFactor, DirectX::XMFLOAT4X4& out);

			int FindJointIndex(Utilz::GUID skeleton, Utilz::GUID jointNameToFind);

			void GetJointMatrix(const Entity& entity, int jointIndex, DirectX::XMFLOAT4X4& matrix);
			
			inline void UpdateMatricesIndex() {
				updateIndex = (updateIndex + 1) % 2;
				mapingIndex = (mapingIndex + 1) % 2;
			};

		private:
			void UpdateAnimation(const Animation& animation, const Skeleton& skeleton, float timePos, DirectX::XMFLOAT4X4* at);

			void CalculateJointMatrix(int jointIndex, const Animation& animation, float animTimePos, DirectX::XMMATRIX& out) const;

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