
#include <FBXConverter.h>

#pragma comment(lib, "libfbxsdk.lib")

using namespace std;
using namespace std::experimental::filesystem;

SE::Core::FBXConverter::FBXConverter() {

	gFbxSdkManager = nullptr;
	pIOsettings = nullptr;
	pFbxScene = nullptr;
	pImporter = nullptr;
	pFbxRootNode = nullptr;
}

SE::Core::FBXConverter::~FBXConverter() {
	
	
}

void SE::Core::FBXConverter::ReleaseAll(FbxManager* gFbxSdkManager) {

	gFbxSdkManager->Destroy();
}

void SE::Core::FBXConverter::Deallocate() {

	for (UINT i = 0; i < meshes.size(); i++) {

		for (UINT j = 0; j < meshes[i].controlPoints.size(); j++) {

		delete meshes[i].controlPoints[j];

		}
	}

}

bool SE::Core::FBXConverter::Load(string fileName, string exportFolder) {

	// Check if the FBX file was loaded properly

	if (!LoadFBXFormat(fileName, exportFolder)) {

		cout << "\nFailed to load the FBX format. Verify that the path is correct.\nPress Enter to quit..." << endl;
		
		return false;
	}

	return true;
}

bool SE::Core::FBXConverter::LoadFBXFormat(string mainFileName, string exportFolder) {

	cout << "#----------------------------------------------------------------------------\n"
		"# STEP 1: LOADING THE MAIN FILE\n"
		"#\n"
		"# FILEPATH: " << mainFileName <<"\n"
		"#----------------------------------------------------------------------------\n" << endl;

	//------------------------------------------------------------------------------//
	// INITIALIZE FBX SDK MANAGER
	//------------------------------------------------------------------------------//

	fileName = removeExtension(getFilename(mainFileName));

	//Path to export to (This must be the asset folder)
	path pathName = exportFolder;

	// Create a folder for all the format files to easily manage them in other project folders
	// Filesystem can create the folder directory for us given a path based on the previously entered path name
	folderName = pathName.string();
	create_directory(folderName);

	logFolder = folderName + "/Log/";
	create_directory(logFolder);

	logFolder = folderName + "/Log/";
	create_directory(logFolder);

	// Initialize the FBX loader and instruct it what types of data to load...
	if (!InitializeFbxManager()) {

		return false;
	}

	// Create both the FbxImporter and the FbxScene from the FbxSdkManager
	if (!InitializeSceneImporter()) {

		return false;
	}

	//------------------------------------------------------------------------------//
	// LOAD MAIN FILE
	//------------------------------------------------------------------------------//

	// Load the specified file. If it can't be found, destroy the FbxSdkManager and return false
	if (!LoadSceneFile(mainFileName, gFbxSdkManager, pImporter, pFbxScene)) {

		ReleaseAll(gFbxSdkManager);
		return false;
	}

	// Retrieve the root node from the currently opened file. It's the key to all the content in the FBX file.
	pFbxRootNode = pFbxScene->GetRootNode();

	if (!pFbxRootNode) {

		cout << "[ERROR] Could not retrieve root node from scene" << endl;
		return false;
	}

	cout << "\n#----------------------------------------------------------------------------\n"
		"# STEP 1: LOAD MESHES\n"
		"#----------------------------------------------------------------------------\n" << endl;

	// Load the meshes from the file
	GetMeshes();

	cout << "\n#----------------------------------------------------------------------------\n"
		"# STEP 2: LOAD LIGHTS\n"
		"#----------------------------------------------------------------------------\n" << endl;

	// Load the lights from the file
	GetLights();

	// When all content has been loaded, destroy the FbxSdkManager
	ReleaseAll(gFbxSdkManager);
	
	return true;
}

bool SE::Core::FBXConverter::LoadSceneFile(string fileName, FbxManager* gFbxSdkManager, FbxImporter* pImporter, FbxScene* pScene) {

	// Gather the new FBX file importer

	string logFileName = logFolder + "/Log_" + "Start_" + fileName + ".log";

	logFile.open(logFileName, ofstream::app);

	logFile << "File: " << fileName << endl;

	bool bSuccess = pImporter->Initialize(fileName.c_str(), -1, gFbxSdkManager->GetIOSettings());

	if (!bSuccess) {

		logFile << "File load status: The file was invalid or not found. Please verify the path and try again" << endl;

		logFile.close();

		cout << "[ERROR] Importer failed to read the file" << endl;
		return false;
	}

	logFile << "File load status: The file was successfully loaded" << endl;

	cout << "[OK] Importer could successfully read the file" << endl;

	// Use the new importer to import its content into the scene

	bSuccess = pImporter->Import(pScene);

	if (!bSuccess) {

		logFile << "File import status: The file could not be imported. Please verify the FBX SDK version" << endl;

		logFile.close();

		cout << "[ERROR] File couldn't be loaded into scene" << endl;
		return false;
	}

	logFile << "File import status: The file was successfully imported" << endl;

	logFile.close();

	cout << "[OK] File was successfully loaded into scene " << "\n\n";

	return true;
}

bool SE::Core::FBXConverter::InitializeFbxManager() {

	string logFileName = logFolder + "/Log_" + "Start_" + fileName + ".log";

	logFile.open(logFileName, ofstream::out);

	if (gFbxSdkManager == nullptr) {

		// We create our Fbx manager and store it in the Fbx manager pointer variable...
		SYSTEMTIME time;
		GetLocalTime(&time);
		gFbxSdkManager = FbxManager::Create();
		logFile << "Date: " << time.wYear << "-" << time.wMonth << "-" << time.wDay << " " << time.wHour << ":" << time.wMinute << endl;
		logFile << "User: " << getenv("USERNAME") << endl;
		logFile << "FBX SDK Version: " << gFbxSdkManager->GetVersion(true) << endl;

		// ...which is the FbxIoSettings, and we can use it to load only meshes and their belonging materials

		pIOsettings = FbxIOSettings::Create(gFbxSdkManager, IOSROOT);

		// We set our Fbx Manager IOsettings with the previously recieved settings specified in the variable above

		gFbxSdkManager->SetIOSettings(pIOsettings);

		cout << "[OK] FbxManager was successfully created" << endl;

		logFile.close();

		return true;
	}

	else {

		logFile.close();

		cout << "[ERROR] An instance of the FbxManager already exists!" << endl;

		return false;
	}
}

bool SE::Core::FBXConverter::InitializeSceneImporter() {

	pImporter = FbxImporter::Create(gFbxSdkManager, "");

	if (!pImporter) {

		cout << "[ERROR] Importer failed to initialize" << endl;
		return false;
	}

	else {

		cout << "[OK] Importer was successfully initialized" << endl;
	}

	pFbxScene = FbxScene::Create(gFbxSdkManager, "");

	if (!pFbxScene) {

		cout << "[ERROR] Scene failed to initialize" << endl;
		return false;
	}

	else {

		cout << "[OK] Scene was successfully initialized" << endl;
	}

	return true;
}


