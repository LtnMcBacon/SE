#include <PathParsing.h>
#include "HLSLParser.h"
#include <fstream>
#include <filesystem>

namespace fs = std::experimental::filesystem;
using namespace SE;
using namespace std;


int main(int argc, char* argv[])
{

	if (argc < 3)
	{
		printf("Usage: %s <source directory> [target directory]\n", argv[0]);
	}
	else
	{
		Parsers::HLSLParser hlslp;
		vector<Utilz::File> files;
		Utilz::get_all_files_names_within_folder(argv[1], files);
		for (auto& f : files)
		{
			if (Utilz::getExtension(f.name) == "hlsl")
			{
				printf("Parsing file: %s...\n", f.name.c_str());
				
				if (hlslp.Parse(Utilz::utf8ToUtf16(f.fullPath).c_str(), (std::string(argv[2]) + "\\"+  Utilz::removeRoot(f.fullPath)).c_str()))
					printf("Could not parse: %s\n", f.name.c_str());
			}
			else
			{
				std::ifstream  src(f.fullPath, std::ios::binary);

				std::string path = (std::string(argv[2]) + "/" + Utilz::removeRoot(f.fullPath)).c_str();
				auto idx = path.find_last_of("\\/");
				auto path2 = path.substr(0, idx);
				fs::create_directories(path2);

				std::ofstream  dst(path, std::ios::binary | std::ios::trunc);

				dst << src.rdbuf();
				src.close();
				dst.close();
			}
		}


		std::ofstream gE;
		gE.open("rawLoaderEntries.txt", std::ios::trunc);
		if (gE.is_open())
		{
			for (auto& f : files)
				gE << (std::string(argv[2]) + "/" + Utilz::removeRoot(f.fullPath)) << std::endl;
		}


	}
	
	return 0;
}