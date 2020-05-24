#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeCommon/Log.h>
#include <YouMeIMEngine/YIM.h>
#include <YouMeIMEngine/YouMeIMManager.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/NetworkService.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeIMEngine/Speech/YouMeSpeechManager.h>
#include <YouMeCommon/KeywordFilter.h>
#include <YouMeCommon/YouMeDataChannel.h>
#include <YouMeCommon/profiledb.h>
#include <YouMeCommon/minizip/MiniZip.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/TranslateUtil.h>
#ifdef WIN32
#include <YouMeCommon/CrossPlatformDefine/Windows/YouMeApplication_Win.h>
#include <TlHelp32.h>
#include <Psapi.h>
#elif (OS_IOS || OS_IOSSIMULATOR)
#include <YouMeCommon/CrossPlatformDefine/IOS/YouMeApplication_iOS.h>
#elif OS_OSX
#include <YouMeCommon/CrossPlatformDefine/OSX/YouMeApplication_OSX.h>
#elif OS_ANDROID
#include <YouMeIMEngine/Android/YouMeApplication_Android.h>
extern YouMeApplication_Android* g_AndroidSystemProvider;
#endif


#define MAX_LOG_SIZE  10 *1024 *1024
#define MAX_DELETE_LOG_SIZE 20 *1024 *1024
bool s_bYouMeOpenLog = false;
bool s_bYouMeStartup = false;
ServerZone g_ServerZone = ServerZone_China;
int g_iYouMeIMMode = 0;
int g_iAppID = 0;
CYouMeSpeechManager* g_pSpeechManager = NULL;
CKeywordFilter* g_pKeywordFilter = NULL;
CProfileDB* g_pProfileDB = NULL;
IYouMeSystemProvider* g_pSystemProvider = NULL;
YouMeIMManager* g_pYouMeIMManager = NULL;
CTranslateUtil* g_pTranslateUtil = NULL;


IYIMMessageBodyBase::~IYIMMessageBodyBase()
{

}

IYIMMessage::~IYIMMessage()
{

}

IYIMContactsMessageInfo::~IYIMContactsMessageInfo()
{
    
}

YIMChatRoomManager::~YIMChatRoomManager()
{
    
}
YIMMessageManager::~YIMMessageManager()
{
    
}

YIMLocationManager::~YIMLocationManager()
{
    
}
IYIMForbidSpeakInfo::~IYIMForbidSpeakInfo()
{
    
}

YIMUserProfileManager::~YIMUserProfileManager()
{
    
}


#ifdef WIN32

std::wstring GetLogFileName()
{
	std::wstring logFileName = L"YouMeIMLogV2.txt";
	wchar_t exePath[MAX_PATH] = { 0 };
	if (GetModuleFileNameW(NULL, exePath, MAX_PATH) <= 0)
	{
		return logFileName;
	}

	int instanceCount = 0;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return logFileName;
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return logFileName;
	}
	do
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
		if (hProcess == NULL)
		{
			continue;
		}
		wchar_t path[MAX_PATH] = { 0 };
		GetModuleFileNameExW(hProcess, NULL, path, MAX_PATH + 1);
		CloseHandle(hProcess);
		if (wcscmp(path, exePath) == 0)
		{
			++instanceCount;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	if (instanceCount > 1)
	{
		wchar_t name[32] = { 0 };
		wsprintfW(name, L"YouMeIMLogV2(%d).txt", instanceCount - 1);
		logFileName = std::wstring(name);
	}
	return logFileName;
}

#endif


void Startup()
{
	if (s_bYouMeStartup)
	{
		return;
	}
#ifdef WIN32
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif // WIN32
	youmecommon::CNetworkService::Instance();

	//全局的application

#ifdef WIN32
	g_pSystemProvider = new YouMeApplication_Win;
#elif (OS_IOS || OS_IOSSIMULATOR)
	g_pSystemProvider = new YouMeApplication_iOS;
#elif OS_OSX
    g_pSystemProvider = new  YouMeApplication_OSX;
#elif OS_ANDROID
	g_pSystemProvider = g_AndroidSystemProvider;
#endif


	//日志目录
#ifdef OS_ANDROID
	XString strAppCacheDir = youmecommon::CXFile::CombinePath(__XT("/sdcard/Android/data/"), g_pSystemProvider->getPackageName());
	XString strCacheDir = youmecommon::CXFile::CombinePath(strAppCacheDir, __XT("yimcache"));
	youmecommon::CXFile::make_dir_tree(strCacheDir.c_str());
	XString strLogPath = youmecommon::CXFile::CombinePath(strCacheDir, __XT("YouMeIMLogV2.txt"));
#elif WIN32
	XString strLogPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getCachePath(), GetLogFileName());
#else
	XString strLogPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getCachePath(), __XT("YouMeIMLogV2.txt"));