void SE::Core::FBXConverter::GetMeshes() {

	for (unsigned int i = 0; i < pFbxRootNode->GetChildCount(); i++) {	// Get number of children nodes from the root node

		Mesh currentMesh;

		FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);	// Current child node being processed in the file

		if (pFbxChildNode->GetNodeAttribute() == NULL) { // Is it a null pointer? If so, continue searching the FBX file.

			continue;
		}
		
		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();	// Get the attribute type of the child node

		if (AttributeType != FbxNodeAttribute::eMesh) { // Does it qualify as a mesh? If not, continue searching the FBX file.

			continue;
		}
		

		// Get the current mesh node and store it in our own datatype
		currentMesh.meshNode = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

		// From the mesh node, gather the vertices. We're required to have the vertices to also include potential weights on the in the next function
		ProcessControlPoints(currentMesh);

		// Check for current mesh skin node and gather clusters if there are any joints in the skeleton
		CheckSkinNode(currentMesh);
		
		// Get name of the current mesh
		currentMesh.name = currentMesh.meshNode->GetNode()->GetName();

		// The the position of the current mesh
		currentMesh.transformAttributes.position.x = (float)currentMesh.meshNode->GetNode()->LclTranslation.Get().mData[0];
		currentMesh.transformAttributes.position.y = (float)currentMesh.meshNode->GetNode()->LclTranslation.Get().mData[1];
		currentMesh.transformAttributes.position.z = (float)currentMesh.meshNode->GetNode()->LclTranslation.Get().mData[2];

		// The the rotation of the current mesh
		currentMesh.transformAttributes.rotation.x = (float)currentMesh.meshNode->GetNode()->LclRotation.Get().mData[0];
		currentMesh.transformAttributes.rotation.y = (float)currentMesh.meshNode->GetNode()->LclRotation.Get().mData[1];
		currentMesh.transformAttributes.rotation.z = (float)currentMesh.meshNode->GetNode()->LclRotation.Get().mData[2];
		
		// The scale of the current mech
		currentMesh.transformAttributes.scale.x = (float)currentMesh.meshNode->GetNode()->LclScaling.Get().mData[0]; 
		currentMesh.transformAttributes.scale.y = (float)currentMesh.meshNode->GetNode()->LclScaling.Get().mData[1];
		currentMesh.transformAttributes.scale.z = (float)currentMesh.meshNode->GetNode()->LclScaling.Get().mData[2];

		// Load the materials of the current mesh
		LoadMaterial(currentMesh);

		// Compute bounding box values
		currentMesh.meshNode->ComputeBBox();

		// bbox values
		FbxDouble3 bboxMax = currentMesh.meshNode->BBoxMax; 
		FbxDouble3 bboxMin = currentMesh.meshNode->BBoxMin;
		
		double test = (float)bboxMax[0];

		currentMesh.bboxValues.max.x = (float)bboxMax[0];
		currentMesh.bboxValues.max.y = (float)bboxMax[1];
		currentMesh.bboxValues.max.z = (float)bboxMax[2];

		currentMesh.bboxValues.min.x = (float)bboxMin[0];
		currentMesh.bboxValues.min.y = (float)bboxMin[1];
		currentMesh.bboxValues.min.z = (float)bboxMin[2];
		
		meshes.push_back(currentMesh);
		
	}

	cout << "[OK] Found " << meshes.size() << " mesh(es) in the format\n\n";

	string logFileName = logFolder + "/Log_" + "Meshes_" + fileName + ".log";
	logFile.open(logFileName, ofstream::out);

	for (unsigned int i = 0; i < meshes.size(); i++) {

		if (meshes[i].vertexLayout == 1) {

			CreateVertexDataBone(meshes[i], pFbxRootNode);
		}

		else {

			CreateVertexDataStandard(meshes[i], pFbxRootNode);
		}
		
		logFile << "-------------------------------------------------------\n"
			<< "Mesh " << i + 1 <<
			"\n-------------------------------------------------------\n";

		// Print the mesh data to the console

		logFile << "Name: " << meshes[i].name.c_str() << "\nPosition: {"
			<< meshes[i].transformAttributes.position.x << ", "
			<< meshes[i].transformAttributes.position.y << ", "
			<< meshes[i].transformAttributes.position.z << "}\nRotation: {"
			<< meshes[i].transformAttributes.rotation.x << ", "
			<< meshes[i].transformAttributes.rotation.y << ", "
			<< meshes[i].transformAttributes.rotation.z << "}\nScale: {"
			<< meshes[i].transformAttributes.scale.x << ", "
			<< meshes[i].transformAttributes.scale.y << ", "
			<< meshes[i].transformAttributes.scale.z << "}\nBounding Box: \n"
			<< "xMax: " << meshes[i].bboxValues.max.x << " yMax : " << meshes[i].bboxValues.max.y << " zMax: " << meshes[i].bboxValues.max.z << "\n"
			<< "xMin: " << meshes[i].bboxValues.min.x << " yMin : " << meshes[i].bboxValues.min.y << " zMin: " << meshes[i].bboxValues.min.z << "\nVertices: "
			<< meshes[i].controlPoints.size() << "\n\nMaterial: "
			<< meshes[i].objectMaterial.materialName.c_str() << "\nType: "

			// Print the material attributes to the console

			<< meshes[i].objectMaterial.materialType.c_str() << "\n\nDiffuse: "
			<< meshes[i].objectMaterial.diffuseColor.x << ", "
			<< meshes[i].objectMaterial.diffuseColor.y << ", "
			<< meshes[i].objectMaterial.diffuseColor.z << "\nDiffuse Factor: "
			<< meshes[i].objectMaterial.diffuseFactor << "\n\nAmbient: "
			<< meshes[i].objectMaterial.ambientColor.x << ", "
			<< meshes[i].objectMaterial.ambientColor.y << ", "
			<< meshes[i].objectMaterial.ambientColor.z << "\nAmbient Factor: "
			<< meshes[i].objectMaterial.ambientFactor << "\n\nSpecular: "
			<< meshes[i].objectMaterial.specularColor.x << ", "
			<< meshes[i].objectMaterial.specularColor.y << ", "
			<< meshes[i].objectMaterial.specularColor.z << "\nSpecular Factor: "
			<< meshes[i].objectMaterial.specularFactor << "\n\n";

			// Print the texture information to the console

			int textureCount = meshes[i].objectMaterial.textures.size();

			for (int index = 0; index < textureCount; index++){

				
				logFile << "\nTexture Name : " << meshes[i].objectMaterial.textures[index].textureName.c_str()
						<< "\nTexture Path: " << meshes[i].objectMaterial.textures[index].texturePath.c_str() << "\n\n";

			}
		
	}

	logFile.close();
}

