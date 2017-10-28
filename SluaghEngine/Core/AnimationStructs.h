#ifndef ANIMATIONSTRUCTS_H
#define ANIMATIONSTRUCTS_H

#include <vector>

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
			
			Joint() {

				GlobalTx = DirectX::XMMatrixIdentity();
				inverseBindPoseMatrix = DirectX::XMMatrixIdentity();
				int parentIndex = 0;
			}
		};

		struct Skeleton {

			std::vector<Joint> Hierarchy;

			Skeleton() {
				Hierarchy.reserve(19);
			}
		};

	}

}

#endif