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
			return 0;
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
			float* sampleData = new float[samples];
			if (sampleData == nullptr)
			{
				sf_close(music);
				ProfileReturn(-1);
			}
			sf_read_float(music, sampleData, samples);
			sf_close(music);

			AudioSample *tempAS = new AudioSample;
			tempAS->info = info;
			tempAS->samples = sampleData;

			if (freeSample.size() == 0)
			{
				soundSample.push_back(tempAS);
				delete sound;
				ProfileReturn(soundSample.size() - 1);
			}
			else 
			{
				int temp = freeSample.top();
				freeSample.pop();
				soundSample[temp] = tempAS;			
				delete sound;
				ProfileReturn(temp);
			}
		}

		int AudioSound::UnloadSound(int soundHandle)
		{
			StartProfile;
			if (soundSample.size() > soundHandle)
			{
				delete soundSample[soundHandle]->samples;
				delete soundSample[soundHandle];
				soundSample[soundHandle] = nullptr;
				freeSample.push(soundHandle);
				ProfileReturn(0);
			}
			ProfileReturn(-1);
		}

		void * AudioSound::GetSample(int soundID, SoundIndexName soundType)
		{
			StartProfile;
			if (soundType == BakgroundLoopSound || soundType == BakgroundSound)
			{
				AudioOut *outData = new AudioOut();
				outData->sample = soundSample[soundID];
				outData->audioPrivateData.currentPos = 0;
				outData->audioPrivateData.volume = &bakgroundVol;
				outData->panData.headPos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				outData->panData.soundPos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				outData->panData.hearingVec = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				ProfileReturn((void*)outData);
			}
			else if (soundType == StereoPanSound || soundType == StereoPanLoopSound)
			{
				AudioOut *outData = new AudioOut();
				outData->sample = soundSample[soundID];
				outData->audioPrivateData.currentPos = 0;
				outData->audioPrivateData.volume = &effectVol;
				outData->panData.headPos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				outData->panData.soundPos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				outData->panData.hearingVec = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				ProfileReturn((void*)outData);
			}
			else if (soundType == VoiceSound)
			{
				AudioOut *outData = new AudioOut();
				outData->sample = soundSample[soundID];
				outData->audioPrivateData.currentPos = 0;
				outData->audioPrivateData.volume = &voiceVol;
				outData->panData.headPos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				outData->panData.soundPos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				outData->panData.hearingVec = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				ProfileReturn((void*)outData);
			}
			else if (soundType == StereoVoiceSound)
			{
				AudioOut *outData = new AudioOut();
				outData->sample = soundSample[soundID];
				outData->audioPrivateData.currentPos = 0;
				outData->audioPrivateData.volume = &voiceSVol;
				outData->panData.headPos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				outData->panData.soundPos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
				outData->panData.hearingVec = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
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
					if (masterVol != 0)
					{
						bakgroundVol = bakgroundVol * (10000 / masterVol);
						effectVol = effectVol * (10000 / masterVol);
						voiceVol = voiceVol * (10000 / masterVol);
					}
					masterVol = newVol;
					bakgroundVol = (bakgroundVol * newVol) / 10000;
					effectVol = (effectVol * newVol) / 10000;
					voiceVol = (voiceVol * newVol) / 10000;
					voiceSVol = voiceVol * 3.0;
					break;
				}
				case BakgroundVol:
				{
					bakgroundVol = (masterVol * newVol) / 10000;
					break;
				}
				case EffectVol:
				{
					effectVol = (masterVol * newVol) / 10000;
					break;
				}
				case VoiceVol:
				{
					voiceVol = (masterVol * newVol) / 10000;
					voiceSVol = voiceVol * 3.0;
					break;
				}
			}
		}

		void AudioSound::Shutdown()
		{
			StartProfile;
			for (auto& sound : soundSample)
			{
				if (sound != nullptr)
				{
					delete sound->samples;
					delete sound;
				}
			}
			soundSample.clear();
			StopProfile;
		}

	}	//namespace Audio
}	//namespace SE