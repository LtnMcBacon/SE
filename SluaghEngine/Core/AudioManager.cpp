#include <Core\AudioManager.h>
#include <Core\Engine.h>


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
			audioSound = new Audio::AudioSound();
			audioStream = new Audio::AudioStream();
			if (audioStream->Initialize() == -1)
			{
				return -1;
			}
			return 0;
		}

		void AudioManager::retSoundData(const Utilz::GUID & guid, void * data, size_t size)
		{
			Audio::AudioFile *sound = new Audio::AudioFile;
			sound->size = size;
			sound->soundData = (char*)data;
			sound->currentPos = 0;
			trackSound[guid] = audioSound->LoadSound(sound);
			delete sound;
		}

		int AudioManager::LoadSound(Utilz::GUID soundFile)
		{
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded == trackSound.end())
			{	
				Core::Engine::GetInstance().GetResourceHandler()->LoadResource(soundFile, ResourceHandler::LoadResourceDelegate::Make<AudioManager, &AudioManager::retSoundData>(this));
				return 1;
			}
			else
			{
				return 0;
			}
		}
		
		int AudioManager::CreateStream(Utilz::GUID soundFile, Audio::SoundIndexName soundType)
		{
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded != trackSound.end())
			{
				return audioStream->CreateStream(soundType, (Audio::AudioOut*)audioSound->GetSample(trackSound[soundFile], soundType));
			}
			return -1;
		}

		int AudioManager::CheckIfLoaded(Utilz::GUID soundFile)
		{
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded != trackSound.end())
			{
				return -1;
			}
			return 0;
		}

		int AudioManager::PlaySound(int streamID)
		{
			return audioStream->PlaySound(streamID);
		}

		int AudioManager::StopSound(int streamID)
		{
			return audioStream->StopSound(streamID);
		}

		int AudioManager::RemoveSound(int streamID)
		{
			return audioStream->RemoveSound(streamID);
		}

		void AudioManager::Shutdown()
		{			
			delete audioStream;
			delete audioSound;
		}


	}	//namespace Core
}	//namespace SE