#ifndef WAVREADER_H
#define WAVREADER_H

#include "AGCPlatform.h"
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
namespace youmecommon
{
	class WavReader
	{
	public:
		WavReader();
		~WavReader();

		bool Open(const XCHAR *inFileName);
		void Close();
		OsInt32 Read(OsInt16 *ioData, OsInt32 inLen);
        
        void ResetToHead();

		OsInt32 GetDataSize() { return m_nLength; }
		OsUInt32 GetSampleRate() { return m_nSampleRate; }
		OsUInt16 GetChannels() { return m_nChannels; }
		OsUInt16 GetBitsPerSample() { return m_nBitsPerSample; }
		OsUInt32    m_nSampleRate;
	private:
		FILE        *m_pFile;
		OsInt32     m_nLength;
		OsUInt16    m_nFormatTag;   // format type
		OsUInt16    m_nChannels;
		OsUInt32    m_nAvgBytesPerSec;
		OsUInt16    m_nBlockAlign;
		OsUInt16    m_nBitsPerSample;
	};
}
#endif
