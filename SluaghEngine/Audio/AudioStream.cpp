#include "AudioStream.h"
#include <fstream>
#include <iostream>
#include <Profiler.h>

namespace SE {
	namespace Audio {
		AudioStream::AudioStream()
		{

		}

		AudioStream::~AudioStream()
		{
			
		}

		int AudioStream::Initialize()
		{
			StartProfile;
			PaError err;
			err = Pa_Initialize();
			if (err != paNoError)
				ProfileReturnConst(-1);
			ProfileReturnConst(0);
		}

		int AudioStream::CreateStream(SoundIndexName soundType, void* streamData)
		{
			StartProfile;
			PaStream *tempStream = nullptr;
			PaError err;
			AudioOut *inSample = (AudioOut*)streamData;
			
			if (Pa_GetDeviceCount() > 0)
			{
				if (soundType == BakgroundSound)
				{
					/* Open an audio I/O stream. */
					err = Pa_OpenDefaultStream(&tempStream,
						0,          /* no input channels */
						inSample->sample->info.channels,          /* stereo output */
						paFloat32,  /* 32 bit floating point output */
						inSample->sample->info.samplerate,
						256,        /* frames per buffer, i.e. the number
									of sample frames that PortAudio will
									request from the callback. Many apps
									may want to use
									paFramesPerBufferUnspecified, which
									tells PortAudio to pick the best,
									possibly changing, buffer size.*/
						bakgroundCallback, /* this is your callback function */
						inSample); /*This is a pointer that will be passed to
													 your callback*/
				}
				else if (soundType == EffectSound)
				{
					/* Open an audio I/O stream. */
					err = Pa_OpenDefaultStream(&tempStream,
						0,          /* no input channels */
						inSample->sample->info.channels,          /* stereo output */
						paFloat32,  /* 32 bit floating point output */
						inSample->sample->info.samplerate,
						256,        /* frames per buffer, i.e. the number
									of sample frames that PortAudio will
									request from the callback. Many apps
									may want to use
									paFramesPerBufferUnspecified, which
									tells PortAudio to pick the best,
									possibly changing, buffer size.*/
						effectCallback, /* this is your callback function */
						inSample); /*This is a pointer that will be passed to
													 your callback*/
				}
				if (err != paNoError)
				{
					ProfileReturnConst(-1);
				}
				else
				{
					if (freeStreamID.size() == 0)
					{
						stream.push_back(tempStream);
						sampleOut.push_back(inSample);
						ProfileReturn(stream.size() - 1);
					}
					else
					{
						stream[freeStreamID.top()] = tempStream;
						sampleOut[freeStreamID.top()] = inSample;
						freeStreamID.pop();
						ProfileReturn(stream.size() - 1);
					}
				}
			}
			delete inSample;
			ProfileReturnConst(-2);
		}
	
		int AudioStream::StreamSound(int streamID)
		{
			StartProfile;
			PaError err;
			if (Pa_GetDeviceCount() > 0)
			{
				err = Pa_StartStream(stream[streamID]);
				if (err != paNoError)
					ProfileReturnConst(-1);
				ProfileReturnConst(0);
			}
			ProfileReturnConst(-2);
		}

		int AudioStream::StopSound(int streamID)
		{
			StartProfile;
			PaError err;
			err = Pa_StopStream(stream[streamID]);
			if (err != paNoError)
				ProfileReturnConst(-1);
			sampleOut[streamID]->pData.currentPos = 0;
			ProfileReturnConst(0);
		}

		int AudioStream::RemoveSound(int streamID)
		{
			StartProfile;
			PaError err;
			err = Pa_CloseStream(stream[streamID]);
			if (err != paNoError)
				ProfileReturnConst(-1);
			stream[streamID] = nullptr;
			delete sampleOut[streamID];
			sampleOut[streamID] = nullptr;
			freeStreamID.push(streamID);
			ProfileReturnConst(0);
		}

		void AudioStream::Shutdown()
		{
			StartProfile;
			for (auto &st : stream)
			{
				if (st)
				{
					Pa_CloseStream(st);
				}
			}
			stream.clear();
			for (auto &so : sampleOut)
			{
				if (so)
				{
					delete so;
				}
			}
			while (freeStreamID.size() > 0)
			{
				freeStreamID.pop();
			}
			sampleOut.clear();
			Pa_Terminate();
			StopProfile;
		}
	}	//namespace Audio
}	//namespace SE