void SE::Core::FBXConverter::GetLights() {

	for (int i = 0; i < pFbxRootNode->GetChildCount(); i++) {	// Get number of children nodes from the root node

		Light currentLight;

		FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);	// Current child being processed in the file

		if (pFbxChildNode->GetNodeAttribute() == NULL) {

			continue;
		}

		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();	// Get the attribute type of the child node

		if (AttributeType != FbxNodeAttribute::eLight) {

			continue;
		}

		currentLight.lightNode = (FbxLight*)pFbxChildNode->GetNodeAttribute();

		currentLight.name = currentLight.lightNode->GetNode()->GetName();

		currentLight.color.x = (float)currentLight.lightNode->Color.Get().mData[0];
		currentLight.color.y = (float)currentLight.lightNode->Color.Get().mData[1];
		currentLight.color.z = (float)currentLight.lightNode->Color.Get().mData[2];

		currentLight.position.x = (float)currentLight.lightNode->GetNode()->LclTranslation.Get().mData[0];
		currentLight.position.y = (float)currentLight.lightNode->GetNode()->LclTranslation.Get().mData[1];
		currentLight.position.z = (float)currentLight.lightNode->GetNode()->LclTranslation.Get().mData[2];

		currentLight.direction.x = (float)currentLight.lightNode->GetNode()->LclRotation.Get().mData[0];
		currentLight.direction.y = (float)currentLight.lightNode->GetNode()->LclRotation.Get().mData[1];
		currentLight.direction.z = (float)currentLight.lightNode->GetNode()->LclRotation.Get().mData[2];

		lights.push_back(currentLight);
	}

	string logFileName = logFolder + "/Log_" + "Lights_" + fileName + ".log";
	logFile.open(logFileName, ofstream::out);

	if (lights.size() > 0) {

		cout << "[OK] Found " << lights.size() << " lights in the format\n\n";

		for (int i = 0; i < lights.size(); i++) {

			if (lights[i].lightNode->LightType.Get() == FbxLight::ePoint) {

				logFile << "-----------------------------------------\n"
					<< "Light " << i + 1 <<
					"\n-----------------------------------------\n"
					<< "Name; " << lights[i].name << "\n"
					"Type: Point\n"
					"Color: {"
					<< lights[i].color.x << ", "
					<< lights[i].color.y << ", "
					<< lights[i].color.z << "}\n"
					"Position: {"
					<< lights[i].position.x << ", "
					<< lights[i].position.y << ", "
					<< lights[i].position.z << "}\n\n";
			}

			if (lights[i].lightNode->LightType.Get() == FbxLight::eSpot) {

				logFile << "-----------------------------------------\n"
					<< "Light " << i + 1 <<
					"\n-----------------------------------------\n"
					<< "Name; " << lights[i].name << "\n"
					"Type: Spot\n"
					"Color: {"
					<< lights[i].color.x << ", "
					<< lights[i].color.y << ", "
					<< lights[i].color.z << "}\n"
					"Position: {"
					<< lights[i].position.x << ", "
					<< lights[i].position.y << ", "
					<< lights[i].position.z << "}\n\n";
			}

			if (lights[i].lightNode->LightType.Get() == FbxLight::eArea) {

				logFile << "-----------------------------------------\n"
					<< "Light " << i + 1 <<
					"\n-----------------------------------------\n"
					<< "Name; " << lights[i].name << "\n"
					"Type: Area\n"
					"Color: {"
					<< lights[i].color.x << ", "
					<< lights[i].color.y << ", "
					<< lights[i].color.z << "}\n"
					"Position: {"
					<< lights[i].position.x << ", "
					<< lights[i].position.y << ", "
					<< lights[i].position.z << "}\n\n";
			}

			if (lights[i].lightNode->LightType.Get() == FbxLight::eDirectional) {

				logFile << "-----------------------------------------\n"
					<< "Light " << i + 1 <<
					"\n-----------------------------------------\n"
					<< "Name; " << lights[i].name << "\n"
					"Type: Directional\n"
					"Color: {"
					<< lights[i].color.x << ", "
					<< lights[i].color.y << ", "
					<< lights[i].color.z << "}\n"
					"Position: {"
					<< lights[i].position.x << ", "
					<< lights[i].position.y << ", "
					<< lights[i].position.z << "}\n"
					"Direction: {"
					<< lights[i].direction.x << ", "
					<< lights[i].direction.y << ", "
					<< lights[i].direction.z << "}\n\n";
			}

			if (lights[i].lightNode->LightType.Get() == FbxLight::eVolume) {

				logFile << "-----------------------------------------\n"
					<< "Light " << i + 1 <<
					"\n-----------------------------------------\n"
					<< "Name; " << lights[i].name << "\n"
					"Type: Volume\n"
					"Color: {"
					<< lights[i].color.x << ", "
					<< lights[i].color.y << ", "
					<< lights[i].color.z << "}\n"
					"Position: {"
					<< lights[i].position.x << ", "
					<< lights[i].position.y << ", "
					<< lights[i].position.z << "}\n\n";
			}
		}

		logFile.close();

	}

	else {

		logFile.close();

		cout << "[NO CONTENT FOUND] No lights were found in the scene";
	}

}


void SE::Core::FBXConverter::ProcessControlPoints(Mesh &pMesh) {

	unsigned int controlPointCount = pMesh.meshNode->GetControlPointsCount();	// Store the total amount of control points

	// Loop through all vertices and create individual vertices that are store in the current mesh's control points vector

	for (unsigned int i = 0; i < controlPointCount; i++) {

		ControlPoint* currentControlPoint = new ControlPoint();

		XMFLOAT3 position;
		position.x = static_cast<float>(pMesh.meshNode->GetControlPointAt(i).mData[0]);

		position.y = static_cast<float>(pMesh.meshNode->GetControlPointAt(i).mData[1]);

		position.z = static_cast<float>(pMesh.meshNode->GetControlPointAt(i).mData[2]);

		currentControlPoint->Position = position;
		pMesh.controlPoints[i] = currentControlPoint;

	}
}

void SE::Core::FBXConverter::CheckSkinNode(Mesh &pMesh) {

	unsigned int deformerCount = pMesh.meshNode->GetDeformerCount();

	for (unsigned int deformerIndex = 0; deformerIndex < deformerCount; deformerIndex++) {

		pMesh.skinNode = reinterpret_cast<FbxSkin*>(pMesh.meshNode->GetDeformer(deformerIndex, FbxDeformer::eSkin));

		if (!pMesh.skinNode) {

			continue;
		}

	}

	// If there was a valid skin node, we want to gather its clusters and the vertex weigths
	if (pMesh.skinNode) {

		pMesh.vertexLayout = 1;

		//Load the skeleton hierarchy. If no joint hierarchy is found, the skeleton will be empty
		GetSkeletonHierarchy(pMesh);

		// Create the bind pose for each joint in the hierarchy
		CreateBindPose(pMesh);

		// Gather the weights for the mesh
		GatherWeights(pMesh);

		// Gather animation data here
		GatherAnimationData(pMesh);
	}

	else {

		pMesh.vertexLayout = 0;
	}

}

void SE::Core::FBXConverter::CreateVertexDataStandard(Mesh &pMesh, FbxNode* pFbxRootNode) {

	if (pFbxRootNode) {

		int index = 0;
		int vertexCounter = 0;
		int i = 0;

		FbxVector4* pVertices = pMesh.meshNode->GetControlPoints();

		int k = pMesh.meshNode->GetPolygonCount();
		for (int j = 0; j < pMesh.meshNode->GetPolygonCount(); j++) {

			// Retreive the size of every polygon which should be represented as a triangle
			int iNumVertices = pMesh.meshNode->GetPolygonSize(j);

			// Reassure that every polygon is a triangle and if not, don't allow the user to pass this point
			assert(iNumVertices == 3);

			// Process every vertex in the triangle
			for (int k = 0; k < iNumVertices; k++) {

				// Retrieve the vertex index to know which control point in the vector to use
				int iControlPointIndex = pMesh.meshNode->GetPolygonVertex(j, k);
				ControlPoint* currentControlPoint = pMesh.controlPoints[iControlPointIndex];

				// Initialize the vertex position from the corresponding control point in the vector
				Vertex vertex;
				vertex.pos = currentControlPoint->Position;

				// Initialize texture coordinates to store in the output vertex
				FbxVector2 FBXTexcoord;
				bool unmapped;

				// Get the names of the UV-Sets attached to this mesh
				/*FbxStringList uvSetList;
				pMesh.meshNode->GetUVSetNames(uvSetList);*/

				iControlPointIndex = pMesh.meshNode->GetPolygonVertexUV(j, k, "map1", FBXTexcoord, unmapped);

				vertex.uv.x = (float)FBXTexcoord.mData[0];
				vertex.uv.y = (float)FBXTexcoord.mData[1];
				vertex.uv.y = 1 - vertex.uv.y;

				// Initialize normals to store in the output vertex
				FbxVector4 FBXNormal;

				iControlPointIndex = pMesh.meshNode->GetPolygonVertexNormal(j, k, FBXNormal);

				vertex.normal.x = (float)FBXNormal.mData[0];
				vertex.normal.y = (float)FBXNormal.mData[1];
				vertex.normal.z = (float)FBXNormal.mData[2];

				CreateNormals(pMesh, iControlPointIndex, vertex.binormal, vertex.tangent, j, k);

				// Push back vertices to the current mesh
				pMesh.standardVertices.push_back(vertex);

				vertexCounter++;

			}

		}

	}

}

