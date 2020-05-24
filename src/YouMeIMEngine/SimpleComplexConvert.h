#ifndef SIMPLE_COMPLEX_CONVERT_H
#define SIMPLE_COMPLEX_CONVERT_H


#include <thread>
#include <YouMeIMEngine/YIMPlatformDefine.h>
#include <YouMeCommon/opencc/opencc.h>


enum FontCovertType
{
	CONVERT_SIMPLE_TO_TRADITION = 0,	// 简体->繁体
	CONVERT_TRADITION_TO_SIMPLE = 1,	// 繁体->简体
	CONVERT_SIMPLE_TO_HK = 2,			// 简体->繁体(香港)
	CONVERT_HK_TO_SIMPLE = 3,			// 繁体(香港)->简体
	CONVERT_TRADITION_TO_HK = 4,		// 繁体->繁体(香港)
	CONVERT_SIMPLE_TO_TW = 5,			// 简体->繁体(台湾)
	CONVERT_SIMPLE_TO_TWPHRASES = 6,	// 简体->繁体(台湾习语)
	CONVERT_TW_TO_SIMPLE = 7,			// 繁体(台湾)->简体
	CONVERT_TW_TO_SIMPLEPHRASES = 8,	// 繁体(台湾)->简体(习语)
	CONVERT_TRADITION_TO_TW = 9			// 繁体->繁体(台湾)
};

class SimpleComplexConvert
{
public:
	SimpleComplexConvert();
	~SimpleComplexConvert();
	void Init(FontCovertType type, const XString& lexiconMD5, const XString& lexiconURL);
	void Convert(const XString& srcText, XString& destText);

private:
	void DowndloadLexicon(FontCovertType type, const XString& lexiconMD5, const XString& lexiconURL);
	bool GetLexiconFiles(FontCovertType type, std::vector<XString>& files);
	bool CheckLexiconFiles(FontCovertType type);
	void DeleteOldLexiconFiles(FontCovertType type);

	opencc_t m_openccConvert;
	std::thread m_downloadThread;
	//std::string m_strLexiconPath;
};


#endif