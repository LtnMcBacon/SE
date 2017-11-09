#ifndef ANIMATIONSTRUCTS_H
#define ANIMATIONSTRUCTS_H

#include <vector>
#include <Utilz\GUID.h>
#include <DirectXMath.h>

namespace SE 
{

	namespace Core
	{

		struct Keyframe {

			DirectX::XMMATRIX LocalTransform;
			float TimePos;
			DirectX::XMFLOAT4 Translation;
			DirectX::XMFLOAT4 Scale;
			DirectX::XMFLOAT4 RotationQuat;

			Keyframe() {

				LocalTransform = DirectX::XMMatrixIdentity();
				TimePos = 0;
				Translation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
				Scale = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
				RotationQuat = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			}
		};

		struct JointKeyFrame
		{
			std::vector<Keyframe> Keyframes;

			JointKeyFrame()
			{
				Keyframes.reserve(60);
			}
		};

		struct Animation {

			std::vector<JointKeyFrame> Joints;
			std::vector<uint32_t> jointToActualJoint;
			unsigned int Length;

			Animation() {
				Joints.reserve(19);
				Length = 0;
			}
		};

		struct Joint {

			DirectX::XMMATRIX GlobalTx;
			DirectX::XMMATRIX LocalTx;
			DirectX::XMMATRIX inverseBindPoseMatrix;
			unsigned int parentIndex;
			Utilz::GUID jointName;
			
			
			Joint() {

				GlobalTx = DirectX::XMMatrixIdentity();
				inverseBindPoseMatrix = DirectX::XMMatrixIdentity();
				parentIndex = 0;
			}
		};

		struct Skeleton {

			std::vector<Joint> Hierarchy;

			Skeleton() {
				Hierarchy.reserve(19);
			}
		};

		static const size_t maxLayers = 6;
		struct BlendBackInfo {

			int animIndex = 0;
			float animLength = 0.0;
			float previousSpeed[maxLayers] = {0.0};

		};
		
		struct AnimationInfo
		{
			size_t nrOfLayers = 0;
			Utilz::GUID skeleton = {""};
			Utilz::GUID animation[maxLayers] = {""};
			float timePos[maxLayers] = {0.0};
			float animationSpeed[maxLayers] = {0.0};
			bool looping[maxLayers] = {false};
			float blendFactor[maxLayers] = {0.0};
			float blendSpeed[maxLayers] = {0.0};
			bool toBlendTarget = false;
			bool toBlendSource = false;
			BlendBackInfo blendBackInfo;
		};

	}

}

#endif