#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>

#include <DirectXMath.h>

#include <Core\FileHeaders.h>
#include "AnimationStructs.h"

namespace SE
{
	namespace Graphics
	{

		class AnimationSystem {

		public:

			AnimationSystem();
			~AnimationSystem();

			int AddSkeleton(SE::Core::JointAttributes* jointData, size_t nrOfJoints, int *skeletonID);
			int AddAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints, size_t skeletonIndex, int *animationID);

		private:

			std::vector<Skeleton> skeletons;

		};

	}
}

#endif