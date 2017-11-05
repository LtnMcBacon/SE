#include <DevConsole\CMDConsole.h>
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
#include "PlaybackProjectileTest.h"
#include "BloomTest.h"
#include "RandRoomTest.h"
#include "GlaistigTest.h"
#include "SlaughTest.h"
#include "DecalTest.h"
#include "ShadowTest.h"
#include "ComputeTest.h"
#include "SkillTest.h"
#include "ThreadPoolTest.h"

#ifdef _DEBUG
#pragma comment(lib, "DevConsoleD.lib")
#else
#pragma comment(lib, "DevConsole.lib")
#endif

using namespace SE::Utilz;
using namespace SE::Test;

std::map<SE::Utilz::GUID, std::tuple<const char*, Test*>, SE::Utilz::GUID::Compare> tests;
template <typename TestType>
void TypesafeTestAdding(const char* nameOfTest) { tests[SE::Utilz::GUID(nameOfTest)] = { nameOfTest, new TestType }; }

#define AddTest(x) TypesafeTestAdding<x>(#x)//{tests[SE::Utilz::GUID(#x)] = { #x, new x };}

int main(int argc, char** argv)
{
	//srand(time(NULL));
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//_crtBreakAlloc = 6064;


	//std::map<SE::Utilz::GUID, std::tuple<const char*,Test*>, SE::Utilz::GUID::Compare> tests;
	AddTest(ComputeTest);
	AddTest(EntityManagerTest);
	AddTest(ResourceHandlerTest);
	//AddTest(WindowTest);
	////AddTest(ObjLoaderTest);
	AddTest(RenderableManagerTest);
	AddTest(SkeletonAnimationTest);
	//AddTest(INITest);
	//AddTest(MaterialManagerTest);
	AddTest(AudioTest);
	//AddTest(RecordingProjectileTest);
	AddTest(PlaybackProjectileTest);
	//AddTest(InitGraphicsTest);
	AddTest(TransformManagerTest);
	//AddTest(ImageLoadTest);
	//AddTest(BoundingTest);
	//AddTest(AllocatorTest);
	//AddTest(LightTest);
	//AddTest(RecordingTest);
	//AddTest(DebugRenderManagerTest);
	AddTest(InstancingTest);

	//AddTest(FlowFieldTest);
	//AddTest(EnemyMovementTest);
	//AddTest(RoomCreationTest);
	//AddTest(PlayerMovementTest);
	//AddTest(GameStateTest);
	//AddTest(BehavioursTest);
	//AddTest(BehaviouralTreeFactoryTest);
	//AddTest(EnemyFactoryTest);
	AddTest(GlaistigTest);
	AddTest(SlaughTest);

	AddTest(PickingTest);
	AddTest(SkillTest);

	//AddTest(PickingTest);
	AddTest(ShadowTest);
  
	AddTest(ProjectileTest);
	//AddTest(GarbageTest);
	//AddTest(ConsoleTest);
	//AddTest(TransformTest);
	AddTest(DecalTest);
	AddTest(BloomTest);
	AddTest(RandRoomTest); 
	AddTest(ThreadPoolTest);
  
	volatile bool running = true;
	SE::DevConsole::IConsole* console = new SE::DevConsole::CMDConsole();
	console->Initialize();
	console->AddCommand([&running](SE::DevConsole::IConsole* backend, int argc, char** argv)
	{
		running = false;
	},
		"exit",
	
		"exit the application");

	console->AddCommand([](SE::DevConsole::IConsole* backend, int argc, char** argv)
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
	









	console->Show();

	


	while (running);

	for (auto& test : tests)
		delete std::get<1>(test.second);
	console->Hide();
	console->Shutdown();
	delete console;
	return 0;

}