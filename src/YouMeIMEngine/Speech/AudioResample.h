#ifndef AUDIO_RESAMPLE_H
#define AUDIO_RESAMPLE_H


#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeCommon/wavresample/speex_resampler.h>
#include <YouMeCommon/XSharedArray.h>
#include "YouMeCommon/XFile.h"


class AudioResample
{
public:
	AudioResample();
	~AudioResample();

	int InitResample(int channels, int inSampleRate, int outSampleRate);
	int DestroyResample();
	int ResampleFile(int channels, int sampleBitSize, int inSampleRate, const XString& srcPath, int outSampleRate, const XString& destPath);
	int ResampleAudioData(short* inBuffer, unsigned int inSize, short* outBuffer);

private:
	int ResampleAudioData(short* inBuffer, unsigned int inSize, youmecommon::CXSharedArray<short>& outBuffer);
	int ReadPCMFrame(short buffer[], youmecommon::CXFile& file, int channels, int sampleRate, int sampleBitSize);

	youmecommon::SpeexResamplerState* m_pSampleState;
};

#endif
