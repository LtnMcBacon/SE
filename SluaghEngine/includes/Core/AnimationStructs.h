#ifndef ANIMATIONSTRUCTS_H
#define ANIMATIONSTRUCTS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>

#include <DirectXMath.h>

struct Joint_Keyframe {

	DirectX::XMMATRIX LocalTransform;
	float TimePos;
	DirectX::XMFLOAT4 Translation;
	DirectX::XMFLOAT4 Scale;
	DirectX::XMFLOAT4 RotationQuat;
};

struct Joint_Animation {

	std::vector<Joint_Keyframe> Keyframes;
	int Length;
};

struct Joint_Container {

	DirectX::XMMATRIX GlobalTx;
	DirectX::XMMATRIX inverseBindPoseMatrix;
	int parentIndex;
	std::vector<Joint_Animation> Animations;
};

#endif