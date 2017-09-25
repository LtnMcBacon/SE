#include "AudioTest.h"
#include <Core\AudioManager.h>
#include <Core\Engine.h>
#include <portaudio\portaudio.h>
#include <Utilz\MemoryMeasuring.h>

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
			Utilz::MemoryMeasuring mm;
			mm.Init();
			auto& e = Core::Engine::GetInstance();
			auto& info = Core::Engine::InitializationInfo();
			auto re = e.Init(info);
			if (re)
			{
				console->Print("Could not init Core, Error: %d.", re);
				return false;
			}
			Core::AudioManager& audio = e.GetAudioManager();
			auto& optHandler = Core::Engine::GetInstance().GetOptionHandler();

			if (audio.LoadSound(Utilz::GUID("Cout.wav")) == 0)
			{
				console->Print("Sound already loaded??????");
				e.Release();
				return false;
			}

			while (audio.CheckIfLoaded(Utilz::GUID("Cout.wav")) == 0)
			{

			}

			int streamID[10]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
			streamID[0] = audio.CreateStream(Utilz::GUID("Cout.wav"), Audio::SoundIndexName::EffectSound);
			if (streamID[0] == -1)
			{
				console->Print("Sound is not loaded!!!!!!!!");
				e.Release();
				return false;
			}
			else if (streamID[0] == -2)
			{
				console->Print("No device!!!!!!");
				e.Release();
				return false;
			}
			else
			{
				auto ent = e.GetEntityManager().Create();
				e.GetRenderableManager().CreateRenderableObject(ent, Utilz::GUID("Placeholder_Arrow.obj"));
				e.GetTransformManager().Create(ent);
				e.GetRenderableManager().ToggleRenderableObject(ent, true);

				streamID[0] = 0;
				audio.StreamSound(streamID[0]);
				e.GetWindow()->MapActionButton(0, Window::KeyEscape);
				e.GetWindow()->MapActionButton(1, Window::KeyW);
				e.GetWindow()->MapActionButton(2, Window::KeyS);
				e.GetWindow()->MapActionButton(3, Window::KeyR);

				while (e.GetWindow()->ButtonPressed(0) != true)
				{
					e.Frame();
					mm.printUsage(console);
					
					if (e.GetWindow()->ButtonPressed(1) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (streamID[i] == -1)
							{
								streamID[i] = audio.CreateStream(Utilz::GUID("Cout.wav"), Audio::SoundIndexName::EffectSound);
								if (streamID[i] == -2)
								{
									console->Print("No device!!!!!!");
								}
								else
								{
									audio.StreamSound(streamID[i]);
								}
								i = 11;
							}
						}
						optHandler.SetOption("Window", "width", 800);
						optHandler.SetOption("Window", "height", 600);
						optHandler.SetOption("Window", "fullScreen", 0);
						optHandler.Trigger();
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
						optHandler.SetOption("Window", "width", 1280);
						optHandler.SetOption("Window", "height", 720);
						optHandler.SetOption("Window", "fullScreen", 1);
						optHandler.Trigger();
					}
					if (e.GetWindow()->ButtonPressed(3) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (streamID[i] != -1)
							{
								audio.StopSound(streamID[i]);
								audio.StreamSound(streamID[i]);
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