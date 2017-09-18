#pragma once
#ifndef SE_AUDIO_AUDIOSTREAM_H
#define SE_AUDIO_AUDIOSTREAM_H


namespace SE {
	namespace Audio {

		class AudioStream
		{
		public:
			AudioStream();
			~AudioStream();
			/**
			* @brief	Calls init functions in stream and sound
			*
			* @details	Calls and makes sure that both stream and sound gets initialized
			*
			* @retval 0 Tells that display creation was sucessful
			*
			*/
			int Initialise();
			void Shutdown();
		private:

		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSTREAM_H