#include "INITest.h"
#include <Utilz\INIReader.h>


namespace SE
{
	namespace Test
	{
		INITest::INITest()
		{

		}

		INITest::~INITest()
		{

		}

		bool INITest::Run(Utilz::IConsoleBackend* console)
		{
			Utilz::INIReader reader;
			std::map<std::string, std::map<std::string, int>> maps;
			if (reader.LoadINI("Asset/temp.ini", maps) != 0)
			{
				console->Print("failed to load \n");
				return false;
			}

			if (reader.WriteToINI("Asset/temp.ini", maps) != 0)
			{
				console->Print("failed to write \n");
				return false;
			}
			return true;

		}
	}
}