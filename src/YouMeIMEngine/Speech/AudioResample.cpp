#include "AudioResample.h"
#include <YouMeCommon/Log.h>
#include <YouMeIMEngine/Speech/SpeechUtil.h>
#include <YouMeIMEngine/Speech/VoiceInterface.h>


#define PCM_FRAME_SIZE_8K 160
#define PCM_FRAME_SIZE_16K 320
#define PCM_FRAME_SIZE_32K 640
#define PCM_FRAME_SIZE_44K 882
#define PCM_FRAME_SIZE_48K 960


AudioResample::AudioResample() : m_pSampleState(NULL)
{

}

AudioResample::~AudioResample()
{
	if (m_pSampleState != NULL)
	{
		youmecommon::speex_resampler_destroy(m_pSampleState);
		m_pSampleState = NULL;
	}
}

int AudioResample::InitResample(int channels, int inSampleRate, int outSampleRate)
{
	if (m_pSampleState != NULL)
	{
		youmecommon::speex_resampler_destroy(m_pSampleState);
	}

	m_pSampleState = youmecommon::speex_resampler_init(channels, inSampleRate, outSampleRate, 3, NULL);
	if (m_pSampleState == NULL)
	{
		return -1;
	}
	return 0;
}

int AudioResample::ResampleAudioData(short* inBuffer, unsigned int inSize, youmecommon::CXSharedArray<short>& outBuffer)
{
	unsigned int iOutSize = inSize;
	int errorcode = speex_resampler_process_int(m_pSampleState, 0, inBuffer, &inSize, (spx_int16_t*)outBuffer.Get(), &iOutSize);
	if (errorcode != 0)
	{
		return -1;
	}
	return iOutSize;
}

int AudioResample::ResampleAudioData(short* inBuffer, unsigned int inSize, short* outBuffer)
{
	unsigned int iOutSize = inSize;
	int errorcode = speex_resampler_process_int(m_pSampleState, 0, inBuffer, &inSize, outBuffer, &iOutSize);
	if (errorcode != 0)
	{
		return -1;
	}
	return iOutSize;
}

int AudioResample::DestroyResample()
{
	if (m_pSampleState != NULL)
	{
		youmecommon::speex_resampler_destroy(m_pSampleState);
		m_pSampleState = NULL;
	}
	return 0;
}

