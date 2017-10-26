#ifndef SE_CORE_ANIMATIONSYSTEM_H_
#define SE_CORE_ANIMATIONSYSTEM_H_

#include <vector>

#include <DirectXMath.h>

#include <AnimationFileHeaders.h>
#include "AnimationStructs.h"

namespace SE
{
	namespace Core
	{

		class AnimationSystem {

		public:

			AnimationSystem();
			~AnimationSystem();

			int AddSkeleton(JointAttributes* jointData, size_t nrOfJoints, int *skeletonID);
			int AddAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints, int *animationID);

			void UpdateAnimation(int animIndex, int skeletonIndex, float timePos, DirectX::XMFLOAT4X4* at);
			void CalculateJointMatrix(int jointIndex, const Animation& animation, float animTimePos, DirectX::XMMATRIX& out) const;
			

			inline Skeleton& GetSkeleton(int index) { return skeletons[index]; };

		private:
			void ReturnFirstFrameMatrix(const JointKeyFrame& joint, DirectX::XMMATRIX& out) const;
			void ReturnLastFrameMatrix(const JointKeyFrame& joint, const Animation& animation, DirectX::XMMATRIX& out) const;
			void Interpolate(const JointKeyFrame& joint, float animTimePos, DirectX::XMMATRIX& out) const;

			std::vector<Skeleton> skeletons;
			std::vector<Animation> animations;
		};

	}
}

#endif