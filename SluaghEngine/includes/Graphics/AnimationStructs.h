#ifndef ANIMATIONSTRUCTS_H
#define ANIMATIONSTRUCTS_H

#include <vector>

#include <DirectXMath.h>

namespace SE 
{

	namespace Graphics
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

		struct Animation {

			std::vector<Keyframe> Keyframes;
			unsigned int Length;

			Animation() {

				Keyframes.reserve(60);
				Length = 0;
			}
		};

		struct Joint {

			DirectX::XMMATRIX GlobalTx;
			DirectX::XMMATRIX LocalTx;
			DirectX::XMMATRIX inverseBindPoseMatrix;
			unsigned int parentIndex;
			std::vector<Animation> Animations;

			Joint() {

				GlobalTx = DirectX::XMMatrixIdentity();
				inverseBindPoseMatrix = DirectX::XMMatrixIdentity();
				int parentIndex = 0;
				Animations.reserve(3);
			}
		};

		struct Skeleton {

			std::vector<Joint> Hierarchy;
			std::vector<DirectX::XMFLOAT4X4> jointArray;

			Skeleton() {

				Hierarchy.reserve(30);
				jointArray.resize(30);
			}
		};

	}

}

#endif