void SE::Core::FBXConverter::CreateVertexDataBone(Mesh &pMesh, FbxNode* pFbxRootNode) {

	if (pFbxRootNode) {

		int vertexCounter = 0;
		int i = 0;

		FbxVector4* pVertices = pMesh.meshNode->GetControlPoints();

		for (int j = 0; j < pMesh.meshNode->GetPolygonCount(); j++) {

			int iNumVertices = pMesh.meshNode->GetPolygonSize(j);	// Retreive the size of every polygon which should be represented as a triangle
			assert(iNumVertices == 3);	// Reassure that every polygon is a triangle and if not, don't allow the user to pass this point

			for (int k = 0; k < iNumVertices; k++) {	// Process every vertex in the triangle

				int iControlPointIndex = pMesh.meshNode->GetPolygonVertex(j, k);	// Retrieve the vertex index to know which control point in the vector to use
				ControlPoint* currentControlPoint = pMesh.controlPoints[iControlPointIndex];

				VertexDeformer vertex;
				vertex.pos = currentControlPoint->Position;	// Initialize the vertex position from the corresponding control point in the vector

				FbxVector2 FBXTexcoord;
				bool unmapped;

				// Get the names of the UV-Sets attached to this mesh
				/*FbxStringList uvSetList;
				pMesh.meshNode->GetUVSetNames(uvSetList);*/

				iControlPointIndex = pMesh.meshNode->GetPolygonVertexUV(j, k, "map1", FBXTexcoord, unmapped);	// Initialize texture coordinates to store in the output vertex

				vertex.uv.x = (float)FBXTexcoord.mData[0];
				vertex.uv.y = (float)FBXTexcoord.mData[1];
				vertex.uv.y = 1 - vertex.uv.y;

				FbxVector4 FBXNormal;

				iControlPointIndex = pMesh.meshNode->GetPolygonVertexNormal(j, k, FBXNormal); // Initialize normals to store in the output vertex

				vertex.normal.x = (float)FBXNormal.mData[0];
				vertex.normal.y = (float)FBXNormal.mData[1];
				vertex.normal.z = (float)FBXNormal.mData[2];

				// Retreive Blending Weight info for each vertex in the mesh
				// Every vertex must have three weights and four influencing bone indices

				vertex.weights[0] = 0.0f;
				vertex.weights[1] = 0.0f;
				vertex.weights[2] = 0.0f;
				vertex.weights[3] = 0.0f;

				vertex.boneIndices[0] = 0;
				vertex.boneIndices[1] = 0;
				vertex.boneIndices[2] = 0;
				vertex.boneIndices[3] = 0;

				for (unsigned int i = 0; i < currentControlPoint->BlendingInfo.size(); i++) {

					// Store weight pairs in a separate blending weight vector
					vertex.boneIndices[i] = currentControlPoint->BlendingInfo[i].BlendIndex;
					vertex.weights[i] = currentControlPoint->BlendingInfo[i].BlendWeight;

				}

				CreateNormals(pMesh, iControlPointIndex, vertex.binormal, vertex.tangent, j, k);

				pMesh.boneVertices.push_back(vertex);	// Store all vertices in a separate vector

				vertexCounter++;


			}

		}

	}

}

void FBXConverter::CreateNormals(Mesh &pMesh, int iControlPointIndex, XMFLOAT3 binormal, XMFLOAT3 tangent, int j, int k) {

	int index = 0;

	if (pMesh.meshNode->GetElementBinormalCount() < 1)
	{
		cout << ("Invalid Binormal Number") << endl;
		
	}

	//////////////////////////////////////////////////////////////
	//                     GET BINORMALS
	//////////////////////////////////////////////////////////////

	for (UINT i = 0; i < pMesh.meshNode->GetElementBinormalCount(); i++)
	{
		FbxGeometryElementBinormal* binormals = pMesh.meshNode->GetElementBinormal(i);
		iControlPointIndex = pMesh.meshNode->GetPolygonVertex(j, k);

		if (binormals->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			switch (binormals->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				binormal.x = (float)binormals->GetDirectArray().GetAt(iControlPointIndex).mData[0];
				binormal.y = (float)binormals->GetDirectArray().GetAt(iControlPointIndex).mData[1];
				binormal.z = (float)binormals->GetDirectArray().GetAt(iControlPointIndex).mData[2];

				//cout << vertex.BiNormal.x << " " << vertex.BiNormal.y << " " << vertex.BiNormal.z << " " << endl;
				break;
			}
			case  FbxGeometryElement::eIndexToDirect:
			{
				index = binormals->GetIndexArray().GetAt(iControlPointIndex);

				binormal.x = (float)binormals->GetDirectArray().GetAt(index).mData[0];
				binormal.y = (float)binormals->GetDirectArray().GetAt(index).mData[1];
				binormal.z = (float)binormals->GetDirectArray().GetAt(index).mData[2];
				break;
			}
			default:
				cout << "Error: Invalid binormal reference mode\n";
				break;
			}
		}
	}

	index = 0;
	//////////////////////////////////////////////////////////////
	//                     GET TANGENTS
	//////////////////////////////////////////////////////////////

	for (UINT i = 0; i < pMesh.meshNode->GetElementTangentCount(); i++)
	{
		FbxGeometryElementTangent* tangents = pMesh.meshNode->GetElementTangent(i);

		if (tangents->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{

			switch (tangents->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				tangent.x = (float)tangents->GetDirectArray().GetAt(iControlPointIndex).mData[0];
				tangent.y = (float)tangents->GetDirectArray().GetAt(iControlPointIndex).mData[1];
				tangent.z = (float)tangents->GetDirectArray().GetAt(iControlPointIndex).mData[2];
				break;

			}
			case  FbxGeometryElement::eIndexToDirect:
			{
				index = tangents->GetIndexArray().GetAt(iControlPointIndex);

				tangent.x = (float)tangents->GetDirectArray().GetAt(index).mData[0];
				tangent.y = (float)tangents->GetDirectArray().GetAt(index).mData[1];
				tangent.z = (float)tangents->GetDirectArray().GetAt(index).mData[2];
				break;
			}
			default:
				cout << "Error: Invalid Tangent reference mode\n";
				break;
			}
		}
	}
}


void SE::Core::FBXConverter::GetSkeletonHierarchy(Mesh &pMesh) {

	string logFileName = logFolder + "/Log_" + "Hierarchy_" + fileName + ".log";

	logFile.open(logFileName, ofstream::out);
	logFile << "DEPTH SEARCH ALGORITHM" << endl;
	logFile << "NAME > INDEX > PARENTINDEX" << endl;
	logFile.close();

	for (int subNodeIndex = 0; subNodeIndex < pFbxRootNode->GetChildCount(); subNodeIndex++) // loops trough all joints in node
	{

		FbxNode* currentChildNode = pFbxRootNode->GetChild(subNodeIndex);	// Get current node in the file
		RecursiveDepthFirstSearch(currentChildNode, pMesh, 0, 0, -1);	// Skeleton root node should be labeled with an index of -1

	}

	int size = pMesh.skeleton.hierarchy.size();

	if (size > 0) {

		cout << "[OK] Found a joint hierarchy with a total of " << size << " joints" << endl;
	}

	else {

		cout << "[WARNING] No active joint hierarchy found";
	}
}

void SE::Core::FBXConverter::RecursiveDepthFirstSearch(FbxNode* node, Mesh &pMesh, int depth, int index, int parentIndex) {

	// Recurvise depth first search function will first control that the actual node is a valid skeleton node by checking if
	// the node isn't a null pointer, which would result in the node attribute and its type being null and result in a thrown exception

	if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {

		string logFileName = logFolder + "/Log_" + "Hierarchy_" + fileName + ".log";
		logFile.open(logFileName, ofstream::app);

		// A "joint" object is created for every valid skeleton node in which its parent index and name is stored
		// The current joint name and the parent joint index is pushed back to the current mesh's skeleton

		Joint currentJoint;
		currentJoint.Name = node->GetName();
		currentJoint.ParentIndex = parentIndex;
		logFile << currentJoint.Name.c_str() << "   " << index << "   " << currentJoint.ParentIndex << endl;;
		pMesh.skeleton.hierarchy.push_back(currentJoint);

		logFile.close();

	}

	// Function is called again to traverse the hierarchy, if there is any, underneath this node

	for (int i = 0; i < node->GetChildCount(); i++) {

		RecursiveDepthFirstSearch(node->GetChild(i), pMesh, depth + 1, pMesh.skeleton.hierarchy.size(), index);
	}
}

