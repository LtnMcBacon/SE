#include "AudioHandler.h"
#include <Profiler.h>

namespace SE {
	namespace Audio {
		AudioHandler::AudioHandler()
		{
			
		}

		AudioHandler::~AudioHandler()
		{
			
		}

		int AudioHandler::Initialize()
		{
			StartProfile;
			audioSound.Initialize();
			if (audioStream.Initialize() == -1)
			{
				ProfileReturnConst(-1);
			}
			ProfileReturnConst(0);
		}

		int AudioHandler::LoadSound(void* data, size_t size)
		{
			StartProfile;
			Audio::AudioFile *sound = new Audio::AudioFile;
			sound->size = size;
			sound->soundData = (char*)data;
			sound->currentPos = 0;
			/*delete sound;
		ProfileReturn(audioSound.LoadSound2());*/
		ProfileReturn(audioSound.LoadSound(sound));
		}
		
		int AudioHandler::CreateStream(int soundID, Audio::SoundIndexName soundType)
		{
			StartProfile;
			ProfileReturn(audioStream.CreateStream(soundType, (Audio::AudioOut*)audioSound.GetSample(soundID, soundType)));
		}

		int AudioHandler::StreamSound(int streamID)
		{
			StartProfile;
			ProfileReturn(audioStream.StreamSound(streamID));
		}

		int AudioHandler::StopSound(int streamID)
		{
			StartProfile;
			ProfileReturn(audioStream.StopSound(streamID));
		}

		int AudioHandler::RemoveSound(int streamID)
		{
			StartProfile;
			ProfileReturn(audioStream.RemoveSound(streamID));
		}

		void AudioHandler::SetSoundVol(SE::Audio::SoundVolType volType, size_t newVol)
		{
			audioSound.SetSoundVol(volType, newVol);
		}

		void AudioHandler::Shutdown()
		{		
			StartProfile;
			audioStream.Shutdown();
			audioSound.Shutdown();
			ProfileReturnVoid;
		}


	}	//namespace Core
}	//namespace SE