#include "AudioManager.h"
#include <Core\Engine.h>

namespace SE {
	namespace Audio {
		AudioManager::AudioManager()
		{

		}

		AudioManager::~AudioManager()
		{

		}

		int AudioManager::Initialise()
		{

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

		int AudioManager::playSound(Utilz::GUID soundFile, SoundIndexName soundType)
		{
			return audioStream->StreamSound(soundType, audioSound->GetSample(trackSound[soundFile], soundType));
		}

		void AudioManager::Shutdown()
		{

		}


	}	//namespace Audio
}	//namespace SE