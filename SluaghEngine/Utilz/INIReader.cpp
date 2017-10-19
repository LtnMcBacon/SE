#include <Utilz\INIReader.h>
#include <algorithm>
#include <cctype>
#include <Profiler.h>

// basic file operations
#include <fstream>

namespace SE
{
	namespace Utilz
	{

		INIReader::INIReader()
		{

		}

		INIReader::~INIReader()
		{

		}

		int INIReader::LoadINI(const std::string& filename, std::map<std::string, std::map<std::string, std::string>>& maps)
		{
			StartProfile;
			std::map<std::string, std::string> values;
			std::ifstream myfile;
			std::streampos size;
			char *memblock;
			myfile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
			if (myfile.is_open())
			{
				size = myfile.tellg();
				memblock = new char[size];
				myfile.seekg(0, std::ios::beg);
				myfile.read(memblock, size);
				myfile.close();
			}
			else
			{
				ProfileReturnConst(-1);
			}

			std::string section;
			std::string name;
			std::string value;
			for (int currentPos = 0; currentPos < size; currentPos++)
			{
				if (memblock[currentPos] == '[')
				{
					if (values.size() > 0)
					{
						maps[section] = values;
						values.clear();
					}
					section.clear();
					currentPos++;
					while (memblock[currentPos] != ']')
					{
						if (memblock[currentPos] != ' ')
						{
							section.push_back(memblock[currentPos]);
							currentPos++;
						}
						else
						{
							currentPos++;
						}
					}
					currentPos++;
					while (memblock[currentPos] != '\n')
					{
						currentPos++;
					}
				}
				else if (memblock[currentPos] == '\n' || memblock[currentPos] == '\r')
				{
					currentPos++;
				}
				else
				{
					name.clear();
					while (memblock[currentPos] != '=')
					{
						if (memblock[currentPos] != ' ')
						{
							name.push_back(memblock[currentPos]);
							currentPos++;
						}
						else
						{
							currentPos++;
						}
					}
					currentPos++;
					value.clear();
					while (memblock[currentPos] != '\n' )
					{
						if (memblock[currentPos] != ' ' &&  memblock[currentPos] != '\r')
						{
							value.push_back(memblock[currentPos]);
							currentPos++;
						}
						else
						{
							currentPos++;
						}
					}
					values[name] = value;
				}	
			}
			maps[section] = values;
			delete[] memblock;
			ProfileReturnConst(0);
		}

		int INIReader::WriteToINI(const std::string& filename, std::map<std::string, std::map<std::string, std::string>>& maps)
		{
			StartProfile;
			std::string memblock;

			for (auto& mapsPair : maps)
			{
				std::map<std::string, std::string> values = maps[mapsPair.first];
				memblock.push_back('[');
				memblock.append(mapsPair.first);
				memblock.push_back(']');
				memblock.push_back('\n');
				for (auto& valuesPair : values)
				{
					std::string name = valuesPair.first + " = " + values[valuesPair.first];
					memblock.append(name);
					memblock.push_back('\n');
				}
				memblock.push_back('\n');
			}

			std::ofstream myfile;
			myfile.open(filename, std::ios::out | std::ios::trunc);
			if (myfile.is_open())
			{
				myfile.write(memblock.c_str(), memblock.size());
				myfile.close();
				ProfileReturnConst(0);
			}
			ProfileReturnConst(-1);
		}
	}	//namespace SE
}	//namespace Utilz