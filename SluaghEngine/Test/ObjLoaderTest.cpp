#include "ObjLoaderTest.h"
#include <Core\Engine.h>
#include <Graphics\Renderer.h>
#include <OBJParser\Parsers.h>


#ifdef _DEBUG
#pragma comment(lib, "OBJParserD.lib")
#else
#pragma comment(lib, "OBJParser.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "CoreD.lib")
#else
#pragma comment(lib, "Core.lib")
#endif

SE::Test::ObjLoaderTest::ObjLoaderTest()
{
}


SE::Test::ObjLoaderTest::~ObjLoaderTest()
{
}

bool SE::Test::ObjLoaderTest::Run(Utilz::IConsoleBackend * console)
{
	auto& e = Core::Engine::GetInstance();
	auto& info = Core::Engine::InitializationInfo();
	auto re = e.Init(info);
	if (re)
	{
		console->Print("Could not init Core, Error: %d.", re);
		return false;
	}

	auto r = e.GetResourceHandler();
	bool result = false;


	r->AddParser(Utilz::GUID("objtest"), [](void* rawData, size_t rawSize, void** parsedData, size_t* parsedSize) -> int
	{
		ArfData::Data arfData;
		ArfData::DataPointers arfp;
		auto r = Arf::ParseObj(rawData, rawSize, &arfData, &arfp);
		if (r)
			return r;

		*parsedSize = sizeof(Graphics::MeshData::MeshData) + sizeof(Graphics::MeshData::Vertex)*(arfData.NumFace * 3);
		*parsedData = operator new (*parsedSize);
		auto meshData = (Graphics::MeshData::MeshData*)*parsedData;
		meshData->NumVertices = arfData.NumFace * 3;
		meshData->vertices = ((Graphics::MeshData::Vertex*)(meshData + 1));
		auto& vertices = meshData->vertices;
		uint32_t index = 0;
		for (uint32_t i = 0; i < arfData.NumSubMesh; i++)
		{
			for (uint32_t j = arfp.subMesh[i].faceStart; j < arfp.subMesh[i].faceCount; j++)
			{
				auto& face = arfp.faces[j];

				for (uint8_t r = 0; r < face.indexCount; r++)
				{
					auto& ind = face.indices[r];
					// Positions
					memcpy(&vertices[index].pos, &arfp.positions[ind.index[0]], sizeof(Graphics::MeshData::Position));
					vertices[index].pos.z = -vertices[index].pos.z;
					// Normals
					//memcpy(&vertices[index].norm, &arfp.normals[ind.index[2]], sizeof(Graphics::MeshData::Normal));
					//// TexCoords
					//if (ind.index[1] == UINT32_MAX)
					//{
					//	memcpy(&vertices[index].tex, &arfp.texCoords[0], sizeof(Graphics::MeshData::TexCoord));
					//	vertices[index].tex.v = 1 - vertices[index].tex.v;
					//}
					//else
					//{
					//	memcpy(&vertices[index].tex, &arfp.texCoords[ind.index[1]], sizeof(Graphics::MeshData::TexCoord));
					//	vertices[index].tex.v = 1 - vertices[index].tex.v;
					//}
					index++;

				}
			}
		}
		return 0;
	});




	r->LoadResource(Utilz::GUID("test.objtest"), [&result](void* data, size_t size) 
	{
		auto& mD = *(Graphics::MeshData::MeshData*)data;
		result = true;
		for (int i = 0; i < mD.NumVertices; i++)
		{
			if (mD.vertices[i].pos.x != (float)i + 1)
				result = false;
			if (mD.vertices[i].pos.y != (float)i + 1)
				result = false;
			if (-mD.vertices[i].pos.z != (float)i + 1)
				result = false;
		}
	});

	e.Release();

	return result;
}
