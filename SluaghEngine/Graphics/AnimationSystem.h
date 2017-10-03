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
			int AddAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints, size_t skeletonIndex, int *animationID);

			void UpdateAnimation(int animIndex, Skeleton &skeleton, float timePos);
			DirectX::XMFLOAT4X4 CalculateJointMatrix(int jointIndex, int animIndex, Skeleton &skeleton, float animTimePos);
			DirectX::XMFLOAT4X4 ReturnFirstFrameMatrix(int jointIndex, int animIndex, Skeleton &skeleton);
			DirectX::XMFLOAT4X4 ReturnLastFrameMatrix(int jointIndex, int animIndex, Skeleton &skeleton);
			DirectX::XMFLOAT4X4 Interpolate(int jointIndex, int animIndex, Skeleton &skeleton, float animTimePos);

		private:

			std::vector<Skeleton> skeletons;

		};

	}
}

#endif