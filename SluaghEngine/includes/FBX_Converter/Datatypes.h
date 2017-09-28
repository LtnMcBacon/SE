#ifndef DATATYPES_H
#define DATATYPES_H

#include <d3d11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

#include <fbxsdk.h>

#include <vector>
#include <algorithm>
#include <unordered_map>

#include "VertexType.h"

using namespace std;
using namespace DirectX;

namespace SE
{
	namespace Core
	{

		struct Keyframe { // Stores the attributes of a keyframe in an animation

			FbxAMatrix GlobalTransform;
			float TimePos;
			XMFLOAT4 Translation;
			XMFLOAT4 Scale;
			XMFLOAT4 RotationQuat;

		};

		struct Animation { // Struct to hold a vector of keyframes and the length of the animation

			vector<Keyframe> Keyframes;
			FbxLongLong Length;
			string Name;
		};

		struct Joint { // Stores the attributes of a joint node

			string Name;
			int ParentIndex;

			FbxAMatrix GlobalBindposeInverse;
			FbxAMatrix LocalTransform;
			FbxAMatrix GlobalTransform;
			vector<Animation> Animations;

			FbxNode* Node;

			Joint() :

				Node(nullptr)

			{
				GlobalBindposeInverse.SetIdentity();
				ParentIndex = -1;

			}

		};

		struct BlendingIndexWeightPair { // Middle hand container to help with passing Vertex/Skinning pair data

			int BlendIndex;
			double BlendWeight;

			BlendingIndexWeightPair() :
				BlendIndex(0),
				BlendWeight(0)
			{}
		};

		struct Texture { // Struct to hold the texture name and its path

			string textureName;
			string texturePath;

			Texture() {

				textureName = "No name";
				texturePath = "No texture attached to this channel";
			}
		};

		struct Material // Struct to store material attributes
		{

			string materialName;

			XMFLOAT3 diffuseColor;
			float diffuseFactor;

			XMFLOAT3 ambientColor;
			float ambientFactor;

			XMFLOAT3 specularColor;
			float specularFactor;

			string materialType;

			vector<Texture> textures;

			Material() {

				textures.reserve(5);
			}
		};

		struct MaterialAttributes {

			XMFLOAT3 diffuseColor;
			float diffuseFactor;

			XMFLOAT3 ambientColor;
			float ambientFactor;

			XMFLOAT3 specularColor;
			float specularFactor;
		};

		struct TransformAttributes {

			XMFLOAT3 position;
			XMFLOAT3 rotation;
			XMFLOAT3 scale;
		};

		struct LightAttributes {

			XMFLOAT3 color;
			XMFLOAT3 position;
			XMFLOAT3 direction;
		};

		struct ControlPoint { // Resembles a physical vertex point in the FBX SDK

			XMFLOAT3 Position;
			vector<BlendingIndexWeightPair> BlendingInfo;

			ControlPoint() {

				BlendingInfo.reserve(4);
			}

		};

		struct Skeleton { // Stores every joint in a skeleton hierarchy from the loaded FBX file

			vector<Joint> hierarchy;
			int hierarchyDepth;

		};

		struct BBox {

			XMFLOAT3 max;
			XMFLOAT3 min;
		};

		struct Mesh { // Extended node type to hold both the FBX mesh node and its vertices

			FbxMesh* meshNode;
			FbxSkin* skinNode;

			TransformAttributes transformAttributes;

			unordered_map<unsigned int, ControlPoint*>controlPoints;
			int vertexLayout;
			vector<Vertex>standardVertices;
			vector<VertexDeformer>boneVertices;

			Skeleton skeleton;

			string name;
			Material objectMaterial;
			BBox bboxValues;

			Mesh() {

				meshNode = nullptr;
				skinNode = nullptr;
			}

		};

		struct Light {

			FbxLight* lightNode;
			string name;
			XMFLOAT3 color;
			XMFLOAT3 position;
			XMFLOAT3 direction;
		};

	}

}

#endif DATATYPES_H

