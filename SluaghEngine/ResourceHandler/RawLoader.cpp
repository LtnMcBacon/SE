#include "RawLoader.h"
#include <fstream>
#include <string>
#include <Profiler.h>

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
		resourceEntires[Utilz::GUID(hash<string>{}(s))] = s;
	}
	ProfileReturnConst(0);
}

int SE::ResourceHandler::RawLoader::LoadResource(const Utilz::GUID & guid, void ** data, size_t * size)const
{
	StartProfile;
	auto find = resourceEntires.find(guid);
	if (find == resourceEntires.end())
		ProfileReturnConst( -1);

	ifstream file(find->second, ios::in | ios::ate);
	if (!file.is_open())
		ProfileReturnConst( -2);

	*size = file.tellg();
	*data = new char[*size];
	file.seekg(0);
	file.read((char*)*data, *size);

	file.close();

	ProfileReturnConst( 0);

}

bool SE::ResourceHandler::RawLoader::Exist(const Utilz::GUID & guid)const
{
	StartProfile;
	auto find = resourceEntires.find(guid);
	if (find == resourceEntires.end())
		ProfileReturnConst(false);


	ProfileReturnConst(true);
}
