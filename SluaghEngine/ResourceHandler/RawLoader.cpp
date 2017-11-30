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
	char* appdataBuffer;
	size_t bcount;
	if (_dupenv_s(&appdataBuffer, &bcount, "APPDATA"))
		throw std::exception("Failed to retrieve path to appdata.");
	std::string appdata(appdataBuffer);
	free(appdataBuffer);
	appdata += "\\Sluagh\\";
	ifstream in(appdata + "rawLoaderEntries.txt", ios::in);
	if (!in.is_open())
		ProfileReturnConst(-1);
	
	std::string s;
	while (getline(in, s))
	{
		s = appdata + s;
		auto& name = Utilz::getFilename(s);
		auto& ext = Utilz::getExtension(s);
		auto& nh = Utilz::GUID(name);
		auto& eh = Utilz::GUID(ext);

		ifstream file(s, ios::in | ios::binary | ios::ate);
		if (!file.is_open())
			ProfileReturnConst(-2);

		size_t size = file.tellg();

		file.close();

		resourceEntires[nh] = { eh, s, Utilz::removeExtension( name), size };

		getline(in, s);
		auto guid = s;
	}
	in.close();
	ProfileReturnConst(0);
}

void SE::ResourceHandler::RawLoader::GetAllGUIDsWithExtension(const Utilz::GUID & ext, std::vector<Utilz::GUID>& guids) const
{
	for (auto& e : resourceEntires)
		if (e.second.ext == ext)
			guids.push_back(e.first);
}
void SE::ResourceHandler::RawLoader::GetAllGUIDsWithExtension(const Utilz::GUID& ext, std::vector<Utilz::GUID>& guids, std::vector<std::string>& names)const
{
	for (auto& e : resourceEntires)
		if (e.second.ext == ext)
		{
			guids.push_back(e.first);
			names.push_back(e.second.name);
		}
}
int SE::ResourceHandler::RawLoader::LoadResource(const Utilz::GUID & guid, void ** data)const
{
	_ASSERT(data);
	StartProfile;
	auto find = resourceEntires.find(guid);
	if (find == resourceEntires.end())
		ProfileReturnConst( -1);

	ifstream file(find->second.path, ios::in | ios::binary);
	if (!file.is_open())
		ProfileReturnConst( -2);

	*data = operator new(find->second.size);
	file.read((char*)*data, find->second.size);
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
		*size = find->second.size;
	ProfileReturnConst(true);
}
