#pragma once
#ifndef SE_AUDIO_AUDIOSTRUCTTYPE_H
#define SE_AUDIO_AUDIOSTRUCTTYPE_H
#include <portaudio\portaudio.h>
#include <sndfile\sndfile.h>
#include <vector>
#include <stack>


#ifdef _DEBUG
#pragma comment (lib, "libsndfile-1.lib")
#else
#pragma comment (lib, "libsndfile-1.lib")
#endif

#ifdef _M_IX86 
#pragma comment (lib, "portaudio_x86.lib")
#else
#pragma comment (lib, "portaudio_x64.lib")
#endif

namespace SE {
	namespace Audio {

		//only for loading sound
		struct AudioFile	
		{
			char* soundData;
			size_t size;
			int currentPos;
		};

		struct AudioSample
		{
			SF_INFO info;
			float* samples;
		};

		struct AudioPrivateData
		{
			size_t currentPos;
			float volume;
		};

		struct AudioOut
		{
			AudioSample *sample;
			AudioPrivateData pData;
		};

		static int bakgroundCallback(const void *inputBuffer,
			void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData)
		{
			/* Cast data passed through stream to our structure. */
			AudioOut *data = static_cast<AudioOut*>(userData);
			float *out = (float*)outputBuffer;

			unsigned int i;
			(void)inputBuffer; /* Prevent unused variable warning. */

			if ((framesPerBuffer * data->pData.currentPos * data->sample->info.channels) + framesPerBuffer * data->sample->info.channels <= data->sample->info.frames * data->sample->info.channels)
			{
				for (i = 0; i<framesPerBuffer; i++)
				{
					for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->pData.currentPos)]) * data->pData.volume;
					}
				}
				data->pData.currentPos++;
				return 0;
			}
			else
			{
				memset(out, 0, framesPerBuffer * data->sample->info.channels);
				for (i = 0; i < (framesPerBuffer * (data->pData.currentPos + 1) * data->sample->info.channels) - (data->sample->info.frames * data->sample->info.channels); i++)
				{
					for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->pData.currentPos)])* data->pData.volume;
					}
				}
				data->pData.currentPos = 0;
				return 0;
			}
			return 1;
		};

		static int effectCallback(const void *inputBuffer,
			void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData)
		{
			/* Cast data passed through stream to our structure. */
			AudioOut *_data = static_cast<AudioOut*>(userData);
			float *_out = (float*)outputBuffer;

			unsigned int _i;
			(void)inputBuffer; /* Prevent unused variable warning. */

			if (framesPerBuffer * _data->pData.currentPos >= _data->sample->info.frames / 2)
				return paComplete;

			for (_i = 0; _i<framesPerBuffer; _i++)
			{
				if (_data->pData.currentPos * framesPerBuffer + _i < _data->sample->info.frames)
				{
					for (int _AmountOfChannels = 0; _AmountOfChannels < _data->sample->info.channels; _AmountOfChannels++)
					{
						*_out++ = (_data->sample->samples[_i * _data->sample->info.channels + _AmountOfChannels + (framesPerBuffer * _data->sample->info.channels * _data->pData.currentPos)]) * _data->pData.volume;
					}
				}
				else
				{
					for (int _AmountOfChannels = 0; _AmountOfChannels < _data->sample->info.channels; _AmountOfChannels++)
					{
						*_out++ = 0.0f;
					}
				}
			}
			_data->pData.currentPos++;
			return paContinue;

			/*if (framesPerBuffer * _data->pData.currentPos < _data->sample->info.frames - framesPerBuffer)
			{
				for (_i = 0; _i<framesPerBuffer; _i++)
				{
					for (int _AmountOfChannels = 0; _AmountOfChannels < _data->sample->info.channels; _AmountOfChannels++)
					{
						*_out++ = (_data->sample->samples[_i * _data->sample->info.channels + _AmountOfChannels + (framesPerBuffer * _data->sample->info.channels * _data->pData.currentPos)]) * _data->pData.volume;
					}
				}
				_data->pData.currentPos++;
				return paContinue;
			}
			else
			{
				memset(_out, 0, framesPerBuffer * _data->sample->info.channels);
				for (_i = 0; _i < (framesPerBuffer * _data->pData.currentPos * _data->sample->info.channels) + framesPerBuffer * _data->sample->info.channels - (_data->sample->info.frames * _data->sample->info.channels) - 1; _i++)
				{
					for (int _AmountOfChannels = 0; _AmountOfChannels < _data->sample->info.channels; _AmountOfChannels++)
					{
						*_out++ = (_data->sample->samples[_i * _data->sample->info.channels + _AmountOfChannels + (framesPerBuffer * _data->sample->info.channels * _data->pData.currentPos)]) * _data->pData.volume;
					}
				}
				return paComplete;
			}
			return paComplete;*/
		};

	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSTRUCTTYPE_H