void SE::Core::FBXConverter::CreateBindPose(Mesh &pMesh) {

	string logFileName = logFolder + "/Log_" + "Bindposes_" + fileName + ".log";

	logFile.open(logFileName, ofstream::out);

	unsigned int deformerCount = pMesh.meshNode->GetDeformerCount();	// A deformer is associated with manipulating geometry through clusters, which are the joints we're after

	//FbxAMatrix geometryTransform = GetGeometryTransformation(node); // Geometric offset must be taken into account, even though it's often an identity matrix

	// Get the number of joints in the hierarchy
	int NUM_BONES = pMesh.skinNode->GetClusterCount();

	logFile << "Number of joints: " << NUM_BONES << endl;

	// Get the root joint node which is the first cluster in the skin. 
	FbxCluster* currentCluster = pMesh.skinNode->GetCluster(0);

	currentCluster->GetName();

	logFile << "-------------------------------------------------------\n"
		<< "Joint: " << currentCluster->GetLink()->GetName() << "\nIndex: " << 0 <<
		"\n-------------------------------------------------------\n";

	// Initialize the root joint first. By doing so, we make sure no children is evaluated before its parent
	pMesh.skeleton.hierarchy[0].LocalTransform = currentCluster->GetLink()->EvaluateLocalTransform(FBXSDK_TIME_INFINITE);
	pMesh.skeleton.hierarchy[0].GlobalTransform = pMesh.skeleton.hierarchy[0].LocalTransform;
	pMesh.skeleton.hierarchy[0].GlobalBindposeInverse = pMesh.skeleton.hierarchy[0].GlobalTransform.Inverse();

	Print4x4Matrix(pMesh.skeleton.hierarchy[0].GlobalBindposeInverse);

	// Loop through all the joints in the hierarchy
	for (int i = 1; i < NUM_BONES; i++) {

		// Receive the current joint cluster
		currentCluster = pMesh.skinNode->GetCluster(i);

		logFile << "-------------------------------------------------------\n"
			<< "Joint: " << currentCluster->GetLink()->GetName() << "\nIndex:" << i <<
			"\n-------------------------------------------------------\n";

		// Create a reference to the currenct joint in the hierarchy to be processed
		Joint &b = pMesh.skeleton.hierarchy[i];

		// Get the current joint LOCAL transformation
		b.LocalTransform = currentCluster->GetLink()->EvaluateLocalTransform(FBXSDK_TIME_INFINITE);
		
		// Calculate the current joint GLOBAL transformation by taking the global transformation of the parent multiplied by this joint LOCAL transformation
		b.GlobalTransform = pMesh.skeleton.hierarchy[b.ParentIndex].GlobalTransform * b.LocalTransform;

		// The inverse bind pose is calculated by taking the inverse of the joint GLOBAL transformation matrix
		b.GlobalBindposeInverse = b.GlobalTransform.Inverse();

		Print4x4Matrix(b.GlobalBindposeInverse);

	}

	logFile.close();

}

