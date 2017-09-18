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
			AudioIn inputSound;
			inputSound.soundData = data;
			inputSound.size = size;
			trackSound[guid] = audioSound->LoadSound(&inputSound);
		}

		int AudioManager::LoadSound(Utilz::GUID soundFile, SoundIndexName soundType)
		{
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded == trackSound.end())
			{			
				Core::Engine::GetInstance().GetResourceHandler()->LoadResource(soundFile, ResourceHandler::LoadResourceDelegate::Make<AudioManager, &AudioManager::retSoundData>(this));
			}
		}

		void AudioManager::Shutdown()
		{

		}


	}	//namespace Audio
}	//namespace SE