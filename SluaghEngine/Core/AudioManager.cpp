#include <Core\AudioManager.h>
#include <Core\Engine.h>
#include <Profiler.h>

namespace SE {
	namespace Core {
		AudioManager::AudioManager()
		{
			
		}

		AudioManager::~AudioManager()
		{
			
		}

		int AudioManager::Initialize()
		{
			StartProfile;
			audioSound.Initialize();
			if (audioStream.Initialize() == -1)
			{
				ProfileReturnConst(-1);
			}
			ProfileReturnConst(0);
		}

		int AudioManager::retSoundData(const Utilz::GUID & guid, void * data, size_t size)
		{
			StartProfile;
			Audio::AudioFile *sound = new Audio::AudioFile;
			sound->size = size;
			sound->soundData = (char*)data;
			sound->currentPos = 0;
			trackSound[guid] = audioSound.LoadSound(sound);
			delete sound;
			ProfileReturn(0);
		}

		int AudioManager::LoadSound(Utilz::GUID soundFile)
		{
			StartProfile;
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded == trackSound.end())
			{	
				Core::Engine::GetInstance().GetResourceHandler()->LoadResource(soundFile, ResourceHandler::LoadResourceDelegate::Make<AudioManager, &AudioManager::retSoundData>(this));
				ProfileReturnConst(1);
			}
			else
			{
				ProfileReturnConst(0);
			}
		}
		
		int AudioManager::CreateStream(Utilz::GUID soundFile, Audio::SoundIndexName soundType)
		{
			StartProfile;
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded != trackSound.end())
			{
				if (trackSound[soundFile] != -1)
				{
					ProfileReturn(audioStream.CreateStream(soundType, (Audio::AudioOut*)audioSound.GetSample(trackSound[soundFile], soundType)));
				}
			}
			ProfileReturnConst(-1);
		}

		int AudioManager::CheckIfLoaded(Utilz::GUID soundFile)
		{
			StartProfile;
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded != trackSound.end())
			{
				ProfileReturnConst(-1);
			}
			ProfileReturnConst(0);
		}

		int AudioManager::StreamSound(int streamID)
		{
			StartProfile;
			ProfileReturn(audioStream.StreamSound(streamID));
		}

		int AudioManager::StopSound(int streamID)
		{
			StartProfile;
			ProfileReturn(audioStream.StopSound(streamID));
		}

		int AudioManager::RemoveSound(int streamID)
		{
			StartProfile;
			ProfileReturn(audioStream.RemoveSound(streamID));
		}

		void AudioManager::Shutdown()
		{		
			StartProfile;
			audioStream.Shutdown();
			audioSound.Shutdown();
			ProfileReturnVoid;
		}


	}	//namespace Core
}	//namespace SE