#endif
	{
		//检查一下日志文件大小
		youmecommon::CXFile logFile;
		logFile.LoadFile(strLogPath, youmecommon::CXFile::Mode_OpenExist);
		XINT64 ulFileSize = logFile.GetFileSize();
		if (ulFileSize >= MAX_DELETE_LOG_SIZE)
		{
			//直接删除文件
			logFile.Close();
			youmecommon::CXFile::remove_file(strLogPath);
		}
		else if (ulFileSize >= MAX_LOG_SIZE)
		{
			logFile.Close();
			XString strLastZipPath = strLogPath + __XT(".zip");
			youmecommon::CXFile::remove_file(strLastZipPath);
			//备份一个
			youmecommon::CMiniZip logZip;
			if (logZip.Open(strLastZipPath))
			{
				logZip.AddOneFileToZip(strLogPath);
				logZip.Close();
			}
			youmecommon::CXFile::remove_file(strLogPath);
		}
	}

	YouMe_LOG_Init(strLogPath);
	g_pSpeechManager = new CYouMeSpeechManager;
	g_pKeywordFilter = new CKeywordFilter(__XT("**"));
	g_pTranslateUtil = new CTranslateUtil;
	s_bYouMeStartup = true;
}

void CleanUp()
{
	if (!s_bYouMeStartup)
	{
		return;
	}
    youmecommon::CNetworkService::Instance()->stop();
	youmecommon::CNetworkService::DestroyInstance();
	if (g_pSpeechManager != NULL)
	{
		g_pSpeechManager->UnInit();
		delete g_pSpeechManager;
		g_pSpeechManager = NULL;
	}
	if (g_pKeywordFilter != NULL)
	{
		delete g_pKeywordFilter;
		g_pKeywordFilter = NULL;
	}
	if (g_pTranslateUtil != NULL)
	{
		delete g_pTranslateUtil;
		g_pTranslateUtil = NULL;
	}
#ifdef WIN32
	WSACleanup();
#endif // WIN32
	delete g_pSystemProvider;
	s_bYouMeStartup = false;
}

YIMManager* YIMManager::CreateInstance()
{
	if (g_pYouMeIMManager == NULL)
	{
		g_pYouMeIMManager = new YouMeIMManager;
		Startup();
	}
	return g_pYouMeIMManager;
}

/*void YIMManager::DestroyInstance()
{
	if (g_pYouMeIMManager != NULL)
	{
		delete g_pYouMeIMManager;
		g_pYouMeIMManager = NULL;

		CleanUp();
	}
}*/

YIMManager::~YIMManager()
{

}

int YIMManager::GetSDKVersion()
{
	return SDK_VER;
}

void YIMManager::SetServerZone(ServerZone zone)
{
	g_ServerZone = zone;
}

YIMErrorcode YIMManager::SetLoginAddress(const char* ip, unsigned short port)
{
	return YouMeIMManager::SetLoginAddress(ip, port);
}

void YIMManager::SetMode(int mode)
{
	g_iYouMeIMMode = mode;
}

void YIMManager::SetAppId(int appId)
{
	g_iAppID = appId;
} 

XString YIMManager::FilterKeyword(const XCHAR* text, int* level)
{
	if (NULL == g_pKeywordFilter)
	{
		return text;
	}
	return g_pKeywordFilter->GetFilterResult(text, level);
}

void YIMManager::SetAudioCacheDir(const XCHAR* audioCacheDir)
{
	if (NULL == audioCacheDir)
	{
		return;
	}
	if (g_pSpeechManager != NULL)
	{
		g_pSpeechManager->SetAudioRecordCacheDir(audioCacheDir);
	}
	else
	{
		CYouMeSpeechManager::SetAudioRecordCacheDir(audioCacheDir);
	}
}

XString YIMManager::GetAudioCachePath()
{
	if (g_pSpeechManager != NULL)
	{
		return g_pSpeechManager->GetAudioRecordCacheDir();
	}
	else
	{
		return XString(__XT(""));
	}
}

bool YIMManager::ClearAudioCachePath()
{
	XString path = GetAudioCachePath();
	if (!path.empty())
	{
		return youmecommon::CXFile::delete_dir(path.c_str(), false);
	}
	return false;
}

void YIMManager::SetVolume(float volume)
{
	if (g_pSpeechManager != NULL)
	{
		g_pSpeechManager->SetPlayVolume(volume);
	}
}

YIMErrorcode YIMManager::StartPlayAudio(const XCHAR* path)
{
	if (NULL == path)
	{
        YouMe_LOG_Error(__XT("StartPlayAudio path is null"));
		return YIMErrorcode_ParamInvalid;
	}
	if (NULL == g_pSpeechManager)
	{
		YouMe_LOG_Error(__XT("speechmanager is not init"));
		return YIMErrorcode_EngineNotInit;
	}
	return g_pSpeechManager->StartPlayAudio(path);
}

YIMErrorcode YIMManager::StopPlayAudio()
{
	if (NULL == g_pSpeechManager)
	{
		YouMe_LOG_Error(__XT("speechmanager is not init"));
		return YIMErrorcode_EngineNotInit;
	}
	return g_pSpeechManager->StopPlayAudio();
}

bool YIMManager::IsPlaying()
{
	if (g_pSpeechManager != NULL)
	{
		return g_pSpeechManager->IsPlaying();
	}
	return false;
}

void YIMManager::SetKeepRecordModel(bool keep)
{
	if (g_pSpeechManager != NULL)
	{
		g_pSpeechManager->SetKeepRecordModel(keep);
	}
}

void YIMManager::GetMicrophoneStatus()
{
	if (g_pSpeechManager != NULL)
	{
		g_pSpeechManager->GetMicrophoneStatus();
	}
}