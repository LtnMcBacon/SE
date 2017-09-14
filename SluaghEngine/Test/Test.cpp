#include <Utilz\Console.h>
#include <Utilz\CMDConsole.h>
#include <Utilz\GUID.h>
#include "Test.h"
#include "EntityManagerTest.h"
#include "InitGraphicsTest.h"
#include "ResouceHandlerTest.h"
#include "ObjLoaderTest.h"
#include "WindowTest.h"
#include <map>


#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif


#define AddTest(x) {tests[SE::Utilz::GUID(#x)] = { #x, new x };}

using namespace SE::Utilz;
using namespace SE::Test;
int main(int argc, char** argv)
{
	std::map<SE::Utilz::GUID, std::tuple<const char*,Test*>, SE::Utilz::GUID::Compare> tests;
	AddTest(EntityManagerTest);
	AddTest(ResouceHandlerTest);
	AddTest(WindowTest);
	AddTest(ObjLoaderTest);
	volatile bool running = true;
	AddTest(InitGraphicsTest);
	Console::Initialize(new CMDConsole);
	Console::AddCommand([&running](IConsoleBackend* backend, int argc, char** argv)
	{
		running = false;
	},
		"exit",
		"exit the application");

	Console::AddCommand([&tests](IConsoleBackend* backend, int argc, char** argv)
	{
		if (argc == 1 || std::string(argv[1]) == "-h")
		{
			backend->Print("\nStart a test: runtest [name of test]\n");
			backend->Print("\t -Available Tests\n");

			for(auto& t : tests)
				backend->Print("\t\t %s\n", std::get<0>(t.second));
		}
		else
		{
			auto& find = tests.find(SE::Utilz::GUID(argv[1]));
			if (find != tests.end())
			{
				bool result = std::get<1>(find->second)->Run(backend);
				backend->Print("Test %s %s\n\n", std::get<0>(find->second), result ? "succeeded" : "failed");
			}
				
			else
			{
				backend->Print("Test not found, %s\n\n", argv[1]);
			}
		}
			
	},
		"runtest",
		"Start a test");
	









	Console::Show();

	


	while (running);

	return 0;

}