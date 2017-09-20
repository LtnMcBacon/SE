#include "AudioTest.h"
#include <Core\AudioManager.h>
#include <Core\Engine.h>
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

			int streamID[10]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
			if (audio.CreateStream(Utilz::GUID("Cout.wav"), Audio::SoundIndexName::Effect) == -1)
			{
				console->Print("Sound is not loaded!!!!!!!!");
				return false;
			}
			else
			{
				streamID[0] = 0;
				audio.PlaySound(streamID[0]);
				e.GetWindow()->MapActionButton(0, Window::KeyEscape);
				e.GetWindow()->MapActionButton(1, Window::KeyW);
				e.GetWindow()->MapActionButton(2, Window::KeyS);
				e.GetWindow()->MapActionButton(3, Window::KeyR);
				while (e.GetWindow()->ButtonPressed(0) != true)
				{
					e.Frame();
					if (e.GetWindow()->ButtonPressed(1) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (streamID[i] == -1)
							{
								streamID[i] = audio.CreateStream(Utilz::GUID("Cout.wav"), Audio::SoundIndexName::Effect);
								audio.PlaySound(streamID[i]);
								i = 11;
							}
						}
					}
					if (e.GetWindow()->ButtonPressed(2) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (streamID[i] != -1)
							{
								audio.StopSound(streamID[i]);
								audio.RemoveSound(streamID[i]);
								streamID[i] = -1;
								i = 11;
							}
						}
					}
					if (e.GetWindow()->ButtonPressed(3) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (streamID[i] != -1)
							{
								audio.StopSound(streamID[i]);
								audio.PlaySound(streamID[i]);
								i = 11;
							}
						}
					}
				}
				e.Release();
				return true;
			}
			return false;
		}
	}
}