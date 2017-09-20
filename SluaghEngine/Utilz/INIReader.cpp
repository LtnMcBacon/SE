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

		int INIReader::LoadINI(const std::string& filename, std::map<std::string, std::map<std::string, int>>& maps)
		{
			StartProfile;
			std::map<std::string, int> values;
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
					}
					section.clear();
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
					section.push_back(memblock[currentPos]);
					currentPos++;
					while (memblock[currentPos] != '\n')
					{
						currentPos++;
					}
				}
				else if (memblock[currentPos] == '\n')
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
						if (memblock[currentPos] != ' ')
						{
							value.push_back(memblock[currentPos]);
							currentPos++;
						}
						else
						{
							currentPos++;
						}
					}
					values[name] = std::stoi(value);
				}
			}
			ProfileReturnConst(0);
		}

		int INIReader::WriteToINI(const std::string& filename, std::map<std::string, std::map<std::string, int>>& maps)
		{
			StartProfile;
			std::string memblock;
			for (auto& mapsPair : maps)
			{	
				std::map<std::string, int> values = maps[mapsPair.first];
				std::string section = '#' + mapsPair.first;
				memblock.append(section);
				memblock.push_back('\n');
				for (auto& valuesPair : values)
				{
					int outVal = values[valuesPair.first];
					std::string name = valuesPair.first + " = ";
					memblock.append(name);
					memblock.push_back(outVal);
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

		std::string INIReader::CreateKey(const std::string& section, const std::string& name)
		{
			StartProfile;
			std::string key = section + "=" + name;
			// Change all to lowercase
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			ProfileReturn(key);
		}
	}	//namespace SE
}	//namespace Utilz