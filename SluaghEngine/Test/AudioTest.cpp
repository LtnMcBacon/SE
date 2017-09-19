#include "AudioTest.h"
#include <Core\AudioManager.h>
#include <Core\Engine.h>
#include <Window\Window.h>
#include <portaudio\portaudio.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "windowD.lib")
#else
#pragma comment(lib, "window.lib")
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
			Core::AudioManager& audio = e.GetAudioManager();

			if (audio.LoadSound(Utilz::GUID("Cout.wav")) == 0)
			{
				console->Print("Sound already loaded??????");
				return false;
			}

			while (audio.CheckIfLoaded(Utilz::GUID("Cout.wav")) == 0)
			{

			}

			if (audio.CreateStream(Utilz::GUID("Cout.wav"), Audio::SoundIndexName::Effect) == -1)
			{
				console->Print("Sound is not loaded!!!!!!!!");
				return false;
			}
			else
			{
				audio.PlaySound(0);
				while (e.GetWindow()->HandleMSG() != false)
				{

				}
				e.Release();
				return true;
			}
			return false;
		}
	}
}