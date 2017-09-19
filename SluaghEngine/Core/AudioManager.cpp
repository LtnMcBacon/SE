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
			trackSound[guid] = audioSound->LoadSound(data, size);
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
		
		int AudioManager::playSound(Utilz::GUID soundFile, Audio::SoundIndexName soundType)
		{
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded != trackSound.end())
			{
				audioToStream.push_back((Audio::AudioOut*)audioSound->GetSample(trackSound[soundFile], soundType));
				return audioStream->StreamSound(soundType, audioToStream[audioToStream.size() - 1]);
				//return audioStream->StreamSound(soundType, audioSound->GetSample(trackSound[soundFile], soundType));
			}
			return -1;
		}

		int AudioManager::CheckIfLoaded(Utilz::GUID soundFile)
		{
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded != trackSound.end())
			{
				return 1;
			}
			return 0;
		}

		void AudioManager::Shutdown()
		{
			delete audioSound;
			delete audioStream;
		}


	}	//namespace Core
}	//namespace SE