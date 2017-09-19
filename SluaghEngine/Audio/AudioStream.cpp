#include "AudioStream.h"
#include <fstream>
#include <iostream>

namespace SE {
	namespace Audio {
		AudioStream::AudioStream()
		{

		}

		AudioStream::~AudioStream()
		{
			for (auto &st : stream)
			{
				if (st)
				{
					Pa_CloseStream(st);
				}
			}
			Pa_Terminate();
		}

		int AudioStream::Initialize()
		{
			PaError err;
			err = Pa_Initialize();
			if (err != paNoError)
				return -1;
			int hej = Pa_GetDeviceCount();
			return 0;
		}

		int AudioStream::CreateStream(SoundIndexName soundType, void* streamData)
		{
			PaStream *tempStream = nullptr;
			PaError err;
			AudioOut *inSample = (AudioOut*)streamData;

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
			else if (soundType == Effect)
			{
				/* Open an audio I/O stream. */
				std::cout << (float)*inSample->sample->samples << std::endl;
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
				return -1;
			}
			else
			{
				if (freeStreamID.size() == 0)
				{
					stream.push_back(tempStream);
					sampleOut.push_back(inSample);
					return stream.size() - 1;
				}
				else
				{
					stream[freeStreamID.top()] = tempStream;
					sampleOut[freeStreamID.top()] = inSample;
					freeStreamID.pop();
					return stream.size() - 1;
				}
			}
				
			return -1;
		}
	
		int AudioStream::PlaySound(int streamID)
		{
			PaError err;
			err = Pa_StartStream(stream[streamID]);
			if (err != paNoError)
				return -1;
			return 0;
		}

		int AudioStream::StopSound(int streamID)
		{
			PaError err;
			err = Pa_StopStream(stream[streamID]);
			if (err != paNoError)
				return -1;
			sampleOut[streamID]->pData.currentPos = 0;
			return 0;
		}

		int AudioStream::RemoveSound(int streamID)
		{
			PaError err;
			err = Pa_CloseStream(stream[streamID]);
			if (err != paNoError)
				return -1;
			delete sampleOut[streamID];
			freeStreamID.push(streamID);
			return 0;
		}
	}	//namespace Audio
}	//namespace SE