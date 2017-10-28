#include "RawLoader.h"
#include <fstream>
#include <string>
#include <Profiler.h>
#include <Utilz\PathParsing.h>
using namespace std;

SE::ResourceHandler::RawLoader::RawLoader()
{

	
}


SE::ResourceHandler::RawLoader::~RawLoader()
{
}

int SE::ResourceHandler::RawLoader::Initialize()
{
	StartProfile;
	ifstream in("rawLoaderEntries.txt", ios::in);
	if (!in.is_open())
		ProfileReturnConst(-1);
	std::string s;
	while (getline(in, s))
	{
		auto& name = Utilz::getFilename(s);
		auto& ext = Utilz::getExtension(s);
		auto& nh = Utilz::GUID(name);
		auto& eh = Utilz::GUID(ext);

		ifstream file(s, ios::in | ios::binary | ios::ate);
		if (!file.is_open())
			ProfileReturnConst(-2);

		size_t size = file.tellg();

		file.close();

		resourceEntires[nh] = { eh, s, size };
	}
	in.close();
	ProfileReturnConst(0);
}

int SE::ResourceHandler::RawLoader::LoadResource(const Utilz::GUID & guid, void ** data)const
{
	_ASSERT(data);
	StartProfile;
	auto find = resourceEntires.find(guid);
	if (find == resourceEntires.end())
		ProfileReturnConst( -1);

	ifstream file(std::get<1>(find->second), ios::in | ios::binary);
	if (!file.is_open())
		ProfileReturnConst( -2);

	*data = operator new(std::get<2>(find->second));
	file.read((char*)*data, std::get<2>(find->second));
	file.close();

	ProfileReturnConst( 0);

}

bool SE::ResourceHandler::RawLoader::Exist(const Utilz::GUID & guid, size_t* size)const
{
	StartProfile;

	auto find = resourceEntires.find(guid);
	if (find == resourceEntires.end())
		ProfileReturnConst(false);

	if(size)
		*size = std::get<2>(find->second);
	ProfileReturnConst(true);
}
