//
//  tdav_codec_audio_mixer.cpp
//  YouMeIM
//
//  Created by pinky on 2017/7/31.
//
//

#include "tdav_codec_audio_mixer.h"

namespace youmecommon
{

	void tdav_codec_swing_max(void* pBuf, int nBytes, const uint8_t nBytesPerSample, int&  max_swing)
	{
		if (2 == nBytesPerSample) {
			uint8_t *pData = (uint8_t*)pBuf;
			int i = 0;
			for (i = 0; i < nBytes - 1; i += 2) {
				short oldValue = 0;

				oldValue |= ((pData[i]) & 0xFF);
				oldValue |= ((pData[i + 1] << 8) & 0xFF00);
				if (oldValue > max_swing) {
					max_swing = oldValue;
				}
				else if (oldValue < -max_swing){
					max_swing = -oldValue;
				}
			}
		}
		else if (1 == nBytesPerSample) {
			int8_t *pData = (int8_t*)pBuf;
			int i = 0;
			for (i = 0; i < nBytes; i++) {
				short oldValue = pData[i];
				if (oldValue > max_swing) {
					max_swing = oldValue;
				}
				else if (oldValue < -max_swing){
					max_swing = -oldValue;
				}
			}
		}
	}

	float tdav_getVolumeGain(int max_swing)
	{
		int pcmMax = (1 << 15) - 1;
		float rate = float(pcmMax) / max_swing;
		return rate;
	}

	void tdav_codec_apply_volume_add(float fGain, void* pBuf, int nBytes, const uint8_t nBytesPerSample)
	{
		if ((fGain < 0.0f) || (1.0f == fGain)) {
			return;
		}

		if (2 == nBytesPerSample) {
			uint8_t *pData = (uint8_t*)pBuf;
			int pcmMin = -(1 << 15);
			int pcmMax = (1 << 15) - 1;
			int i = 0;
			for (i = 0; i < nBytes - 1; i += 2) {
				short oldValue = 0;
				short newValue;
				int   tempValue;

				oldValue |= ((pData[i]) & 0xFF);
				oldValue |= ((pData[i + 1] << 8) & 0xFF00);
				if (oldValue >= 0) {
					tempValue = (int)(oldValue * fGain + 0.5f);
				}
				else {
					tempValue = (int)(oldValue * fGain - 0.5f);
				}

				if (tempValue > pcmMax) {
					newValue = (short)pcmMax;
				}
				else if (tempValue < pcmMin) {
					newValue = (short)pcmMin;
				}
				else {
					newValue = (short)tempValue;
				}
				pData[i] = (uint8_t)(newValue & 0xFF);
				pData[i + 1] = (uint8_t)((newValue & 0xFF00) >> 8);
			}
		}
		else if (1 == nBytesPerSample) {
			int8_t *pData = (int8_t*)pBuf;
			int pcmMin = -(1 << 7);
			int pcmMax = (1 << 7) - 1;
			int i = 0;
			for (i = 0; i < nBytes; i++) {
				int tempValue;
				if (pData[i] >= 0) {
					tempValue = (int)(pData[i] * fGain + 0.5f);
				}
				else {
					tempValue = (int)(pData[i] * fGain - 0.5f);
				}

				if (tempValue > pcmMax) {
					pData[i] = (int8_t)pcmMax;
				}
				else if (tempValue < pcmMin) {
					pData[i] = (int8_t)pcmMin;
				}
				else {
					pData[i] = (int8_t)tempValue;
				}
			}
		}
	}

}