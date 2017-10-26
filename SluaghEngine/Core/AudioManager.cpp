#include "AudioManager.h"
#include <Profiler.h>

namespace SE {
	namespace Core {
		AudioManager::AudioManager(const InitializationInfo& initInfo) : initInfo(initInfo)
		{

			_ASSERT(initInfo.entityManager);
			_ASSERT(initInfo.resourceHandler);
			_ASSERT(initInfo.console);
			audioHandler = Audio::CreateNewAudioHandler();
			auto res = audioHandler->Initialize();
			if (res)
				throw std::exception("Could not initialize audio systems");
		}
		AudioManager::~AudioManager()
		{
			audioHandler->Shutdown();
			delete audioHandler;
		}
		void AudioManager::Create(const Entity & entity, const CreateInfo & createInfo)
		{
			StartProfile;
			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(entity))
				ProfileReturnVoid;

			auto find = entToSounds.find(entity);
			auto& entityEntry = entToSounds[entity];
			if (find == entToSounds.end())
				soundEntity.push_back(entity);
			
			auto findStream = entityEntry.guidToStream.find(createInfo.soundFile);
			if(findStream != entityEntry.guidToStream.end())
				ProfileReturnVoid;


			auto fileLoaded = guidToSound.find(createInfo.soundFile);
			auto& sound = guidToSound[createInfo.soundFile];
			if (fileLoaded == guidToSound.end()) // Load the sound
			{
				auto res = initInfo.resourceHandler->LoadResource(createInfo.soundFile, [this](auto guid, auto data, auto size) {
					guidToSound[guid].handle = audioHandler->LoadSound(data, size);
					return ResourceHandler::InvokeReturn::Success;
				});
				if (res)
				{
					initInfo.console->PrintChannel("Could not load sound. GUID: %u, Error: %d", "Resources", createInfo.soundFile, res);
					ProfileReturnVoid;
				}
			}

			int handle = audioHandler->CreateStream(sound.handle, createInfo.soundType);
			if (handle < 0)
			{
				initInfo.console->PrintChannel("Could not create stream. GUID: %u, Error: %d", "Warning", createInfo.soundFile, handle);
				ProfileReturnVoid;
			}
			sound.refCount++;
			entityEntry.guidToStream[createInfo.soundFile] = handle;

			ProfileReturnVoid;
		}
		void AudioManager::PlaySound(const Entity & entity, const Utilz::GUID & soundFile)
		{
			auto findE = entToSounds.find(entity);
			if (findE != entToSounds.end())
			{
				auto findS = findE->second.guidToStream.find(soundFile);
				if (findS != findE->second.guidToStream.end())
				{
					audioHandler->StreamSound(findS->second);
				}
			}
		}
		void AudioManager::StopSound(const Entity & entity, const Utilz::GUID & soundFile)
		{
			auto findE = entToSounds.find(entity);
			if (findE != entToSounds.end())
			{
				auto findS = findE->second.guidToStream.find(soundFile);
				if (findS != findE->second.guidToStream.end())
				{
					audioHandler->StopSound(findS->second);
				}
			}
		}
		void AudioManager::RemoveSound(const Entity & entity, const Utilz::GUID & soundFile)
		{
			auto findE = entToSounds.find(entity);
			if (findE != entToSounds.end())
			{
				auto findS = findE->second.guidToStream.find(soundFile);
				if (findS != findE->second.guidToStream.end())
				{
					findE->second.guidToStream.erase(soundFile);
					guidToSound[soundFile].refCount--;
					if (guidToSound[soundFile].refCount == 0)
					{
						auto res = audioHandler->RemoveSound(guidToSound[soundFile].handle);
						if (res)
						{
							initInfo.console->PrintChannel("Could not remove sound. GUID: %u, Error: %d", "Warning", soundFile, res);
						}
					}
						
				}
			}
		}
		void AudioManager::SetSoundVol(const SE::Audio::SoundVolType & volType, size_t newVol)
		{
			audioHandler->SetSoundVol(volType, newVol);
		}
		void AudioManager::Frame(Utilz::TimeCluster * timer)
		{
			_ASSERT(timer);
			timer->Start("AudioManager");
			GarbageCollection();
			timer->Stop("AudioManager");
		}

		void AudioManager::GarbageCollection()
		{
			StartProfile;
			uint32_t alive_in_row = 0;

			while (soundEntity.size() > 0 && alive_in_row < 4U)
			{
				std::uniform_int_distribution<uint32_t> distribution(0U, soundEntity.size() - 1U);
				uint32_t i = distribution(generator);
				if (initInfo.entityManager->Alive(soundEntity[i]))
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
			for (auto& s : entToSounds[soundEntity[index]].guidToStream)
			{
				audioHandler->RemoveSound(s.second);
			}
			entToSounds.erase(soundEntity[index]);
			soundEntity[index] = soundEntity[soundEntity.size() - 1];
			soundEntity.pop_back();
			StopProfile;
		}

		void AudioManager::Destroy(const Entity & entity)
		{

		}

	}	//namespace Core
}	//namespace SE