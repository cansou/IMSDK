#include <stdio.h>

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/CInterface/IYouMeCInterface.h>
#if WIN32
#include <Windows.h>
#include <WinSock2.h>
#else
#include <unistd.h>
#endif
#include <memory>
#include <thread>
#include <YouMeCommon/curl/curl.h>
#include <YouMeCommon/TimeUtil.h>
#include <YouMeCommon/DownloadUploadManager.h>
void mYGetMessage()
{
	while (true)
	{
		 const XCHAR* pszMsg = IM_GetMessage();
		 if (pszMsg == NULL)
		 {
			 continue;;
		 }
		 OutputDebugString("IM_GetMessage: ");
		 OutputDebugStringW(pszMsg);
		 OutputDebugString("\n");
		 IM_PopMessage(pszMsg);
	}
}

int main()
{
	IM_Init(__XT("YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0"), __XT("r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE="));
	IM_Login(__XT("joexie"), __XT("123456"));
	IM_SetMode(2);

	std::thread getMsg(mYGetMessage);
	
	while (true)
	{
		int  command = getchar();
		switch (command)
		{
		case '1': //登出
		{
			IM_Logout();
		}
		break;
		case '2':
		{
			IM_Login(__XT("joexie"), __XT("123456"));
		}
		break;
		case '3':
		{
			IM_SendTextMessage(__XT("winnie"), ChatType_PrivateChat, __XT("发送的测试文本"), NULL);
		}
		break;
		case '4':
		{
			IM_SendAudioMessage(__XT("winnie"), ChatType_PrivateChat, NULL);
		}
		break;
		case '5':
		{
			IM_SendOnlyAudioMessage(__XT("winnie"), ChatType_PrivateChat, NULL);
		}
		break;
		case '6':
		{
			IM_StopAudioMessage(__XT("停止参数"));
		}
		break;
		case '7':
		{
			IM_StartAudioSpeech(NULL, true);
		}
		break;
		case '8':
		{
			IM_StopAudioSpeech();
		}
		break;
		case '9':
		{
			IM_QueryHistoryMessage(__XT("winnie"), 3, 100, 1);
		}
		break;
		case 'a':
		{
			IM_DeleteHistoryMessage(3, 0);
		}
		break;
		case 'b':
		{
			IM_DeleteHistoryMessage(0, 1481803098);
		}
		break;
		case 'c':
		{
			IM_JoinChatRoom(__XT("test"));
		}
		break;
		case 'd':
		{
			IM_LeaveChatRoom(__XT("test"));
		}
		break;
		case 'e':
		{
			IM_SendTextMessage(__XT("test"), ChatType_RoomChat, __XT("发送的测试文本"), NULL);

		}
		break;
		case 'f':
		{
			IM_QueryRoomHistoryMessage(__XT("test"));
		}
		break;
		default:
			break;
		}
	}
    return 0;
}