
// This is required to link DirectX from the Windows SDK
#include <d3d11.h>	
#include <d3dcompiler.h>	

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

// For some strange reason, DirectXMath must be included in order to find the namespace "DirectX"
#include <DirectXMath.h>

// The core header file of the FBX SDK
#include <fbxsdk.h>

#include <iostream>
#include <istream>
#include <ostream>

#include <windows.h>

// The FBX Converter used to create custom format
#include "FBXConverter.h"

#include <experimental/filesystem>

//----------------------------------------------------------------------------------------------------------------------------------//
// HOW TO LINK THE FBX SDK
//----------------------------------------------------------------------------------------------------------------------------------//

// 1. In Properties -> General, set Character Set to "Unicode" to use DirectX wstrings

// 2. In Properties -> VC++ Directories, add "FBX SDK\include" to Include Directories

// 3. In Properties -> C++ -> Preprocessor, add "FBXSDK_SHARED" to Preprocessor Definitions

// 4. In Properties -> C++ -> Code Generation, set Runtime Library to "Multi-threaded Debug DLL (/MDd)" for Debug 
// and "Multi-threaded DLL (/MD)" for Release

// 5. In Properties -> Linker -> General, set Additional Library Directories to "FBX SDK\lib\vs2012\x64\release" (or debug depending on configuration)

// 6. In Properties -> Linker -> Input, add "libfbxsdk.lib" to Additional Dependencies

using namespace DirectX;
using namespace std;
using namespace std::experimental::filesystem;

// One instance of an FBX Converter represents an entire file that can contain the following:
// 1. Standard Meshes ( Geometry without deformer attached )
// 2. Skinned Meshes ( Geometry with deformer attached )
// 3. Material Attributes and Texture on the diffuse channel (1 texture per object)
// 4: Lights
// 5. Cameras
SE::FBX::FBXConverter File;

int main(int argc, char *argv[]) {

	if (argc != 3) {

		cout << "Please enter the correct number of parameters\nFilepath, Export Folder" << endl;

		exit(0);

	}

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag

	// Filesystem can give us the solution directory with a simple function call, or we define our own in command prompt and store it.
	//path path = std::experimental::filesystem::current_path();
	//path.string();

	string loadPath = argv[1];

	if (!File.Load(loadPath.c_str(), argv[2], loadPath.c_str())) {

		exit(0);
	}

	File.Write();

	File.Deallocate();
	
	return 0;
}

