#include "AudioTest.h"
#include <Core\AudioManager.h>
#include <Core\Engine.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "audioD.lib")
#else
#pragma comment(lib, "audio.lib")
#endif

namespace SE
{
	namespace Test
	{
		AudioTest::AudioTest() 
		{

		}
		AudioTest::~AudioTest()
		{

		}

		bool AudioTest::Run(SE::Utilz::IConsoleBackend* console)
		{		
			auto& e = Core::Engine::GetInstance();
			auto& info = Core::Engine::InitializationInfo();
			auto re = e.Init(info);
			if (re)
			{
				console->Print("Could not init Core, Error: %d.", re);
				return false;
			}

			Core::AudioManager* audioManager = new Core::AudioManager();

			if (audioManager->Initialize() != 0)
			{
				console->Print("Init failed");
				return false;
			}

			if (audioManager->LoadSound(Utilz::GUID("Cout.wav")) == 0)
			{
				console->Print("Sound already loaded??????");
				return false;
			}

			while (audioManager->CheckIfLoaded(Utilz::GUID("Cout.wav")) == 0)
			{

			}

			if (audioManager->playSound(Utilz::GUID("Cout.wav"), Audio::SoundIndexName::BakgroundSound) == -1)
			{
				console->Print("Sound is not loaded!!!!!!!!");
				return false;
			}

			for (int i = 0; 0 < 50000; i++)
			{
				console->Print("playing sound!!!");
			}
			console->Print("no longer playing sound!!!");
			getchar();
			audioManager->Shutdown();
			return true;
		}
	}
}