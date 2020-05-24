//
//  tdav_codec_audio_mixer.hpp
//  YouMeIM
//
//  Created by pinky on 2017/7/31.
//
//

#ifndef tdav_codec_audio_mixer_hpp
#define tdav_codec_audio_mixer_hpp

#include <stdio.h>
#include "AGCPlatform.h"

namespace youmecommon
{
	//振幅拉伸fGain倍
	void tdav_codec_apply_volume_add(float fGain, void* pBuf, int nBytes, const uint8_t nBytesPerSample);

	//计算音段音频的最大振幅
	void tdav_codec_swing_max(void* pBuf, int nBytes, const uint8_t nBytesPerSample, int&  max_swing);

	//根据当前的最大振幅，计算拉到最大音量需要的振幅拉升倍数
	float tdav_getVolumeGain(int max_swing);
}

#endif /* tdav_codec_audio_mixer_hpp */
