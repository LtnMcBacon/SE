#ifndef FILEHEADERS_H
#define FILEHEADERS_H

#include <d3d11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

namespace SE
{
	namespace Graphics
	{

		struct Mesh_Header {

			uint32_t vertexLayout;
			uint32_t nrOfVertices;
		};

		struct TransformAttributes {

			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 rotation;
			DirectX::XMFLOAT3 scale;
		};

		struct BBox {

			DirectX::XMFLOAT3 max;
			DirectX::XMFLOAT3 min;
		};


	
		struct Material_Header {

			uint32_t nrOfTextures;
		};

		struct MaterialAttributes {

			DirectX::XMFLOAT3 diffuseColor;
			float diffuseFactor;

			DirectX::XMFLOAT3 ambientColor;
			float ambientFactor;

			DirectX::XMFLOAT3 specularColor;
			float specularFactor;
		};



		struct Light_Header {

			uint32_t nrOfLights;
		};

		struct LightAttributes {

			DirectX::XMFLOAT3 color;
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 direction;
		};

	}

}

#endif
