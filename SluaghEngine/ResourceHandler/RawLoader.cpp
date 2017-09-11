#include "RawLoader.h"
#include <fstream>
#include <string>
using namespace std;

SE::ResourceHandler::RawLoader::RawLoader()
{

	
}


SE::ResourceHandler::RawLoader::~RawLoader()
{
}

int SE::ResourceHandler::RawLoader::Initialize()
{
	ifstream in("rawLoaderEntries.txt", ios::in);
	if (!in.is_open())
		return -1;
	std::string s;
	while (getline(in, s))
	{
		resourceEntires[Utilz::GUID(hash<string>{}(s))] = s;
	}
	return 0;
}

int SE::ResourceHandler::RawLoader::LoadResource(const Utilz::GUID & guid, void ** data, size_t * size)const
{
	auto find = resourceEntires.find(guid);
	if (find == resourceEntires.end())
		return -1;

	ifstream file(find->second, ios::in | ios::ate);
	if (!file.is_open())
		return -2;

	*size = file.tellg();
	*data = new char[*size];
	file.seekg(0);
	file.read((char*)*data, *size);

	file.close();

	return 0;

}

bool SE::ResourceHandler::RawLoader::Exist(const Utilz::GUID & guid)const
{
	auto find = resourceEntires.find(guid);
	if (find == resourceEntires.end())
		return false;

	return true;
}
