#ifndef SE_CORE_ANIMATIONSYSTEM_H_
#define SE_CORE_ANIMATIONSYSTEM_H_

#include <unordered_map>

#include <DirectXMath.h>

#include <AnimationFileHeaders.h>
#include "AnimationStructs.h"
#include <Utilz\GUID.h>
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

			int AddAnimation(const Utilz::GUID& guid, DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints);
			bool IsAnimationLoaded(const Utilz::GUID& guid)const;

			void UpdateAnimation(const Utilz::GUID& skeleton, const Utilz::GUID& animation, float timePos, DirectX::XMFLOAT4X4* at);
			void CalculateJointMatrix(int jointIndex, const Animation& animation, float animTimePos, DirectX::XMMATRIX& out) const;
			

			void MapBuffer(void* data, int done, int toDraw)const;
		private:
			void ReturnFirstFrameMatrix(const JointKeyFrame& joint, DirectX::XMMATRIX& out) const;
			void ReturnLastFrameMatrix(const JointKeyFrame& joint, const Animation& animation, DirectX::XMMATRIX& out) const;
			void Interpolate(const JointKeyFrame& joint, float animTimePos, DirectX::XMMATRIX& out) const;


		
			std::unordered_map<Utilz::GUID, Skeleton, Utilz::GUID::Hasher> skeletons;
			std::unordered_map<Utilz::GUID, Animation, Utilz::GUID::Hasher> animations;
		};

	}
}

#endif