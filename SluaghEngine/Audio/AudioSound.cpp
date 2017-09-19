#include "AudioSound.h"


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
			return 0;
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

		int AudioSound::LoadSound(void * inputData, size_t inputSize)
		{
			SF_VIRTUAL_IO sfvirtual;
			sfvirtual.get_filelen = sfvirt::sf_vio_get_filelen1;
			sfvirtual.seek = sfvirt::sf_vio_seek1;
			sfvirtual.read = sfvirt::sf_vio_read1;
			sfvirtual.write = sfvirt::sf_vio_write1;
			sfvirtual.tell = sfvirt::sf_vio_tell1;
			AudioFile TestSound;
			TestSound.soundData = (char*)inputData;
			TestSound.size = inputSize;
			TestSound.currentPos = 0;
			SF_INFO _info;
			_info.format = SF_FORMAT_WAV;
			SNDFILE* _music = sf_open_virtual(&sfvirtual, SFM_READ, &_info, &TestSound);
			int _samples = (_info.channels * _info.frames);
			float* _sampleData = new float[_samples];
			sf_read_float(_music, _sampleData, _samples);
			sf_close(_music);

			AudioSample _tempAS;
			_tempAS.info = _info;
			_tempAS.samples = _sampleData;

			soundSample.push_back(_tempAS);
			return soundSample.size() - 1;
		}

		void * AudioSound::GetSample(int soundID, SoundIndexName soundType)
		{
			if (soundType == BakgroundSound)
			{
				AudioOut *outData = new AudioOut();
				outData->sample = &soundSample[soundID];
				outData->pData.currentPos = 0;
				outData->pData.volume = 1.0;
				return (void*)outData;
			}
			return nullptr;
		}

		void AudioSound::Shutdown()
		{

		}

	}	//namespace Audio
}	//namespace SE