#include "SimpleComplexConvert.h"
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/CrossPlatformDefine/IYouMeSystemProvider.h>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/DownloadUploadManager.h>
#include <YouMeCommon/minizip/MiniUnZip.h>
#include <YouMeCommon/Log.h>

extern IYouMeSystemProvider* g_pSystemProvider;


SimpleComplexConvert::SimpleComplexConvert() : m_openccConvert(NULL)
{

}

SimpleComplexConvert::~SimpleComplexConvert()
{
	if (m_openccConvert != NULL)
	{
		opencc_close(m_openccConvert);
	}
	if (m_downloadThread.joinable())
	{
		m_downloadThread.join();
	}
}

void SimpleComplexConvert::Init(FontCovertType type, const XString& lexiconMD5, const XString& lexiconURL)
{
	if (lexiconURL.empty() || lexiconMD5.empty())
	{
		return;
	}
	if (m_openccConvert != NULL)
	{
		return;
	}
	if (m_downloadThread.joinable())
	{
		m_downloadThread.join();
	}
	m_downloadThread = std::thread(&SimpleComplexConvert::DowndloadLexicon, this, type, lexiconMD5, lexiconURL);
}

void SimpleComplexConvert::Convert(const XString& srcText, XString& destText)
{
	/*if (m_strLexiconPath.empty())
	{
		destText = srcText;
		return;
	}
	if (srcText.empty())
	{
		return;
	}

	std::string inputText = XStringToUTF8(srcText);
	opencc::SimpleConverter m_bSimpleConverter(m_strLexiconPath);
	std::string result = m_bSimpleConverter.Convert(inputText);
	if (!result.empty())
	{
		destText = UTF8TOXString(result);
	}*/


	if (NULL == m_openccConvert)
	{
		destText = srcText;
		return;
	}
	if (srcText.empty())
	{
		return;
	}

	std::string inputText = XStringToUTF8(srcText);
	char result[2048] = { '\0' };
	size_t length = opencc_convert_utf8_to_buffer(m_openccConvert, inputText.c_str(), inputText.size(), result);
	if (length > 0 && strlen(result) > 0)
	{
		std::string temp = std::string(result, strlen(result));
		destText = UTF8TOXString(temp);
	}
}

void SimpleComplexConvert::DowndloadLexicon(FontCovertType type, const XString& lexiconMD5, const XString& lexiconURL)
{
	YouMe_LOG_Debug(__XT("Enter"));

	if (NULL == g_pSystemProvider)
	{
		return;
	}
	XString fileName = lexiconURL.substr(lexiconURL.find_last_of('/') + 1);
	XString dir = g_pSystemProvider->getCachePath();
	XString savePath = youmecommon::CXFile::CombinePath(dir, fileName);
	int ret = -1;
	if (youmecommon::CXFile::IsFileExist(savePath))
	{
		XString fileMD5 = youmecommon::CCryptUtil::MD5File(savePath);
		ret = CStringUtil::compare_nocase(fileMD5.c_str(), lexiconMD5.c_str());
		if (ret != 0)
		{
			youmecommon::CXFile::remove_file(savePath);
		}
	}
	if (ret != 0)
	{
		if (!CDownloadUploadManager::DownloadFile(lexiconURL, savePath))
		{
			YouMe_LOG_Error(__XT("download failde url:%s"), lexiconURL.c_str());
			return;
		}
		DeleteOldLexiconFiles(type);
	}
	if (!CheckLexiconFiles(type))
	{
		CMiniUnZip unzip;
		if (!unzip.Open(savePath))
		{
			YouMe_LOG_Error(__XT("open lexicon file failed %s"), savePath.c_str());
			return;
		}
		if (!unzip.UnZipToDir(g_pSystemProvider->getCachePath()))
		{
			YouMe_LOG_Error(__XT("unzip lexicon file failed %s"), savePath.c_str());
			return;
		}
		if (!CheckLexiconFiles(type))
		{
			YouMe_LOG_Error(__XT("lexicon file not complete"));
			return;
		}
	}
	
	std::vector<XString> files;
	GetLexiconFiles(type, files);
	if (files.size() > 0)
	{
		XString configFile = youmecommon::CXFile::CombinePath(dir, files[0]);
		m_openccConvert = opencc_open(XStringToUTF8(configFile).c_str());
		//m_strLexiconPath = XStringToUTF8(configFile);
	}

	YouMe_LOG_Debug(__XT("Leave"));
}

