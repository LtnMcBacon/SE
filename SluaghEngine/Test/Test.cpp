#include <Utilz\Console.h>
#include <Utilz\CMDConsole.h>
#include <Utilz\GUID.h>
#include "Test.h"
#include "EntityManagerTest.h"
#include "InitGraphicsTest.h"
#include "ResourceHandlerTest.h"
//#include "ObjLoaderTest.h"
#include "WindowTest.h"
#include "TransformManagerTest.h"
#include "INITest.h"
#include "RenderableManagerTest.h"
#include "FlowFieldTest.h"
#include "MaterialManagerTest.h"
#include "BoundingTest.h"
#include "ImageLoadTest.h"
#include "AllocatorTest.h"
#include "AudioTest.h"
#include "DebugRenderTest.h"
#include "InstancingTest.h"
#include <map>
#include <ctime>
#include "GameStateTest.h"
#include <crtdbg.h>
#include "EnemyMovementTest.h"
#include "RoomCreationTest.h"
#include "PlayerMovementTest.h"
#include "ProjectileTest.h"
#include "PickingTest.h"
#include "LightTest.h"
#include "BehavioursTest.h"
#include "RecordingTest.h"
#include "BehaviouralTreeFactoryTest.h"
#include "SkeletonAnimationTest.h"
#include "GarbageTest.h"
#include "ConsoleTest.h"
#include "EnemyFactoryTest.h"
#include "TransformTest.h"
#include "RecordingProjectileTest.h"
#include "GlaistigTest.h"
#include "SlaughTest.h"


#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

using namespace SE::Utilz;
using namespace SE::Test;

std::map<SE::Utilz::GUID, std::tuple<const char*, Test*>, SE::Utilz::GUID::Compare> tests;
template <typename TestType>
void TypesafeTestAdding(const char* nameOfTest) { tests[SE::Utilz::GUID(nameOfTest)] = { nameOfTest, new TestType }; }

#define AddTest(x) TypesafeTestAdding<x>(#x)//{tests[SE::Utilz::GUID(#x)] = { #x, new x };}

int main(int argc, char** argv)
{
	srand(time(NULL));
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//_crtBreakAlloc = 26812;

	//std::map<SE::Utilz::GUID, std::tuple<const char*,Test*>, SE::Utilz::GUID::Compare> tests;
	AddTest(EntityManagerTest);
	AddTest(ResourceHandlerTest);
	AddTest(WindowTest);
	//AddTest(ObjLoaderTest);
	AddTest(RenderableManagerTest);
	AddTest(SkeletonAnimationTest);
	AddTest(INITest);
	AddTest(MaterialManagerTest);
	AddTest(AudioTest);
	AddTest(RecordingProjectileTest);
	//AddTest(InitGraphicsTest);
	AddTest(TransformManagerTest);
	AddTest(ImageLoadTest);
	AddTest(BoundingTest);
	AddTest(AllocatorTest);
	AddTest(LightTest);
	AddTest(RecordingTest);
	AddTest(DebugRenderManagerTest);
	AddTest(InstancingTest);

	AddTest(FlowFieldTest);
	AddTest(EnemyMovementTest);
	AddTest(RoomCreationTest);
	AddTest(PlayerMovementTest);
	AddTest(GameStateTest);
	AddTest(BehavioursTest);
	AddTest(BehaviouralTreeFactoryTest);
	AddTest(EnemyFactoryTest);
	AddTest(GlaistigTest);
	AddTest(SlaughTest);

	AddTest(PickingTest);

  
	AddTest(ProjectileTest);
	AddTest(GarbageTest);
	AddTest(ConsoleTest);
	AddTest(TransformTest);
  
	volatile bool running = true;
	Console::Initialize(new CMDConsole);
	Console::AddCommand([&running](IConsoleBackend* backend, int argc, char** argv)
	{
		running = false;
	},
		"exit",
	
		"exit the application");

	Console::AddCommand([](IConsoleBackend* backend, int argc, char** argv)
	{
		if (argc == 1 || std::string(argv[1]) == "-h")
		{
			backend->Print("\nStart a test: runtest [name of test] [options]\n");
			backend->Print("\t -Options:\n");
			backend->Print("\t\t -h - Show help.\n");
			backend->Print("\t\t -a - Run all tests.\n");
			backend->Print("\t -Available Tests:\n");

			for(auto& t : tests)
				backend->Print("\t\t %s\n", std::get<0>(t.second));
		}
		else if (std::string(argv[1]) == "-a")
		{		
			try
			{
				bool allTest = true;
				for (auto& t : tests)
				{
					backend->Print("Running test: %s...\n", std::get<0>(t.second));
					bool result = std::get<1>(t.second)->Run(backend);
					if (!result) allTest = false;
					backend->Print("Test %s %s\n\n", std::get<0>(t.second), result ? "succeeded" : "failed");
				}
				backend->Print("Results: %s\n\n", allTest ? "All tests were successful." : "One or more tests failed.");
			}
			catch (const std::exception& exe)
			{
				backend->Print("\nExeception: %s\n", exe.what());
			}
		}
		else
		{
			auto& find = tests.find(SE::Utilz::GUID(argv[1]));
			if (find != tests.end())
			{
				try
				{
					bool result = std::get<1>(find->second)->Run(backend);
					backend->Print("Test %s %s\n\n", std::get<0>(find->second), result ? "succeeded" : "failed\n");
				}
				catch (const std::exception& exe)
				{
					backend->Print("%s Exeception: %s\n", std::get<0>(find->second), exe.what());
				}
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

	for (auto& test : tests)
		delete std::get<1>(test.second);
	Console::Hide();
	Console::Shutdown();

	return 0;

}