#include "ObjLoaderTest.h"
#include <Core\Engine.h>
#include <Graphics\Renderer.h>
#include <OBJParser\Parsers.h>
#include <Profiler.h>


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

static bool result = false;
namespace fuck
{
	void Load(const SE::Utilz::GUID& guid, void* data, size_t size)
	{
		auto& mD = *(Arf::Mesh::Data*)data;
		auto verts = (Arf::Mesh::Position*)mD.vertices;
		result = true;
		for (int i = 0; i < mD.NumVertices; i++)
		{
			if (verts[i].x != (float)i + 1)
				result = false;
			if (verts[i].y != (float)i + 1)
				result = false;
			if (verts[i].z != (float)i + 1)
				result = false;
		}
	}
}


bool SE::Test::ObjLoaderTest::Run(Utilz::IConsoleBackend * console)
{
	StartProfile;
	auto& e = Core::Engine::GetInstance();
	auto info = Core::Engine::InitializationInfo();
	auto re = e.Init(info);
	if (re)
	{
		console->Print("Could not init Core, Error: %d.\n", re);
		ProfileReturnConst(false);
	}

	auto r = e.GetResourceHandler();
	result = true;

	r->AddParser(Utilz::GUID("objtest"), [](void* rawData, size_t rawSize, void** parsedData, size_t* parsedSize) -> int
	{
		ArfData::Data arfData;
		ArfData::DataPointers arfp;
		auto r = Arf::ParseObj(rawData, rawSize, &arfData, &arfp);
		if (r)
			return r;
		auto data = (Arf::Mesh::Data**)parsedData;
		r = Arf::Interleave(arfData, arfp, data, parsedSize, Arf::Mesh::InterleaveOption::Position);
		if (r)
			return r;

		delete arfp.buffer;
		return 0;
	});




	r->LoadResource(Utilz::GUID("test.objtest"), ResourceHandler::LoadResourceDelegate::Make<&fuck::Load>());

	e.Release();

	ProfileReturnConst(result);
}