bool SimpleComplexConvert::GetLexiconFiles(FontCovertType type, std::vector<XString>& files)
{
	switch (type)
	{
	case CONVERT_SIMPLE_TO_TRADITION:
	{
		files.push_back(__XT("s2t.json"));
		files.push_back(__XT("STCharacters.txt"));
		files.push_back(__XT("STPhrases.txt"));
	}
	break;
	case CONVERT_TRADITION_TO_SIMPLE:
	{
		files.push_back(__XT("t2s.json"));
		files.push_back(__XT("TSPhrases.txt"));
		files.push_back(__XT("TSCharacters.txt"));
	}
	break;
	case CONVERT_SIMPLE_TO_HK:
	{
		files.push_back(__XT("s2hk.json"));
		files.push_back(__XT("STPhrases.txt"));
		files.push_back(__XT("STCharacters.txt"));
		files.push_back(__XT("HKVariantsPhrases.txt"));
		files.push_back(__XT("HKVariants.txt"));
	}
	break;
	case CONVERT_HK_TO_SIMPLE:
	{
		files.push_back(__XT("hk2s.json"));
		files.push_back(__XT("TSPhrases.txt"));
		files.push_back(__XT("TSCharacters.txt"));
		files.push_back(__XT("HKVariantsRevPhrases.txt"));
		files.push_back(__XT("HKVariantsRev.txt"));
	}
	break;
	case CONVERT_TRADITION_TO_HK:
	{
		files.push_back(__XT("t2hk.json"));
		files.push_back(__XT("HKVariants.txt"));
	}
	break;
	case CONVERT_SIMPLE_TO_TW:
	{
		files.push_back(__XT("s2tw.json"));
		files.push_back(__XT("STPhrases.txt"));
		files.push_back(__XT("STCharacters.txt"));
		files.push_back(__XT("TWVariants.txt"));
	}
	break;
	case CONVERT_SIMPLE_TO_TWPHRASES:
	{
		files.push_back(__XT("s2twp.json"));
		files.push_back(__XT("STPhrases.txt"));
		files.push_back(__XT("STCharacters.txt"));
		files.push_back(__XT("TWPhrases.txt"));
		files.push_back(__XT("TWVariants.txt"));
	}
	break;
	case CONVERT_TW_TO_SIMPLE:
	{
		files.push_back(__XT("tw2s.json"));
		files.push_back(__XT("TSPhrases.txt"));
		files.push_back(__XT("TSCharacters.txt"));
		files.push_back(__XT("TWVariantsRevPhrases.txt"));
		files.push_back(__XT("TWVariantsRev.txt"));
	}
	break;
	case CONVERT_TW_TO_SIMPLEPHRASES:
	{
		files.push_back(__XT("tw2sp.json"));
		files.push_back(__XT("TSPhrases.txt"));
		files.push_back(__XT("TSCharacters.txt"));
		files.push_back(__XT("TWVariantsRevPhrases.txt"));
		files.push_back(__XT("TWVariantsRev.txt"));
		files.push_back(__XT("TWPhrasesRev.txt"));
	}
	break;
	case CONVERT_TRADITION_TO_TW:
	{
		files.push_back(__XT("t2tw.json"));
		files.push_back(__XT("TWVariants.txt"));
	}
	break;
	break;
	default:
		return false;
	}
	return true;
}

bool SimpleComplexConvert::CheckLexiconFiles(FontCovertType type)
{
	if (g_pSystemProvider == NULL)
	{
		return false;
	}
	std::vector<XString> files;
	if (!GetLexiconFiles(type, files))
	{
		return false;
	}
	XString dir = g_pSystemProvider->getCachePath();
	for (std::vector<XString>::const_iterator itr = files.begin(); itr != files.end(); ++itr)
	{
		XString path = youmecommon::CXFile::CombinePath(dir, *itr);
		if (!youmecommon::CXFile::IsFileExist(path))
		{
			return false;
		}
	}
	return true;
}

void SimpleComplexConvert::DeleteOldLexiconFiles(FontCovertType type)
{
	if (g_pSystemProvider == NULL)
	{
		return;
	}
	std::vector<XString> files;
	if (!GetLexiconFiles(type, files))
	{
		return;
	}
	XString dir = g_pSystemProvider->getCachePath();
	for (std::vector<XString>::const_iterator itr = files.begin(); itr != files.end(); ++itr)
	{
		XString path = youmecommon::CXFile::CombinePath(dir, *itr);
		youmecommon::CXFile::remove_file(path);
	}
}