void SE::Core::FBXConverter::GatherWeights(Mesh &pMesh) {

	unsigned int clusterCount = pMesh.skinNode->GetClusterCount();	// Every joint is technically a deformer, so we must process through each one in the hierarchy

	for (unsigned int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {

		FbxCluster* currentCluster = pMesh.skinNode->GetCluster(clusterIndex); // Current joint being processed in the hierarchy
		std::string currentJointName = currentCluster->GetLink()->GetName();	// Here is the direct link to the joint required to retrieve its name and other attributes
		unsigned int currentJointIndex = FindJointIndexByName(currentJointName, pMesh.skeleton);	// Call to function to retrieve joint index from skeleton hierarchy

		// Associate the joint with the control points it affects
		unsigned int indicesCount = currentCluster->GetControlPointIndicesCount();

			for (unsigned int i = 0; i < indicesCount; i++)
			{

				BlendingIndexWeightPair currentBlendPair;
				currentBlendPair.BlendIndex = currentJointIndex;
				currentBlendPair.BlendWeight = currentCluster->GetControlPointWeights()[i];
				pMesh.controlPoints[currentCluster->GetControlPointIndices()[i]]->BlendingInfo.push_back(currentBlendPair);

			}

	}
}


void SE::Core::FBXConverter::GatherAnimationData(Mesh &pMesh) {

	string logFileName = logFolder + "/Log_" + "Animations_" + fileName + ".log";

	logFile.open(logFileName, ofstream::out);

	unsigned int clusterCount = pMesh.skinNode->GetClusterCount();	// Every joint is technically a deformer, so we must process through each one in the hierarchy

	for (unsigned int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) // loops trough every joint
	{

		FbxCluster* currentCluster = pMesh.skinNode->GetCluster(clusterIndex); // Current joint being processed in the hierarchy
		string currentJointName = currentCluster->GetLink()->GetName();	// Here is the direct link to the joint required to retrieve its name and other attributes
		unsigned int currentJointIndex = FindJointIndexByName(currentJointName, pMesh.skeleton);	// Call to function to retrieve joint index from skeleton hierarchy

		// Get the number of animation stacks (Should only be one in our case)
		for (size_t i = 0; i < pFbxScene->GetSrcObjectCount<FbxAnimStack>(); i++) // for every stack
		{
			// Get the current animation stack
			int stackCount = pFbxScene->GetSrcObjectCount<FbxAnimStack>();
			FbxAnimStack* AnimStack = pFbxScene->GetSrcObject<FbxAnimStack>(i);
			FbxString animStackName = AnimStack->GetName();

			int numLayers = AnimStack->GetMemberCount<FbxAnimLayer>();

			// For every layer / every animation
			for (size_t j = 0; j < numLayers; j++) 
			{

				FbxAnimLayer* currentAnimLayer;
				Animation CurrentAnimation;

				currentAnimLayer = AnimStack->GetMember<FbxAnimLayer>(j);
				CurrentAnimation.Name = currentAnimLayer->GetName();

				// From the current joint, get animation curves

				// Translation curves
				const FbxAnimCurve* translationCurveX = currentCluster->GetLink()->LclTranslation.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
				const FbxAnimCurve* translationCurveY = currentCluster->GetLink()->LclTranslation.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				const FbxAnimCurve* translationCurveZ = currentCluster->GetLink()->LclTranslation.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

				// Rotation curves
				const FbxAnimCurve* rotationCurveX = currentCluster->GetLink()->LclRotation.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
				const FbxAnimCurve* rotationCurveY = currentCluster->GetLink()->LclRotation.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				const FbxAnimCurve* rotationCurveZ = currentCluster->GetLink()->LclRotation.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

				// Scaling curves (Should always be 1)
				const FbxAnimCurve* scalingCurveX = currentCluster->GetLink()->LclScaling.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
				const FbxAnimCurve* scalingCurveY = currentCluster->GetLink()->LclScaling.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				const FbxAnimCurve* scalingCurveZ = currentCluster->GetLink()->LclScaling.GetCurve(currentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

				// Find out how many keyframes there are on the curve (Must subtract with 1 to not go out of range)
				const int numKeys = (translationCurveY) ? translationCurveY->KeyGetCount() - 1 : 0;
				CurrentAnimation.Keyframes.resize(numKeys);
				CurrentAnimation.Length = numKeys;
				logFile << "-------------------------------------------------------\n"
					<< "Joint: " << currentJointName << "\nNumber of animatons: " << numLayers - 1 << "\nAnimation: " << CurrentAnimation.Name << "\nIndex: " << j << "\nLength: " << numKeys <<
					"\n-------------------------------------------------------\n";

				// Access the current value on each individual channel on the different curves at a given keyframe
				for (UINT timeIndex = 0; timeIndex < numKeys; timeIndex++) {

					logFile << "Time: " << timeIndex + 1 << endl;

					// Receive the value on the selected channel on the current keyframe
					float translationX = static_cast<float>(translationCurveX->KeyGetValue(timeIndex));
					float translationY = static_cast<float>(translationCurveY->KeyGetValue(timeIndex));
					float translationZ = static_cast<float>(translationCurveZ->KeyGetValue(timeIndex));
					
					float rotationX = static_cast<float>(rotationCurveX->KeyGetValue(timeIndex));
					float rotationY = static_cast<float>(rotationCurveY->KeyGetValue(timeIndex));
					float rotationZ = static_cast<float>(rotationCurveZ->KeyGetValue(timeIndex));
					
					float scalingX = static_cast<float>(scalingCurveX->KeyGetValue(timeIndex));
					float scalingY = static_cast<float>(scalingCurveY->KeyGetValue(timeIndex));
					float scalingZ = static_cast<float>(scalingCurveZ->KeyGetValue(timeIndex));

					// Build the vectors for the global transform matrix
					FbxVector4 translationVector = { translationX, translationY, translationZ, 1.0f };
					FbxVector4 rotationVector = { rotationX, rotationY, rotationZ, 1.0f };
					FbxVector4 scalingVector = { scalingX, scalingY, scalingZ, 1.0f };

					FbxAMatrix globalTransform;

					// Set the vectors for the global transform matrix (Build the keyframe)
					globalTransform.SetT(translationVector);
					globalTransform.SetR(rotationVector);
					globalTransform.SetS(scalingVector);

					CurrentAnimation.Keyframes[timeIndex].GlobalTransform = globalTransform;
					CurrentAnimation.Keyframes[timeIndex].TimePos = timeIndex;

					// Gather translation from matrix
					CurrentAnimation.Keyframes[timeIndex].Translation = XMFLOAT4(
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetT().mData[0],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetT().mData[1],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetT().mData[2],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetT().mData[3]);

					// Gather scale from matrix
					CurrentAnimation.Keyframes[timeIndex].Scale = XMFLOAT4(
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetS().mData[0],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetS().mData[1],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetS().mData[2],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetS().mData[3]);

					// Gather rotation from matrix
					CurrentAnimation.Keyframes[timeIndex].RotationQuat = XMFLOAT4(
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetQ().mData[0],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetQ().mData[1],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetQ().mData[2],
						CurrentAnimation.Keyframes[timeIndex].GlobalTransform.GetQ().mData[3]);

					Print4x4Matrix(CurrentAnimation.Keyframes[timeIndex].GlobalTransform);

				}

				// If the animation length wasn't greater than 0, don't push back the animation. Usually this is the base layer, or an empty animation layer
				if (CurrentAnimation.Length > 0){

					pMesh.skeleton.hierarchy[currentJointIndex].Animations.push_back(CurrentAnimation);

				}

				else {

					logFile << "This animation layer had no keyframes" << "\n\n";
				}
			}

		}

	}

	logFile.close();

}


void SE::Core::FBXConverter::LoadMaterial(Mesh& pMesh) {

	FbxSurfaceMaterial* surfaceMaterial;
	FbxNode* materialNode;
	
	materialNode = pMesh.meshNode->GetNode();
	int materialCount = materialNode->GetMaterialCount();

	for (int i = 0; i < materialCount; i++) {

		// Get the material node
		surfaceMaterial = materialNode->GetMaterial(i);

		// Set the material name for the current mesh's material
		pMesh.objectMaterial.materialName = surfaceMaterial->GetName();

		// Check if the material is of the type Lambert
		if (surfaceMaterial->GetClassId() == FbxSurfaceLambert::ClassId) {

			pMesh.objectMaterial.materialType = "Lambert";

			FbxSurfaceLambert* lambertMaterial = (FbxSurfaceLambert*)surfaceMaterial;
			FbxPropertyT<FbxDouble3> lambertDiffuse = lambertMaterial->Diffuse;
			FbxPropertyT<FbxDouble3> lambertAmbient = lambertMaterial->Ambient;
			
			FbxDouble3 lambertDiffuseInfo = lambertDiffuse.Get();
			FbxDouble3 lambertAmbientInfo = lambertAmbient.Get();

			pMesh.objectMaterial.diffuseColor.x = lambertDiffuseInfo.mData[0];
			pMesh.objectMaterial.diffuseColor.y = lambertDiffuseInfo.mData[1];
			pMesh.objectMaterial.diffuseColor.z = lambertDiffuseInfo.mData[2];

			pMesh.objectMaterial.diffuseFactor = lambertMaterial->DiffuseFactor;

			pMesh.objectMaterial.ambientColor.x = lambertAmbientInfo.mData[0];
			pMesh.objectMaterial.ambientColor.y = lambertAmbientInfo.mData[1];
			pMesh.objectMaterial.ambientColor.z = lambertAmbientInfo.mData[2];

			pMesh.objectMaterial.ambientFactor = lambertMaterial->AmbientFactor;


			// Lambert doesn't have any specularity to it, so this can be set to 0
			pMesh.objectMaterial.specularColor.x = 0.0f;
			pMesh.objectMaterial.specularColor.y = 0.0f;
			pMesh.objectMaterial.specularColor.z = 0.0f;

			pMesh.objectMaterial.specularFactor = 0.0f;
		}

		// Check if the material is of the type Phong
		else if (surfaceMaterial->GetClassId() == FbxSurfacePhong::ClassId) {

			pMesh.objectMaterial.materialType = "Phong";

			FbxSurfacePhong* phongMaterial = (FbxSurfacePhong*)surfaceMaterial;
			FbxPropertyT<FbxDouble3> phongDiffuse = phongMaterial->Diffuse;
			FbxPropertyT<FbxDouble3> phongAmbient = phongMaterial->Ambient;
			FbxPropertyT<FbxDouble3> phongSpecular = phongMaterial->Specular;

			FbxDouble3 phongDiffuseInfo = phongDiffuse.Get();
			FbxDouble3 phongAmbientInfo = phongAmbient.Get();
			FbxDouble3 phongSpecularInfo = phongSpecular.Get();

			pMesh.objectMaterial.diffuseColor.x = phongDiffuseInfo.mData[0];
			pMesh.objectMaterial.diffuseColor.y = phongDiffuseInfo.mData[1];
			pMesh.objectMaterial.diffuseColor.z = phongDiffuseInfo.mData[2];

			pMesh.objectMaterial.diffuseFactor = phongMaterial->DiffuseFactor;

			pMesh.objectMaterial.ambientColor.x = phongAmbientInfo.mData[0];
			pMesh.objectMaterial.ambientColor.y = phongAmbientInfo.mData[1];
			pMesh.objectMaterial.ambientColor.z = phongAmbientInfo.mData[2];

			pMesh.objectMaterial.ambientFactor = phongMaterial->AmbientFactor;

			pMesh.objectMaterial.specularColor.x = phongSpecularInfo.mData[0];
			pMesh.objectMaterial.specularColor.y = phongSpecularInfo.mData[1];
			pMesh.objectMaterial.specularColor.z = phongSpecularInfo.mData[2];

			pMesh.objectMaterial.specularFactor = phongMaterial->Shininess;

		}

		// Get the texture on the diffuse material property
		FbxProperty diffuseProperty = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		GetChannelTexture(pMesh, diffuseProperty);

		// Get the texture on the bump material property
		FbxProperty bumpProperty = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		GetChannelTexture(pMesh, bumpProperty);

	}
}

void SE::Core::FBXConverter::GetChannelTexture(Mesh& pMesh, FbxProperty materialProperty) {

	// Look if any textures are attached (currently only on the diffuse channel) 
	int textureCount = materialProperty.GetSrcObjectCount<FbxTexture>();

	if (textureCount > 0) {

		for (int j = 0; j < textureCount; j++) {

			Texture texture;

			FbxTexture* materialTexture = FbxCast<FbxTexture>(materialProperty.GetSrcObject<FbxTexture>(j));

			texture.textureName = materialTexture->GetName();

			FbxFileTexture* textureFile = (FbxFileTexture*)materialTexture;

			texture.texturePath = textureFile->GetFileName();

			pMesh.objectMaterial.textures.push_back(texture);
		}

	}

}

bool SE::Core::FBXConverter::ExportTexture(Texture &texture, string textureFolder) {

	// Components to build the texture path to gather the texture from
	string texturePath = texture.texturePath;
	string textureName = texture.textureName;
	string extension = "." + texturePath.substr(texturePath.find(".") + 1);

	// Filesystem can copy the texture file for us, given a texture path and settings ( currently set to overwrite )
	try {
		copy_file(texturePath, textureFolder + textureName + extension, std::experimental::filesystem::copy_options::overwrite_existing);

	} catch (filesystem_error& e) {
		std::cout << "Could not copy texture: " << e.what() << '\n';
		return false;
	}

	return true;
}

void SE::Core::FBXConverter::Write() {

	cout << "\n#----------------------------------------------------------------------------\n"
		"# STEP 3: WRITE DATA\n"
		"#----------------------------------------------------------------------------\n" << endl;

	// Path to where the texture should be exported
	string meshFolder = folderName + "/Meshes/";
	string materialFolder = folderName + "/Materials/";
	string textureFolder = folderName + "/Textures/";
	string skeletonFolder = folderName + "/Skeleton/";
	string animationFolder = folderName + "/Animations/";
	string lightFolder = folderName + "/Lights/";

	// Create the texture directory in the exported format folder
	create_directory(meshFolder);
	create_directory(materialFolder);
	create_directory(textureFolder);
	create_directory(skeletonFolder);
	create_directory(animationFolder);
	create_directory(lightFolder);

	for (int meshIndex = 0; meshIndex < meshes.size(); meshIndex++) {

		WriteMaterial(materialFolder, textureFolder, meshes[meshIndex].objectMaterial);

		WriteMesh(meshFolder, meshes[meshIndex]);

		WriteSkeleton(skeletonFolder, meshes[meshIndex].skeleton, meshes[meshIndex].name);

		WriteAnimation(animationFolder, meshes[meshIndex].skeleton);
	}

	WriteLights(lightFolder);

	cout << "\nFinished writing the FBX file. See log folder for details. \n\nPress enter to close the program..." << endl;
}

void SE::Core::FBXConverter::WriteMaterial(string folderName, string textureFolder, Material& meshMaterial) {

	// Define the file name
	string binaryFile = folderName + "/" + meshMaterial.materialName + "_" + fileName + ".mat";

	// Define the ofstream 
	ofstream outBinary(binaryFile, std::ios::binary);

	uint32_t nrOfTextures = meshMaterial.textures.size();

	// Write the mesh header
	outBinary.write(reinterpret_cast<char*>(&nrOfTextures), sizeof(uint32_t));

	// Define the material attributes
	MaterialAttributes material;

	material.ambientColor.x = meshMaterial.ambientColor.x;
	material.ambientColor.y = meshMaterial.ambientColor.y;
	material.ambientColor.z = meshMaterial.ambientColor.z;
	material.ambientFactor = meshMaterial.ambientFactor;

	material.diffuseColor.x = meshMaterial.diffuseColor.x;
	material.diffuseColor.y = meshMaterial.diffuseColor.y;
	material.diffuseColor.z = meshMaterial.diffuseColor.z;
	material.diffuseFactor = meshMaterial.diffuseFactor;

	material.specularColor.x = meshMaterial.specularColor.x;
	material.specularColor.y = meshMaterial.specularColor.y;
	material.specularColor.z = meshMaterial.specularColor.z;
	material.specularFactor = meshMaterial.specularFactor;

	// Write the material attributes
	outBinary.write(reinterpret_cast<char*>(&material), sizeof(MaterialAttributes));

	cout << "[OK] Exported " << meshMaterial.materialName << " to " << folderName << endl;

	for (int textureIndex = 0; textureIndex < meshMaterial.textures.size(); textureIndex++) {

		if(ExportTexture(meshMaterial.textures[textureIndex], textureFolder)) {

			uint32_t size = meshMaterial.textures[textureIndex].textureName.size();
			string textureName = meshMaterial.textures[textureIndex].textureName;

			outBinary.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
			outBinary.write(reinterpret_cast<char*>(&textureName), size);

			cout << "[OK] Exported " << meshMaterial.textures[textureIndex].textureName << " to " << textureFolder << endl;
		}

		else {

			string textureName = "NULL";
			uint32_t size = textureName.size();

			outBinary.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
			outBinary.write(reinterpret_cast<char*>(&textureName), size);

			cout << "[ERROR] Failed to Export " << meshMaterial.textures[textureIndex].textureName << " to " + textureFolder << endl;

		}
	}

	outBinary.close();
}

void SE::Core::FBXConverter::WriteMesh(string folderName, Mesh& mesh) {

	// Define the file name
	string binaryFile = folderName + "/" + mesh.name + "_" + fileName + ".mesh";

	// Define the ofstream 
	ofstream outBinary(binaryFile, std::ios::binary);

	// Define the mesh header
	uint32_t headerContent[2];
	headerContent[0] = mesh.vertexLayout;

	if (headerContent[0] == 1) {

		headerContent[1] = mesh.boneVertices.size();

		// Write the mesh header
		outBinary.write(reinterpret_cast<char*>(headerContent), sizeof(headerContent));

		// Write the mesh vertices to the binary file
		outBinary.write(reinterpret_cast<char*>(mesh.boneVertices.data()), sizeof(mesh.boneVertices[0]) * headerContent[1]);

		// Write the mesh transform attributes
		outBinary.write(reinterpret_cast<char*>(&mesh.transformAttributes), sizeof(TransformAttributes));

		// Write the mesh bounding box
		outBinary.write(reinterpret_cast<char*>(&mesh.bboxValues), sizeof(BBox));

		cout << "[OK] Exported " << mesh.name << " to " << folderName << endl;

	}

	else {

		headerContent[1] = mesh.standardVertices.size();

		// Write the mesh header
		outBinary.write(reinterpret_cast<char*>(headerContent), sizeof(headerContent));

		// Write the vertices to the binary file
		outBinary.write(reinterpret_cast<char*>(mesh.standardVertices.data()), sizeof(mesh.standardVertices[0]) * headerContent[1]);

		// Write the mesh transform attributes
		outBinary.write(reinterpret_cast<char*>(&mesh.transformAttributes), sizeof(TransformAttributes));

		// Write the mesh bounding box
		outBinary.write(reinterpret_cast<char*>(&mesh.bboxValues), sizeof(BBox));

		cout << "[OK] Exported " << mesh.name << " to " << folderName << endl;

	}

	outBinary.close();
}

void SE::Core::FBXConverter::WriteSkeleton(string folderName, Skeleton skeleton, string meshName) {

	if (skeleton.hierarchy.size() > 0){

		// Define the file name
		string binaryFile = folderName + "/" + meshName + "_" + fileName + ".skel";

		// Define the ofstream 
		ofstream outBinary(binaryFile, std::ios::binary);

		uint32_t nrOfJoints = skeleton.hierarchy.size();

		// Write the skeleton header
		outBinary.write(reinterpret_cast<char*>(&nrOfJoints), sizeof(uint32_t));

		for (int jointIndex = 0; jointIndex < skeleton.hierarchy.size(); jointIndex++) {

			// Get the bindpose and joint parent index
			uint32_t parentIndex = skeleton.hierarchy[jointIndex].ParentIndex;
			XMFLOAT4X4 bindPoseMatrix = Load4X4Transformations(skeleton.hierarchy[jointIndex].GlobalBindposeInverse);

			outBinary.write(reinterpret_cast<char*>(&parentIndex), sizeof(uint32_t));
			outBinary.write(reinterpret_cast<char*>(&bindPoseMatrix), sizeof(XMFLOAT4X4));

		}

		cout << "[OK] Exported " << meshName << " skeleton" << " to " << folderName << endl;

		outBinary.close();
	}

	else {

		cout << "[WARNINIG] No skeleton attached to " << meshName << endl;
	}
}

void SE::Core::FBXConverter::WriteAnimation(string folderName, Skeleton skeleton) {

	if (skeleton.hierarchy.size() > 0) {

		//Vector to hold the total amount of keyframes for all animations. Format supports up to five animations.
		uint32_t nrOfAnimations = skeleton.hierarchy[0].Animations.size();
		uint32_t nrOfJoints = skeleton.hierarchy.size();

		// Loop through each animation
		for (int currentAnimationIndex = 0; currentAnimationIndex < nrOfAnimations; currentAnimationIndex++)
		{
			vector<XMFLOAT4X4> animationTransformations;

			uint32_t currentAnimLength = skeleton.hierarchy[0].Animations[currentAnimationIndex].Keyframes.size();
			string animationName = skeleton.hierarchy[0].Animations[currentAnimationIndex].Name;

			// Loop through each joint in hierarchy ( Every joint has the same number of transformations as the length of the current animation )
			for (int currentJointIndex = 0; currentJointIndex < nrOfJoints; currentJointIndex++) {

				// Loop through each keyframe in the current joint being processed
				for (int currentKeyFrameIndex = 0; currentKeyFrameIndex < currentAnimLength; currentKeyFrameIndex++) {

					FbxAMatrix keyframe = skeleton.hierarchy[currentJointIndex].Animations[currentAnimationIndex].Keyframes[currentKeyFrameIndex].GlobalTransform;
					XMFLOAT4X4 jointGlobalTransform = Load4X4Transformations(keyframe);

					animationTransformations.push_back(jointGlobalTransform);

				}

			}

			// Define the file name
			string binaryFile = folderName + "/" + animationName + "_" + fileName + ".anim";

			// Define the ofstream 
			ofstream outBinary(binaryFile, std::ios::binary);

			// Write the current animation length, the number of joints and the total amount of keyframes for the animation
			outBinary.write(reinterpret_cast<char*>(&nrOfJoints), sizeof(uint32_t));
			outBinary.write(reinterpret_cast<char*>(&currentAnimLength), sizeof(uint32_t));
			outBinary.write(reinterpret_cast<char*>(animationTransformations.data()), sizeof(animationTransformations[0]) * animationTransformations.size());

			cout << "[OK] Exported " << animationName << " to " << folderName << endl;

			outBinary.close();
		}

	}

	else {

		cout << "[WARNING] No animations found" << endl;
	}

}

void SE::Core::FBXConverter::WriteLights(string folderName) {

	if(lights.size() > 0){

		// Define the file name
		string binaryFile = folderName + "/" + fileName + ".light";

		// Define the ofstream 
		ofstream outBinary(binaryFile, std::ios::binary);

		uint32_t nrOfLights = lights.size();

		// Write the mesh header
		outBinary.write(reinterpret_cast<char*>(&nrOfLights), sizeof(uint32_t));

		for (size_t i = 0; i < lights.size(); i++){

			LightAttributes lightAttributes;;
		
			lightAttributes.position = lights[i].position;
			lightAttributes.color = lights[i].color;
			lightAttributes.direction = lights[i].direction;
		
			outBinary.write(reinterpret_cast<char*>(&lightAttributes), sizeof(lightAttributes));
		
		}

		cout << "[OK] Exported " << nrOfLights << " to " << folderName << endl;

		outBinary.close();

	}
}

FbxAMatrix SE::Core::FBXConverter::GetGeometryTransformation(FbxNode* node) {

	// Geometric offset is to allow this offset to not inherit and propagate to children or its parents

	if (!node) {	// If the node is valid, continue processing the transformation

		throw std::exception("NULL for mesh geometry");

	}

	const FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(T, R, S);
}

unsigned int SE::Core::FBXConverter::FindJointIndexByName(string& jointName, Skeleton skeleton) {

	for (unsigned int i = 0; i < skeleton.hierarchy.size(); i++) {

		if (skeleton.hierarchy[i].Name == jointName) {

			return i;
		}
	}

	// If Skeleton information can't be read, inform the user of the problem. This is often due to differing names among the joints in the hierarchy.
	// Therefore, naming convention in Maya is very important. 

	throw std::exception("Skeleton information in FBX file cannot be received and might be corrupt");
}

void SE::Core::FBXConverter::ConvertToLeftHanded(FbxAMatrix &matrix) {

	// Get the translation and rotation from the matrix to be processed
	FbxVector4 translation = matrix.GetT();
	FbxVector4 rotation = matrix.GetR();

	// To convert to DirectX left handed coordinate system, we negate the z-translation and xy rotation in the matrix

	translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
	rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);

	// Update the matrix with the converted translation and rotation
	matrix.SetT(translation);
	matrix.SetR(rotation);
}

FbxMesh* SE::Core::FBXConverter::GetMeshFromRoot(FbxNode* node, string meshName) {	// Function to receive a mesh from the root node

	FbxMesh* currentMesh;

	for (int i = 0; i < node->GetChildCount(); i++) {	// Get number of children nodes from the root node

		FbxNode* pFbxChildNode = node->GetChild(i);	// Current child being processed in the file

		if (pFbxChildNode->GetNodeAttribute() == NULL) {

			continue;
		}

		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();	// Get the attribute type of the child node

		if (AttributeType != FbxNodeAttribute::eMesh) {

			continue;
		}

		string currentMeshName = pFbxChildNode->GetName();

		if (currentMeshName != meshName) {

			continue;
		}

		currentMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

	}

	return currentMesh;
}

XMFLOAT4X4 SE::Core::FBXConverter::Load4X4Transformations(FbxAMatrix fbxMatrix) {

	XMFLOAT4X4 matrix;

	matrix.m[0][0] = fbxMatrix.Get(0, 0);
	matrix.m[0][1] = fbxMatrix.Get(0, 1);
	matrix.m[0][2] = fbxMatrix.Get(0, 2);
	matrix.m[0][3] = fbxMatrix.Get(0, 3);

	matrix.m[1][0] = fbxMatrix.Get(1, 0);
	matrix.m[1][1] = fbxMatrix.Get(1, 1);
	matrix.m[1][2] = fbxMatrix.Get(1, 2);
	matrix.m[1][3] = fbxMatrix.Get(1, 3);

	matrix.m[2][0] = fbxMatrix.Get(2, 0);
	matrix.m[2][1] = fbxMatrix.Get(2, 1);
	matrix.m[2][2] = fbxMatrix.Get(2, 2);
	matrix.m[2][3] = fbxMatrix.Get(2, 3);

	matrix.m[3][0] = fbxMatrix.Get(3, 0);
	matrix.m[3][1] = fbxMatrix.Get(3, 1);
	matrix.m[3][2] = fbxMatrix.Get(3, 2);
	matrix.m[3][3] = fbxMatrix.Get(3, 3);

	return matrix;
}

void SE::Core::FBXConverter::Print4x4Matrix(FbxAMatrix fbxMatrix) {

	logFile << fbxMatrix.Get(0, 0) << "  "
			<< fbxMatrix.Get(0, 1) << "  "
			<< fbxMatrix.Get(0, 2) << "  "
			<< fbxMatrix.Get(0, 3) << "\n"

			<< fbxMatrix.Get(1, 0) << "  "
			<< fbxMatrix.Get(1, 1) << "  "
			<< fbxMatrix.Get(1, 2) << "  "
			<< fbxMatrix.Get(1, 3) << "\n"

			<< fbxMatrix.Get(2, 0) << "  "
			<< fbxMatrix.Get(2, 1) << "  "
			<< fbxMatrix.Get(2, 2) << "  "
			<< fbxMatrix.Get(2, 3) << "\n"

			<< fbxMatrix.Get(3, 0) << "  "
			<< fbxMatrix.Get(3, 1) << "  "
			<< fbxMatrix.Get(3, 2) << "  "
			<< fbxMatrix.Get(3, 3) << "\n\n";
}

string SE::Core::FBXConverter::getFilename(string const& path) {

	const size_t last_slash_idx = path.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		return path.substr(last_slash_idx + 1);
	}
	return path;
}

string SE::Core::FBXConverter::removeExtension(const string& path) {

	const size_t period_idx = path.find_last_of('.');
	if (std::string::npos != period_idx)
	{
		return path.substr(0, period_idx);
	}
	return path;
}