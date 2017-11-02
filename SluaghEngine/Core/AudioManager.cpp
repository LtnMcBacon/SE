#include "AudioManager.h"
#include <Profiler.h>

namespace SE {
	namespace Core {
		AudioManager::AudioManager(const InitializationInfo& initInfo) : initInfo(initInfo)
		{

			_ASSERT(initInfo.entityManager);
			_ASSERT(initInfo.transformManager);
			_ASSERT(initInfo.resourceHandler);
			_ASSERT(initInfo.console);
			Init();
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
		void AudioManager::Init()
		{
			initInfo.transformManager->RegisterSetDirty({ this, &AudioManager::SetDirty });
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
					return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
				});
				if (res)
				{
					initInfo.console->PrintChannel("Resources", "Could not load sound. GUID: %u, Error: %d",  createInfo.soundFile, res);
					ProfileReturnVoid;
				}
			}

			int handle;
			if (createInfo.soundType != Audio::StereoPanSound && createInfo.soundType != Audio::StereoPanLoopSound)
				handle = audioHandler->CreateStream(sound.handle, createInfo.soundType);
			else
			{
				Audio::PanData panData;
				panData.headPos = initInfo.transformManager->GetPosition(cameraEnt);
				panData.hearingVec = initInfo.transformManager->GetRight(cameraEnt);
				panData.hearingVec.x = -panData.hearingVec.x;
				panData.hearingVec.y = -panData.hearingVec.y;
				panData.hearingVec.z = -panData.hearingVec.z;
				panData.soundPos = initInfo.transformManager->GetPosition(entity);
				handle = audioHandler->CreatePanStream(sound.handle, createInfo.soundType, panData);
			}
			if (handle < 0)
			{
				initInfo.console->PrintChannel("Warning", "Could not create stream. GUID: %u, Error: %d",  createInfo.soundFile, handle);
				ProfileReturnVoid;
			}
			sound.refCount++;
			entityEntry.guidToStream[createInfo.soundFile].stream = handle;
			entityEntry.guidToStream[createInfo.soundFile].soundType = createInfo.soundType;
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
					audioHandler->StreamSound(findS->second.stream);
					findS->second.playState = true;
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
					audioHandler->StopSound(findS->second.stream);
					findS->second.playState = true;
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
							initInfo.console->PrintChannel("Warning", "Could not remove sound. GUID: %u, Error: %d",  soundFile, res);
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
			timer->Start(CREATE_ID_HASH("AudioManager"));
			GarbageCollection();
			if (PanExist)
				UpdateDirtyTransforms();
			timer->Stop(CREATE_ID_HASH("AudioManager"));
		}

		void AudioManager::SetCameraEnt(const Entity & entity)
		{
			cameraEnt = entity;
			PanExist = true;
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
				audioHandler->RemoveSound(s.second.stream);
			}
			entToSounds.erase(soundEntity[index]);
			soundEntity[index] = soundEntity[soundEntity.size() - 1];
			soundEntity.pop_back();
			StopProfile;
		}

		void AudioManager::Destroy(const Entity & entity)
		{

		}

		void SE::Core::AudioManager::SetDirty(const Entity & entity, size_t index)
		{
			if (entity == cameraEnt)
			{
				cameraMove = true;
			}
			dirtyEntites.push_back({ index, entity });
		}

		void SE::Core::AudioManager::UpdateDirtyTransforms()
		{
			StartProfile;
			Audio::PanData panData;
			panData.headPos = initInfo.transformManager->GetPosition(cameraEnt);
			panData.hearingVec = initInfo.transformManager->GetRight(cameraEnt);
			panData.hearingVec.x = -panData.hearingVec.x;
			panData.hearingVec.y = -panData.hearingVec.y;
			panData.hearingVec.z = -panData.hearingVec.z;
			if (cameraMove)
			{
				for (auto& ent : soundEntity)
				{
					auto& findE = entToSounds.find(ent);
					if (findE != entToSounds.end())
					{
						for (auto& sounds : findE->second.guidToStream)
						{
							if (sounds.second.playState == true && (sounds.second.soundType == Audio::StereoPanSound || sounds.second.soundType == Audio::StereoPanLoopSound))
							{
								panData.soundPos = initInfo.transformManager->GetPosition(findE->first);
								audioHandler->UpdateStreamPos(sounds.second.stream, panData);
							}
						}
					}
				}
				cameraMove = false;
			}
			else
			{
				for (auto& dirty : dirtyEntites)
				{
					auto& findE = entToSounds.find(dirty.entity);
					if (findE != entToSounds.end())
					{
						for (auto& sounds : findE->second.guidToStream)
						{
							if (sounds.second.playState == true && (sounds.second.soundType == Audio::StereoPanSound || sounds.second.soundType == Audio::StereoPanLoopSound))
							{
								panData.soundPos = initInfo.transformManager->GetPosition(findE->first);
								audioHandler->UpdateStreamPos(sounds.second.stream, panData);
							}
						}
					}
				}
			}
			dirtyEntites.clear();
			StopProfile;
		}
	}	//namespace Core
}	//namespace SE