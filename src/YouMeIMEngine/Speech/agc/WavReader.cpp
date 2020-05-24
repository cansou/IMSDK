#include "WavReader.h"
namespace youmecommon
{
#define WF_OFFSET_FORMATTAG         20
#define WF_OFFSET_CHANNELS          22
#define WF_OFFSET_SAMPLESPERSEC     24
#define WF_OFFSET_AVGBYTESPERSEC    28
#define WF_OFFSET_BLOCKALIGN        32
#define WF_OFFSET_BITSPERSAMPLE     34
#define WF_OFFSET_DATASIZE          40
#define WF_OFFSET_DATA              44
#define WF_HEADER_SIZE  WF_OFFSET_DATA

	WavReader::WavReader()
		: m_nSampleRate(0)
        , m_pFile(0)
		, m_nLength(0)
        , m_nFormatTag(0)
        , m_nChannels(0)
		, m_nAvgBytesPerSec(0)
		, m_nBlockAlign(0)
		, m_nBitsPerSample(0)
	{
	}

	WavReader::~WavReader()
	{
		Close();
	}

	bool WavReader::Open(const XCHAR *inFileName)
	{
		unsigned char aHeader[WF_HEADER_SIZE];

#ifdef WIN32
		m_pFile = _wfopen(inFileName, __XT("rb"));

#else	
		m_pFile = fopen(inFileName, __XT("rb"));
#endif

		if (!m_pFile) return false;

		fseek(m_pFile, 0, SEEK_END);
		m_nLength = OsInt32(ftell(m_pFile) - WF_HEADER_SIZE);

		fseek(m_pFile, 0, SEEK_SET);
		fread(aHeader, 1, WF_HEADER_SIZE, m_pFile);

		m_nFormatTag = *((OsUInt16*)(aHeader + WF_OFFSET_FORMATTAG));
		m_nChannels = *((OsUInt16*)(aHeader + WF_OFFSET_CHANNELS));
		m_nSampleRate = *((OsUInt32*)(aHeader + WF_OFFSET_SAMPLESPERSEC));
		m_nAvgBytesPerSec = *((OsUInt32*)(aHeader + WF_OFFSET_AVGBYTESPERSEC));
		m_nBlockAlign = *((OsUInt16*)(aHeader + WF_OFFSET_BLOCKALIGN));
		m_nBitsPerSample = *((OsUInt16*)(aHeader + WF_OFFSET_BITSPERSAMPLE));
		return true;
	}
    
    void WavReader::ResetToHead()
    {
        if( m_pFile )
        {
            fseek(m_pFile, WF_HEADER_SIZE, SEEK_SET);
        }
    }

	void WavReader::Close()
	{
		if (m_pFile)
		{
			fclose(m_pFile);
			m_pFile = 0;
		}
	}

	OsInt32 WavReader::Read(OsInt16 *ioData, OsInt32 inLen)
	{
		return (OsInt32)fread(ioData, 2, inLen, m_pFile);
	}
}
