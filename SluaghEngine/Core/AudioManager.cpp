#include <Core\AudioManager.h>
#include <Profiler.h>

namespace SE {
	namespace Core {
		AudioManager::AudioManager(ResourceHandler::IResourceHandler* resourceHandler, const EntityManager & entityManager)
			:resourceHandler(resourceHandler), entityManager(entityManager)
		{

			assert(resourceHandler);

		}

		int AudioManager::Initialize()
		{
			StartProfile;
			audioHandler = Audio::CreateNewAudioHandler();
			ProfileReturn(audioHandler->Initialize());
		}

		int AudioManager::retSoundData(const Utilz::GUID & guid, void * data, size_t size)
		{
			StartProfile;
			trackSound[guid] = audioHandler->LoadSound(data, size);
			ProfileReturn(0);
		}

		int AudioManager::LoadSound(Utilz::GUID soundFile)
		{
			StartProfile;
			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded == trackSound.end())
			{
				resourceHandler->LoadResource(soundFile, ResourceHandler::LoadResourceDelegate::Make<AudioManager, &AudioManager::retSoundData>(this));
				ProfileReturnConst(-1);
			}
			else
			{
				ProfileReturnConst(0);
			}
		}

		int AudioManager::CreateStream(const Entity& entity, Utilz::GUID soundFile, Audio::SoundIndexName soundType)
		{
			StartProfile;
			// Check if the entity is alive
			if (!entityManager.Alive(entity))
				ProfileReturnConst(-1);

			auto fileLoaded = trackSound.find(soundFile);
			if (fileLoaded != trackSound.end())
			{
				if (trackSound[soundFile] != -1)
				{
					int handle = audioHandler->CreateStream(trackSound[soundFile], soundType);
					if (handle >= 0)
					{
						if (entToSounds[entity].freeStreamID.size() > 0)
						{
							entToSounds[entity].streamID[entToSounds[entity].freeStreamID.top()] = handle;
							entToSounds[entity].freeStreamID.pop();
						}
						else
						{
							entToSounds[entity].streamID.push_back(handle);
						}
						entToSounds[entity].amountOfSound++;
						if (entToSounds[entity].amountOfSound < 2)
						{
							soundEntity.push_back(entity);
						}
					}
					ProfileReturn(handle);
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

		int AudioManager::StreamSound(const Entity& entity, int entSoundID)
		{
			StartProfile;
			// Check if the entSoundID exist
			if (entSoundID >= entToSounds[entity].amountOfSound || entToSounds[entity].streamID[entSoundID] == -1)
				ProfileReturnConst(-1);
			ProfileReturn(audioHandler->StreamSound(entToSounds[entity].streamID[entSoundID]));
		}

		int AudioManager::StopSound(const Entity& entity, int entSoundID)
		{
			StartProfile;
			// Check if the entSoundID exist
			if (entSoundID >= entToSounds[entity].amountOfSound || entToSounds[entity].streamID[entSoundID] == -1)
				ProfileReturnConst(-1);
			ProfileReturn(audioHandler->StopSound(entToSounds[entity].streamID[entSoundID]));
		}

		int AudioManager::RemoveSound(const Entity & entity, int entSoundID)
		{
			StartProfile;
			// Check if the entSoundID exist
			if (entSoundID >= entToSounds[entity].amountOfSound || entToSounds[entity].streamID[entSoundID] == -1)
				ProfileReturnConst(-1);
			if (audioHandler->RemoveSound(entToSounds[entity].streamID[entSoundID]))
			{
				entToSounds[entity].streamID[entSoundID] = -1;
				entToSounds[entity].freeStreamID.push(entSoundID);
				ProfileReturnConst(0);
			}
			ProfileReturn(-1);
		}

		void AudioManager::SetSoundVol(SE::Audio::SoundVolType volType, int newVol)
		{
			audioHandler->SetSoundVol(volType, newVol);
		}

		void AudioManager::Frame()
		{
			GarbageCollection();
		}

		void AudioManager::Shutdown()
		{
			audioHandler->Shutdown();
			delete audioHandler;
		}

		void AudioManager::GarbageCollection()
		{
			StartProfile;
			uint32_t alive_in_row = 0;

			while (soundEntity.size() > 0 && alive_in_row < 4U)
			{
				std::uniform_int_distribution<uint32_t> distribution(0U, soundEntity.size() - 1U);
				uint32_t i = distribution(generator);
				if (entityManager.Alive(soundEntity[i]))
				{
					alive_in_row++;
					continue;
				}
				alive_in_row = 0;
				Destroy(i);
			}
			StopProfile;
		}

		void AudioManager::Destroy(size_t index)
		{
			StartProfile;
			while (entToSounds[soundEntity[index]].amountOfSound > 0)
			{
				if (entToSounds[soundEntity[index]].streamID[entToSounds[soundEntity[index]].amountOfSound - 1] != -1)
				{
					RemoveSound(soundEntity[index], entToSounds[soundEntity[index]].streamID[entToSounds[soundEntity[index]].amountOfSound - 1]);
				}
			}
			entToSounds.erase(soundEntity[index]);
			soundEntity[index] = soundEntity[soundEntity.size() - 1];
			soundEntity.pop_back();
			StopProfile;
		}

	}	//namespace Core
}	//namespace SE