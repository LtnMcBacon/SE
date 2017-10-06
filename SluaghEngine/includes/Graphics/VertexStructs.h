#ifndef VERTEXSTRUCTS_H
#define VERTEXSTRUCTS_H

#include <d3d11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

struct Vertex { // Struct for an ordinary vertex layout without deformer data

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 binormal;
	DirectX::XMFLOAT3 tangent;
};

struct VertexDeformer { // Struct for a vertex layout with deformer data that holds the four influences (Weight pairs)

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 binormal;
	DirectX::XMFLOAT3 tangent;
	float weights[4];
	uint32_t boneIndices[4];
};

#endif