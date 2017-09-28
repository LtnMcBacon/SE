#ifndef VERTEXTYPE_H
#define VERTEXTYPE_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

namespace SE
{
	namespace Core
	{

		struct Vertex { // Struct for an ordinary vertex layout without deformer data

			XMFLOAT3 pos;
			XMFLOAT2 uv;
			XMFLOAT3 normal;
			XMFLOAT3 binormal;
			XMFLOAT3 tangent;

		};

		struct VertexDeformer { // Struct for a vertex layout with deformer data that holds the four influences (Weight pairs)

			XMFLOAT3 pos;
			XMFLOAT2 uv;
			XMFLOAT3 normal;
			XMFLOAT3 binormal;
			XMFLOAT3 tangent;
			float weights[4];
			uint32_t boneIndices[4];
		};

	}

}

#endif
