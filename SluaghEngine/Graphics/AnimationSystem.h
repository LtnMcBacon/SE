#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>

#include <DirectXMath.h>

#include <Graphics\FileHeaders.h>
#include <Graphics\AnimationStructs.h>

namespace SE
{
	namespace Graphics
	{

		class AnimationSystem {

		public:

			AnimationSystem();
			~AnimationSystem();

			int AddSkeleton(JointAttributes* jointData, size_t nrOfJoints, int *skeletonID);
			int AddAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints, int *animationID);

			void UpdateAnimation(int animIndex, int skeletonIndex, float timePos, DirectX::XMFLOAT4X4* at);
			void CalculateJointMatrix(int jointIndex, const Animation& animation, const Skeleton &skeleton, float animTimePos, DirectX::XMFLOAT4X4& out);
			void ReturnFirstFrameMatrix(const JointKeyFrame& joint, DirectX::XMFLOAT4X4& out);
			void ReturnLastFrameMatrix(const JointKeyFrame& joint, const Animation& animation, DirectX::XMFLOAT4X4& out);
			void Interpolate(const JointKeyFrame& joint, float animTimePos, DirectX::XMFLOAT4X4& out);

			inline Skeleton& GetSkeleton(int index) { return skeletons[index]; };

		private:

			std::vector<Skeleton> skeletons;
			std::vector<Animation> animations;
		};

	}
}

#endif