int AudioResample::ReadPCMFrame(short buffer[], youmecommon::CXFile& file, int channels, int sampleRate, int sampleBitSize)
{
	XINT64 nRead = 0;
	int frameSize = 0;
	if (sampleBitSize == 8 && channels == 1)
	{
		unsigned char* pcmFrame = NULL;
		if (SAMPLERATE_8K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_8K;
			static unsigned char pcmFrame_8k_8b_1[PCM_FRAME_SIZE_8K];
			pcmFrame = pcmFrame_8k_8b_1;
		}
		else if (SAMPLERATE_16K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_16K;
			static unsigned char pcmFrame_16k_8b_1[PCM_FRAME_SIZE_16K];
			pcmFrame = pcmFrame_16k_8b_1;
		}
		else if (SAMPLERATE_32K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_32K;
			static unsigned char pcmFrame_32k_8b_1[PCM_FRAME_SIZE_32K];
			pcmFrame = pcmFrame_32k_8b_1;
		}
		else if (SAMPLERATE_44K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_44K;
			static unsigned char pcmFrame_44k_8b_1[PCM_FRAME_SIZE_44K];
			pcmFrame = pcmFrame_44k_8b_1;
		}
		else if (SAMPLERATE_48K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_48K;
			static unsigned char pcmFrame_48k_8b_1[PCM_FRAME_SIZE_48K];
			pcmFrame = pcmFrame_48k_8b_1;
		}
		else
		{
			return -1;
		}

		nRead = file.Read(pcmFrame, frameSize * 2);
		for (int x = 0; x < frameSize; ++x)
		{
			buffer[x] = (short)((short)pcmFrame[x] << 7);
		}
	}
	else if (sampleBitSize == 8 && channels == 2)
	{
		unsigned char* pcmFrame = NULL;
		if (SAMPLERATE_8K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_8K;
			static unsigned char pcmFrame_8k_8b_2[PCM_FRAME_SIZE_8K << 1];
			pcmFrame = pcmFrame_8k_8b_2;
		}
		else if (SAMPLERATE_16K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_16K;
			static unsigned char pcmFrame_16k_8b_2[PCM_FRAME_SIZE_16K << 1];
			pcmFrame = pcmFrame_16k_8b_2;
		}
		else if (SAMPLERATE_32K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_32K;
			static unsigned char pcmFrame_32k_8b_2[PCM_FRAME_SIZE_32K << 1];
			pcmFrame = pcmFrame_32k_8b_2;
		}
		else if (SAMPLERATE_44K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_44K;
			static unsigned char pcmFrame_44k_8b_2[PCM_FRAME_SIZE_44K << 1];
			pcmFrame = pcmFrame_44k_8b_2;
		}
		else if (SAMPLERATE_48K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_48K;
			static unsigned char pcmFrame_48k_8b_2[PCM_FRAME_SIZE_48K << 1];
			pcmFrame = pcmFrame_48k_8b_2;
		}
		else
		{
			return -1;
		}

		nRead = file.Read(pcmFrame, frameSize * channels);
		for (int x = 0, y = 0; y < frameSize; ++y, x += 2)
		{
			// 1 - 取两个声道之左声道
			buffer[y] = (short)((short)pcmFrame[x + 0] << 7);
			// 2 - 取两个声道之右声道
			//speech[y] =(short)((short)pcmFrame_8b2[x+1] << 7);
			// 3 - 取两个声道的平均值
			//ush1 = (short)pcmFrame_8b2[x+0];
			//ush2 = (short)pcmFrame_8b2[x+1];
			//ush = (ush1 + ush2) >> 1;
			//speech[y] = (short)((short)ush << 7);
		}
	}
	else if (sampleBitSize == 16 && channels == 1)
	{
		unsigned short* pcmFrame = NULL;
		if (SAMPLERATE_8K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_8K;
			static unsigned short pcmFrame_8k_16b_1[PCM_FRAME_SIZE_8K << 1];
			pcmFrame = pcmFrame_8k_16b_1;
		}
		else if (SAMPLERATE_16K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_16K;
			static unsigned short pcmFrame_16k_16b_1[PCM_FRAME_SIZE_16K << 1];
			pcmFrame = pcmFrame_16k_16b_1;
		}
		else if (SAMPLERATE_32K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_32K;
			static unsigned short pcmFrame_32k_16b_1[PCM_FRAME_SIZE_32K << 1];
			pcmFrame = pcmFrame_32k_16b_1;
		}
		else if (SAMPLERATE_44K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_44K;
			static unsigned short pcmFrame_44k_16b_1[PCM_FRAME_SIZE_44K << 1];
			pcmFrame = pcmFrame_44k_16b_1;
		}
		else if (SAMPLERATE_48K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_48K;
			static unsigned short pcmFrame_48k_16b_1[PCM_FRAME_SIZE_48K << 1];
			pcmFrame = pcmFrame_48k_16b_1;
		}
		else
		{
			return -1;
		}

		frameSize *= sampleBitSize / 8;
		nRead = file.Read((byte*)pcmFrame, frameSize * 2);
		for (int x = 0; x < frameSize; ++x)
		{
			buffer[x] = (short)pcmFrame[x + 0];
		}
	}
	else if (sampleBitSize == 16 && channels == 2)
	{
		unsigned short* pcmFrame = NULL;
		if (SAMPLERATE_8K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_8K;
			static unsigned short pcmFrame_8k_16b_2[PCM_FRAME_SIZE_8K << 2];
			pcmFrame = pcmFrame_8k_16b_2;
		}
		else if (SAMPLERATE_16K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_16K;
			static unsigned short pcmFrame_16k_16b_2[PCM_FRAME_SIZE_16K << 2];
			pcmFrame = pcmFrame_16k_16b_2;
		}
		else if (SAMPLERATE_32K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_32K;
			static unsigned short pcmFrame_32k_16b_2[PCM_FRAME_SIZE_32K << 2];
			pcmFrame = pcmFrame_32k_16b_2;
		}
		else if (SAMPLERATE_44K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_44K;
			static unsigned short pcmFrame_44k_16b_2[PCM_FRAME_SIZE_44K << 2];
			pcmFrame = pcmFrame_44k_16b_2;
		}
		else if (SAMPLERATE_48K == sampleRate)
		{
			frameSize = PCM_FRAME_SIZE_48K;
			static unsigned short pcmFrame_48k_16b_2[PCM_FRAME_SIZE_48K << 2];
			pcmFrame = pcmFrame_48k_16b_2;
		}
		else
		{
			return -1;
		}
		frameSize = sampleBitSize / 8 * channels;
		nRead = file.Read((byte*)pcmFrame, frameSize * 2);
		for (int x = 0, y = 0; y < frameSize; ++y, x += 2)
		{
			//speech[y] = (short)pcmFrame_16b2[x+0];
			buffer[y] = (short)((int)((int)pcmFrame[x + 0] + (int)pcmFrame[x + 1])) >> 1;
		}
	}

	// 如果读到的数据不是一个完整的PCM帧, 就返回0
	if (nRead < frameSize * channels)
	{
		return 0;
	}

	return nRead;
}

int AudioResample::ResampleFile(int channels, int sampleBitSize, int inSampleRate, const XString& srcPath, int outSampleRate, const XString& destPath)
{
	youmecommon::CXFile srcFile;
	if (0 != srcFile.LoadFile(srcPath, youmecommon::CXFile::Mode_OpenExist_ReadOnly))
	{
		YouMe_LOG_Error(__XT("open file failed %s"), srcPath.c_str());
		return -1;
	}

	int ret = InitResample(channels, inSampleRate, outSampleRate);
	if (ret != 0)
	{
		srcFile.Close();
		return -1;
	}

	youmecommon::CXFile tempFile;
	XString tempPath = srcPath + __XT(".tmp");
	if (0 != tempFile.LoadFile(tempPath, youmecommon::CXFile::Mode_CREATE_ALWAYS))
	{
		YouMe_LOG_Error(__XT("create file failed %s"));
		srcFile.Close();
		return -1;
	}

	srcFile.Seek(44, SEEK_SET);
	int iFrameSize = inSampleRate / 50 * (sampleBitSize / 8);
	youmecommon::CXSharedArray<short> buffer(iFrameSize);
	youmecommon::CXSharedArray<short> outBuffer(iFrameSize);
	while (true)
	{
		if (!ReadPCMFrame(buffer.Get(), srcFile, channels, inSampleRate, sampleBitSize))
		{
			break;
		}
		int size = ResampleAudioData(buffer.Get(), iFrameSize, outBuffer);
		tempFile.Write((const byte*)outBuffer.Get(), size*sizeof(short));
	}

	DestroyResample();

	srcFile.Close();
	tempFile.Close();

	if (!SpeechUtil::PcmToWav(tempPath, sampleBitSize, channels, outSampleRate, 0, destPath))
	{
		YouMe_LOG_Error(__XT("convert wav failed"));
		youmecommon::CXFile::remove_file(tempPath);
		return -1;
	}
	youmecommon::CXFile::remove_file(tempPath);

	return 0;
}