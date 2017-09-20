#include <PathParsing.h>
#include "HLSLParser.h"



using namespace SE;
using namespace std;


int main(int argc, char* argv[])
{


	//if (argc < 3)
	//{
	//	printf("Usage: %s <source directory> [target directory]\n", argv[0]);
	//}
	//else
	{



		Parsers::HLSLParser hlslp;
		vector<Utilz::File> files;
		Utilz::get_all_files_names_within_folder("Asset", files);//argv[1], files);
		for (auto& f : files)
		{
			if (Utilz::getExtension(f.name) == "hlsl")
			{
				printf("Parsing file: %s...\n", f.name.c_str());
				
				if(hlslp.Parse(Utilz::utf8ToUtf16(f.name).c_str(), "test.bbf"))
					printf("Could not parse: %s\n", f.name);
			}
		}
	}
	
	return 0;
}