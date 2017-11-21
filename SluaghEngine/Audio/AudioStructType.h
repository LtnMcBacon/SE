#pragma once
#ifndef SE_AUDIO_AUDIOSTRUCTTYPE_H
#define SE_AUDIO_AUDIOSTRUCTTYPE_H
#include <portaudio\portaudio.h>
#include <sndfile\sndfile.h>
#include <vector>
#include <stack>
#include "AudioTypeDefs.h"


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
			float* volume;
		};

		struct AudioOut
		{
			AudioSample *sample;
			AudioPrivateData audioPrivateData;
			PanData panData;
		};

		static int bakgroundLoopCallback(const void *inputBuffer,
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

			if (framesPerBuffer * data->audioPrivateData.currentPos < data->sample->info.frames - framesPerBuffer)
			{
				for (i = 0; i<framesPerBuffer; i++)
				{
					for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * *data->audioPrivateData.volume;
					}
				}
				data->audioPrivateData.currentPos++;
				return paContinue;
			}
			else
			{
				memset(out, 0, framesPerBuffer * data->sample->info.channels);
				for (i = 0; i < (data->sample->info.frames - framesPerBuffer * data->audioPrivateData.currentPos); i++)
				{
					for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * *data->audioPrivateData.volume;
					}
				}
				data->audioPrivateData.currentPos = 0;
				return paContinue;
			}
			return paComplete;
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

			if (framesPerBuffer * data->audioPrivateData.currentPos < data->sample->info.frames - framesPerBuffer)
			{
				for (i = 0; i<framesPerBuffer; i++)
				{
					for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * *data->audioPrivateData.volume;
					}
				}
				data->audioPrivateData.currentPos++;
				return paContinue;
			}
			else
			{
				memset(out, 0, framesPerBuffer * data->sample->info.channels);
				for (i = 0; i < (data->sample->info.frames - framesPerBuffer * data->audioPrivateData.currentPos); i++)
				{
					for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * *data->audioPrivateData.volume;
					}
				}
				return paComplete;
			}
			return paComplete;
		};

		static int SteroePanCallback(const void *inputBuffer,
			void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData)
		{
			/* Cast data passed through stream to our structure. */
			AudioOut *data = static_cast<AudioOut*>(userData);
			float *out = (float*)outputBuffer;
			float volBasedOfDist[2];

			float dist = sqrt((data->panData.headPos.x - data->panData.soundPos.x) * (data->panData.headPos.x - data->panData.soundPos.x) + (data->panData.headPos.y - data->panData.soundPos.y) * (data->panData.headPos.y - data->panData.soundPos.y) + (data->panData.headPos.z - data->panData.soundPos.z) * (data->panData.headPos.z - data->panData.soundPos.z));

			DirectX::XMFLOAT3 soundVec = DirectX::XMFLOAT3((data->panData.soundPos.x - data->panData.headPos.x) / dist, (data->panData.soundPos.y - data->panData.headPos.y) / dist, (data->panData.soundPos.z - data->panData.headPos.z) / dist);
			//dot product of Sound and head left and right
			float dotSHLeft = 1.0f + (data->panData.hearingVec.x * soundVec.x) + (data->panData.hearingVec.y * soundVec.y) + (data->panData.hearingVec.z * soundVec.z);
			float dotSHRight = 1.0f + ((-data->panData.hearingVec.x) * soundVec.x) + ((-data->panData.hearingVec.y) * soundVec.y) + ((-data->panData.hearingVec.z) * soundVec.z);
			if (dist >= 1)
			{
				volBasedOfDist[0] = (1.0f / dist) * (dotSHLeft / 2.0f) * *data->audioPrivateData.volume;
				volBasedOfDist[1] = (1.0f / dist) * (dotSHRight / 2.0f) * *data->audioPrivateData.volume;
			}
			else
			{
				volBasedOfDist[0] = (dotSHLeft / 2.0f) * *data->audioPrivateData.volume;
				volBasedOfDist[1] = (dotSHRight / 2.0f) * *data->audioPrivateData.volume;
			}



			unsigned int i;
			(void)inputBuffer; /* Prevent unused variable warning. */

			if (framesPerBuffer * data->audioPrivateData.currentPos < data->sample->info.frames - framesPerBuffer)
			{
				if (data->sample->info.channels == 2)
				{
					for (i = 0; i < framesPerBuffer; i++)
					{
						for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
						{
							*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * volBasedOfDist[AmountOfChannels];
						}
					}
				}
				else 
				{
					for (i = 0; i < framesPerBuffer; i++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * volBasedOfDist[0];
						*out++ = (data->sample->samples[i * data->sample->info.channels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * volBasedOfDist[1];
					}
				}
				data->audioPrivateData.currentPos++;
				return paContinue;
			}
			else
			{
				memset(out, 0, framesPerBuffer * data->sample->info.channels);
				for (i = 0; i < (data->sample->info.frames - framesPerBuffer * data->audioPrivateData.currentPos); i++)
				{
					for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * volBasedOfDist[AmountOfChannels];
					}
				}
				return paComplete;
			}
			return paComplete;
		};

		static int SteroePanLoopCallback(const void *inputBuffer,
			void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData)
		{
			/* Cast data passed through stream to our structure. */
			AudioOut *data = static_cast<AudioOut*>(userData);
			float *out = (float*)outputBuffer;
			float volBasedOfDist[2];

			float dist = sqrt((data->panData.headPos.x - data->panData.soundPos.x) * (data->panData.headPos.x - data->panData.soundPos.x) + (data->panData.headPos.y - data->panData.soundPos.y) * (data->panData.headPos.y - data->panData.soundPos.y) + (data->panData.headPos.z - data->panData.soundPos.z) * (data->panData.headPos.z - data->panData.soundPos.z));

			DirectX::XMFLOAT3 soundVec = DirectX::XMFLOAT3((data->panData.soundPos.x - data->panData.headPos.x) / dist, (data->panData.soundPos.y - data->panData.headPos.y) / dist, (data->panData.soundPos.z - data->panData.headPos.z) / dist);
			//dot product of Sound and head left and right
			float dotSHLeft =  1.0f + (data->panData.hearingVec.x * soundVec.x) + (data->panData.hearingVec.y * soundVec.y) + (data->panData.hearingVec.z * soundVec.z);
			float dotSHRight = 1.0f + ((-data->panData.hearingVec.x) * soundVec.x) + ((-data->panData.hearingVec.y) * soundVec.y) + ((-data->panData.hearingVec.z) * soundVec.z);
			if (dist >= 1)
			{
				volBasedOfDist[0] = (1.0f / dist) * (dotSHLeft / 2.0f) * *data->audioPrivateData.volume;
				volBasedOfDist[1] = (1.0f / dist) * (dotSHRight / 2.0f) * *data->audioPrivateData.volume;
			}
			else
			{
				volBasedOfDist[0] = (dotSHLeft / 2.0f) * *data->audioPrivateData.volume;
				volBasedOfDist[1] = (dotSHRight / 2.0f) * *data->audioPrivateData.volume;
			}



			unsigned int i;
			(void)inputBuffer; /* Prevent unused variable warning. */

			if (framesPerBuffer * data->audioPrivateData.currentPos < data->sample->info.frames - framesPerBuffer)
			{
				if (data->sample->info.channels == 2)
				{
					for (i = 0; i < framesPerBuffer; i++)
					{
						for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
						{
							*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * volBasedOfDist[AmountOfChannels];
						}
					}
				}
				else
				{
					for (i = 0; i < framesPerBuffer; i++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * volBasedOfDist[0];
						*out++ = (data->sample->samples[i * data->sample->info.channels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * volBasedOfDist[1];
					}
				}
				data->audioPrivateData.currentPos++;
				return paContinue;
			}
			else
			{
				memset(out, 0, framesPerBuffer * data->sample->info.channels);
				for (i = 0; i < (data->sample->info.frames - framesPerBuffer * data->audioPrivateData.currentPos); i++)
				{
					for (int AmountOfChannels = 0; AmountOfChannels < data->sample->info.channels; AmountOfChannels++)
					{
						*out++ = (data->sample->samples[i * data->sample->info.channels + AmountOfChannels + (framesPerBuffer * data->sample->info.channels * data->audioPrivateData.currentPos)]) * volBasedOfDist[AmountOfChannels];
					}
				}
				data->audioPrivateData.currentPos = 0;
				return paContinue;
			}
			return paComplete;
		};

	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSTRUCTTYPE_H