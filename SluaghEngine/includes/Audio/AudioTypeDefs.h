
#ifndef SE_AUDIO_AUDIO_TYPEDEF_H
#define SE_AUDIO_AUDIO_TYPEDEF_H
#include <DirectXMath.h>

namespace SE {
	namespace Audio {

		enum SoundIndexName { BakgroundSound, BakgroundLoopSound, StereoPanSound, StereoPanLoopSound, VoiceSound, StereoVoiceSound };
		enum SoundVolType { MasterVol, BakgroundVol, EffectVol, VoiceVol };

		struct PanData
		{
			DirectX::XMFLOAT3 soundPos;
			DirectX::XMFLOAT3 headPos;
			DirectX::XMFLOAT3 hearingVec;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSTRUCTTYPE_H