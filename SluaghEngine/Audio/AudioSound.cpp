#include "AudioSound.h"
#include <Profiler.h>
#include <Core\Engine.h>

namespace SE {
	namespace Audio {
		AudioSound::AudioSound()
		{

		}

		AudioSound::~AudioSound()
		{
			
		}

		int AudioSound::Initialize()
		{
			StartProfile;
			auto& optHandler = Core::Engine::GetInstance().GetOptionHandler();
			masterVol = optHandler.GetOption("Audio", "masterVolume", 100);
			effectVol = optHandler.GetOption("Audio", "effectVolume", 80);
			bakgroundVol = optHandler.GetOption("Audio", "bakgroundVolume", 50);
			optHandler.Register(Utilz::Delegate<void()>::Make<AudioSound, &AudioSound::OptionUpdate>(this));
			sampleStack.InitStackAlloc(100000000);
			ProfileReturnConst(0);
		}

		namespace sfvirt {
			sf_count_t sf_vio_get_filelen1(void *user_data)
			{
				AudioFile *data = (AudioFile*)user_data;
				return data->size;
			};

			sf_count_t sf_vio_seek1(sf_count_t offset, int whence, void *user_data)
			{
				AudioFile *data = (AudioFile*)user_data;
				if (whence == SEEK_CUR)
				{
					if (data->currentPos + offset < data->size && data->currentPos + offset >= 0)
						data->currentPos += offset;
				}
				else if (whence == SEEK_SET)
				{
					if (offset < data->size && offset >= 0)
						data->currentPos = offset;
				}
				else if (whence == SEEK_END)
				{
					if (offset >= 0 && offset < data->size)
						data->currentPos = data->size + offset;
				}
				return data->currentPos;
			};

			sf_count_t sf_vio_read1(void *ptr, sf_count_t count, void *user_data)
			{
				AudioFile *data = (AudioFile*)user_data;
				char *saveData = (char*)ptr;
				int nrOfCpy;
				if (data->currentPos + count < data->size)
				{
					memcpy(ptr, &data->soundData[data->currentPos], count);
					data->currentPos += count;
					return count;
				}
				else
				{
					memcpy(ptr, &data->soundData[data->currentPos], data->size - data->currentPos);
					data->currentPos = data->size - 1;
					return data->size - data->currentPos;
				}
			};

			sf_count_t sf_vio_write1(const void *ptr, sf_count_t count, void *user_data)
			{
				//no write implemented
				return 1;
			};

			sf_count_t sf_vio_tell1(void *user_data)
			{
				AudioFile *data = (AudioFile*)user_data;
				return data->currentPos;
			};
		}

		int AudioSound::LoadSound(AudioFile* sound)
		{
			StartProfile;
			SF_VIRTUAL_IO sfvirtual;
			sfvirtual.get_filelen = sfvirt::sf_vio_get_filelen1;
			sfvirtual.seek = sfvirt::sf_vio_seek1;
			sfvirtual.read = sfvirt::sf_vio_read1;
			sfvirtual.write = sfvirt::sf_vio_write1;
			sfvirtual.tell = sfvirt::sf_vio_tell1;
			SF_INFO info;
			info.format = SF_FORMAT_WAV;
			SNDFILE* music = sf_open_virtual(&sfvirtual, SFM_READ, &info, sound);
			int samples = (info.channels * info.frames);
			float* sampleData = (float*)sampleStack.GetMemoryAligned(samples * sizeof(float), sizeof(float));
			if (sampleData == nullptr)
			{
				sf_close(music);
				ProfileReturn(-1);
			}
			sf_read_float(music, sampleData, samples);
			sf_close(music);

			AudioSample tempAS;
			tempAS.info = info;
			tempAS.samples = sampleData;

			soundSample.push_back(tempAS);
			ProfileReturn(soundSample.size() - 1);
		}

		void * AudioSound::GetSample(int soundID, SoundIndexName soundType)
		{
			StartProfile;
			if (soundType == BakgroundSound)
			{
				AudioOut *outData = new AudioOut();
				outData->sample = &soundSample[soundID];
				outData->pData.currentPos = 0;
				outData->pData.volume = (masterVol * bakgroundVol) / 10000;
				ProfileReturn((void*)outData);
			}
			else if (soundType == Effect)
			{
				AudioOut *outData = new AudioOut();
				outData->sample = &soundSample[soundID];
				outData->pData.currentPos = 0;
				outData->pData.volume = (masterVol * effectVol) / 10000;
				ProfileReturn((void*)outData);
			}
			ProfileReturnConst(nullptr);
		}

		void AudioSound::Shutdown()
		{
			StartProfile;
			sampleStack.ClearStackAlloc();
			soundSample.clear();
			StopProfile;
		}

		void AudioSound::OptionUpdate()
		{
			StartProfile;
			auto& optHandler = Core::Engine::GetInstance().GetOptionHandler();
			masterVol = optHandler.GetOption("Audio", "masterVolume", 100);
			effectVol = optHandler.GetOption("Audio", "effectVolume", 80);
			bakgroundVol = optHandler.GetOption("Audio", "bakgroundVolume", 50);
			ProfileReturnVoid;
		}

	}	//namespace Audio
}	//namespace SE