#ifndef FBXCONVERTER_H
#define FBXCONVERTER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <fbxsdk.h>

#include "Datatypes.h"
#include "VertexType.h"

#include <filesystem>

using namespace std;
using namespace std::experimental::filesystem;

class FBXConverter {

public:

	FBXConverter();
	~FBXConverter();

	void ReleaseAll(FbxManager* gFbxSdkManager);
	void Deallocate();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// FBXCONVERTER LOAD FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	bool Load(string fileName, string exportFolder);
	bool LoadFBXFormat(string mainFileName, string exportFolder);
	bool LoadSceneFile(string fileName, FbxManager* gFbxSdkManager, FbxImporter* pImporter, FbxScene* pScene);
	bool InitializeFbxManager();
	bool InitializeSceneImporter();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// FBXCONVERTER PRIMARY FUNCTIONS IN ORDER
	//----------------------------------------------------------------------------------------------------------------------------------//

	void GetMeshes();
	void GetLights();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// MESH FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	void ProcessControlPoints(Mesh &pMesh);
	void CheckSkinNode(Mesh &pMesh);

	void CreateVertexDataStandard(Mesh &pMesh, FbxNode* pFbxRootNode);
	void CreateVertexDataBone(Mesh &pMesh, FbxNode* pFbxRootNode);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SKELETON/SKINNING FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	void GetSkeletonHierarchy(Mesh &pMesh);
	void RecursiveDepthFirstSearch(FbxNode* node, Mesh &pMesh, int depth, int index, int parentIndex);
	void CreateBindPose(Mesh &pMesh);
	void GatherWeights(Mesh &pMesh);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// ANIMATION FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	void GatherAnimationData(Mesh &pMesh);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// MATERIAL/TEXTURE FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	void LoadMaterial(Mesh& pMesh);
	void GetChannelTexture(Mesh& pMesh, FbxProperty materialProperty);
	bool ExportTexture(Texture &texture, string textureFolder);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// EXPORT FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	void Write();
	void WriteMaterial(string folderName, string textureFolder, Material& meshMaterial);
	void WriteMesh(string folderName, Mesh& mesh);
	void WriteSkeleton(string folderName, Skeleton skeleton, string meshName);
	void WriteAnimation(string folderName, Skeleton skeleton);
	void WriteLights(string folderName);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// HELPER FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	FbxAMatrix		 GetGeometryTransformation(FbxNode* node);
	unsigned int	 FindJointIndexByName(std::string& jointName, Skeleton skeleton);
	void			 ConvertToLeftHanded(FbxAMatrix &matrix);
	FbxMesh*		 GetMeshFromRoot(FbxNode* node, string meshName);
	XMFLOAT4X4		 Load4X4Transformations(FbxAMatrix fbxMatrix);
	void			 Print4x4Matrix(FbxAMatrix fbxMatrix);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// STRING HELPER FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	string getFilename(string const& path);
	string removeExtension(const string& path);

private:

	string fileName;
	string folderName;
	string logFolder;

	FbxManager* gFbxSdkManager;
	FbxIOSettings* pIOsettings;
	FbxScene* pFbxScene;
	FbxImporter* pImporter;
	FbxNode* pFbxRootNode;

	ofstream logFile;

	vector<Mesh> meshes;
	vector<Light> lights;

};


#endif FBXCONVERTER_H

