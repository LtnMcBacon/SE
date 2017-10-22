//#include "ObjLoaderTest.h"
//#include <Core\IEngine.h>
//#include <Graphics\Renderer.h>
//#include <OBJParser\Parsers.h>
//#include <Profiler.h>
//
//
//#ifdef _DEBUG
//#pragma comment(lib, "OBJParserD.lib")
//#else
//#pragma comment(lib, "OBJParser.lib")
//#endif
//
//#ifdef _DEBUG
//#pragma comment(lib, "CoreD.lib")
//#else
//#pragma comment(lib, "Core.lib")
//#endif
//
//SE::Test::ObjLoaderTest::ObjLoaderTest()
//{
//}
//
//
//SE::Test::ObjLoaderTest::~ObjLoaderTest()
//{
//}
//
//static bool result = false;
//namespace Parsing
//{
//	int Load(const SE::Utilz::GUID& guid, void* data, size_t size)
//	{
//		ArfData::Data arfData;
//		ArfData::DataPointers arfp;
//		auto r = Arf::ParseObj(data, size, &arfData, &arfp);
//		if (r)
//			return r;
//		Arf::Mesh::Data* parsedData;
//		size_t parsedSize;
//		r = Arf::Interleave(arfData, arfp, &parsedData, &parsedSize, Arf::Mesh::InterleaveOption::Position);
//		if (r)
//			return r;
//
//		delete arfp.buffer;
//
//		auto& mD = *(Arf::Mesh::Data*)parsedData;
//		auto verts = (Arf::Mesh::Position*)mD.vertices;
//		result = true;
//		for (int i = 0; i < mD.NumVertices; i++)
//		{
//			if (verts[i].x != (float)i + 1)
//				result = false;
//			if (verts[i].y != (float)i + 1)
//				result = false;
//			if (verts[i].z != (float)i + 1)
//				result = false;
//		}
//
//		delete parsedData;
//
//		return 0;
//	}
//}
//
//
//bool SE::Test::ObjLoaderTest::Run(Utilz::IConsoleBackend * console)
//{
//	StartProfile;
//	auto& e = Core::Engine::GetInstance();
//	auto info = Core::Engine::InitializationInfo();
//	auto re = e.Init(info);
//	if (re)
//	{
//		console->Print("Could not init Core, Error: %d.\n", re);
//		ProfileReturnConst(false);
//	}
//
//	auto r = e.GetResourceHandler();
//	result = true;
//
//
//
//
//
//	r->LoadResource(Utilz::GUID("test.objtest"), ResourceHandler::LoadResourceDelegate::Make<&Parsing::Load>());
//
//	e.Release();
//
//	ProfileReturnConst(result);
//}
