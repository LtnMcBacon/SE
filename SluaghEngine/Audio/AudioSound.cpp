#include "AudioSound.h"
#include <Profiler.h>
#include <fstream>
#include <Utilz\Memory.h>
namespace sfvirt {
			sf_count_t sf_vio_get_filelen1(void *user_data)
			{
				SE::Audio::AudioFile *data = static_cast<SE::Audio::AudioFile*>(user_data);
				return (sf_count_t)data->size;
			};

			sf_count_t sf_vio_seek1(sf_count_t offset, int whence, void *user_data)
			{
				SE::Audio::AudioFile *data = static_cast<SE::Audio::AudioFile*>(user_data);
				if (whence == SEEK_CUR)
				{
					if (data->currentPos + offset <= data->size && data->currentPos + offset >= 0)
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
				SE::Audio::AudioFile *data = static_cast<SE::Audio::AudioFile*>(user_data);
				if (data->currentPos + count <= data->size)
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
				SE::Audio::AudioFile *data = static_cast<SE::Audio::AudioFile*>(user_data);
				return data->currentPos;
			};
		}


namespace SE {
	namespace Audio {
		AudioSound::AudioSound()
		{
			masterVol = 0;
			effectVol = 0;
			bakgroundVol = 0;
		}

		AudioSound::~AudioSound()
		{
			
		}

		int AudioSound::Initialize()
		{
			StartProfile;
			using namespace Utilz::Memory;
			sampleStack.InitStackAlloc(20_mb);
			ProfileReturnConst(0);
		}

		AudioFile* ReadRaw()
		{
			AudioFile *sound = new AudioFile;
			std::streampos size;
			char * memblock;
			std::ifstream myfile;
			myfile.open("Assets/Sound/Canary.wav", std::ios::in | std::ios::binary | std::ios::ate);
			if (myfile.is_open())
			{
				size = myfile.tellg();
				memblock = new char[size];
				myfile.seekg(0, std::ios::beg);
				myfile.read(memblock, size);
				myfile.close();
				sound->size = size;
				sound->soundData = memblock;
				sound->currentPos = 0;
			}
			return sound;
		}

		size_t AudioSound::LoadSound(AudioFile* sound)
		{
			StartProfile;
			SF_VIRTUAL_IO sfvirtual;
			sfvirtual.get_filelen = sfvirt::sf_vio_get_filelen1;
			sfvirtual.seek = sfvirt::sf_vio_seek1;
			sfvirtual.read = sfvirt::sf_vio_read1;
			sfvirtual.write = sfvirt::sf_vio_write1;
			sfvirtual.tell = sfvirt::sf_vio_tell1;
			SF_INFO info;
			//info.format = SF_FORMAT_WAV;
			SNDFILE* music = sf_open_virtual(&sfvirtual, SFM_READ, &info, sound);
			//SNDFILE* music2 = sf_open("Assets/Sounds/Canary.wav", SFM_READ, &info);
			/*if (sizeof(music) != sizeof(music2))
			{
				int hej = 0;
			}*/
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
			delete sound;
			ProfileReturn(soundSample.size() - 1);
		}

		size_t AudioSound::LoadSound2()
		{
			StartProfile;
			SF_INFO info;
			SNDFILE* music = sf_open("Assets/Sounds/Canary.wav", SFM_READ, &info);
			int samples = (info.channels * info.frames);
			float* sampleData = new float[samples];
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
			else if (soundType == EffectSound)
			{
				AudioOut *outData = new AudioOut();
				outData->sample = &soundSample[soundID];
				outData->pData.currentPos = 0;
				outData->pData.volume = (masterVol * effectVol) / 10000;
				ProfileReturn((void*)outData);
			}
			ProfileReturnConst(nullptr);
		}

		void AudioSound::SetSoundVol(SE::Audio::SoundVolType volType, size_t newVol)
		{
			switch (volType)
			{
				case MasterVol:
				{
					masterVol = newVol;
				}
				case BakgroundVol:
				{
					bakgroundVol = newVol;
				}
				case EffectVol:
				{
					effectVol = newVol;
				}
			}
		}

		void AudioSound::Shutdown()
		{
			StartProfile;
			sampleStack.ClearStackAlloc();
			soundSample.clear();
			StopProfile;
		}

	}	//namespace Audio
}	//namespace SE