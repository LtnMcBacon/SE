#include "AudioStream.h"

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
			PaError err;
			err = Pa_Initialize();
			if (err != paNoError)
			{
				return -1;
			}
			return 0;
		}

		int AudioStream::StreamSound(SoundIndexName soundType, void* streamData)
		{
			PaStream *tempStream;
			PaError err;

			if (soundType == BakgroundSound)
			{
				AudioOut *inSample = (AudioOut*)streamData;
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
				AudioOut *inSample = (AudioOut*)streamData;
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
			if (err != paNoError)
			{
				return -1;
			}
			else
			{
				stream.push_back(tempStream);
				err = Pa_StartStream(stream[stream.size() - 1]);
				if (err != paNoError)
				{
					return -1;
				}
				return stream.size() - 1;
			}
				
			return -1;
		}

		void AudioStream::Shutdown()
		{
			PaError err;
			err = Pa_Terminate();
		}

	}	//namespace Audio
}	//namespace SE