#include "com_youme_im_NativeEngine.h"
#include <YouMeCommon/NetworkService.h>
#include <YouMeIMEngine/Speech/Android/AndroidVoiceManager.h>
#include <YouMeIMEngine/Android/YouMeApplication_Android.h>
#include "../../CInterface/IYouMeCInterface.h"
#include <YouMeCommon/Log.h>
#include <YouMeIMEngine/Location/LocationInterface.h>

static JavaVM *mJvm = NULL;
static jclass s_voiceManager = NULL;
static jmethodID m_initSpeechRecognizer = 0;
static jmethodID m_setAudioRecordCacheDir = 0;
static jmethodID m_getAudioRecordCacheDir = 0;
static jmethodID m_setAudioRecordParam = 0;
static jmethodID m_setRecognizeLanguage = 0;
static jmethodID m_startSpeech = 0;
static jmethodID m_startOnlySpeech = 0;
static jmethodID m_stopSpeech = 0;
static jmethodID m_cancleSpeech = 0;
static jmethodID m_uninit = 0;
static jmethodID m_updateToken = 0;

static jmethodID m_setPlayVolume = 0;
static jmethodID m_startPlayAudio = 0;
static jmethodID m_stopPlayAudio = 0;
static jmethodID m_isPlaying = 0;
static jmethodID m_getMicrophoneStatus = 0;

static jmethodID m_getNetworkType = 0;

static jclass mYouGeoLocationClass = NULL;
static jmethodID m_YoumeGetGeographyLocation = 0;

static jclass mDealImageClass = NULL;
static jmethodID m_resizeImage = 0;

IAudioListener* g_AndroidSpeechListen = NULL;
ILocationListen* g_AndroidLocationLisen = NULL;
YouMeApplication_Android* g_AndroidSystemProvider = new YouMeApplication_Android;

/*
typedef union jvalue {
    jboolean    z;
    jbyte       b;
    jchar       c;
    jshort      s;
    jint        i;
    jlong       j;
    jfloat      f;
    jdouble     d;
    jobject     l;
} jvalue;

*/
class JNIEvnWrap
{
public:
    JNIEvnWrap()
    {
        m_pThreadJni = NULL;
        m_bAttachThread = false;
        if (NULL == mJvm) {
            return;
        }
        
        if (mJvm->GetEnv ((void **)&m_pThreadJni, JNI_VERSION_1_4) != JNI_OK)
        {
            struct JavaVMAttachArgs args = {0};
            args.version = JNI_VERSION_1_4;
            args.name="youmeim";
            args.group = NULL;

            int status = mJvm->AttachCurrentThread (&m_pThreadJni, &args );

            if (status >= 0)
            {
                m_bAttachThread = true;
            }
        }
        
    }
    ~JNIEvnWrap()
    {
        if ((NULL != m_pThreadJni) && m_bAttachThread)
        {
            mJvm->DetachCurrentThread ();
        }
    }
    
    JNIEnv* m_pThreadJni;
    bool m_bAttachThread;
};


//初始化的时候会调进来一次，在这个方法里持有jvm的引用
JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM *jvm, void *reserved)
{
    mJvm = jvm;
    JNIEnv *pEnv = NULL;
    if (NULL == mJvm)
    {
        return -1;
    }
    if (mJvm->GetEnv ((void **)&pEnv, JNI_VERSION_1_4) != JNI_OK)
    {
        return -1;
    }

    //语音接口
    jclass voiceManagerClass = pEnv->FindClass ("com/youme/voice/VoiceManager");
    if (NULL == voiceManagerClass)
    {
        return -1;
    }
	s_voiceManager = (jclass)(pEnv->NewGlobalRef (voiceManagerClass));
	
	m_initSpeechRecognizer = pEnv->GetStaticMethodID (s_voiceManager, "InitSpeechRecognizer", "(Ljava/lang/String;Ljava/lang/String;I)V");
	m_setAudioRecordCacheDir = pEnv->GetStaticMethodID (s_voiceManager, "SetAudioRecordCacheDir", "(Ljava/lang/String;)V");
	m_getAudioRecordCacheDir = pEnv->GetStaticMethodID (s_voiceManager, "GetAudioRecordCacheDir", "()Ljava/lang/String;");
	m_setAudioRecordParam = pEnv->GetStaticMethodID (s_voiceManager, "SetAudioRecordParam", "(III)V");
	m_setRecognizeLanguage = pEnv->GetStaticMethodID (s_voiceManager, "SetRecognizeLanguage", "(I)V");
	m_startSpeech = pEnv->GetStaticMethodID (s_voiceManager, "StartSpeech", "(J)I");
	m_startOnlySpeech = pEnv->GetStaticMethodID (s_voiceManager, "StartOnlySpeech", "(J)I");
	m_stopSpeech = pEnv->GetStaticMethodID (s_voiceManager, "StopSpeech", "()I");
	m_cancleSpeech = pEnv->GetStaticMethodID (s_voiceManager, "CancleSpeech", "()I");
	m_uninit = pEnv->GetStaticMethodID (s_voiceManager, "UnInit", "()V");
	m_updateToken = pEnv->GetStaticMethodID (s_voiceManager, "UpdateToken", "(Ljava/lang/String;)V");
	
	m_setPlayVolume = pEnv->GetStaticMethodID (s_voiceManager, "SetPlayVolume", "(F)V");
	m_startPlayAudio = pEnv->GetStaticMethodID (s_voiceManager, "StartPlayAudio", "(Ljava/lang/String;)I");
	m_stopPlayAudio = pEnv->GetStaticMethodID (s_voiceManager, "StopPlayAudio", "()I");
	m_isPlaying = pEnv->GetStaticMethodID (s_voiceManager, "IsPlaying", "()Z");
	m_getMicrophoneStatus = pEnv->GetStaticMethodID (s_voiceManager, "GetMicrophoneStatus", "()I");

	m_getNetworkType = pEnv->GetStaticMethodID (s_voiceManager, "GetNetworkType", "()I");
	
	
	jclass geographyLocationManager = pEnv->FindClass("com/youme/lbs/GeographyLocationManager");
	if (geographyLocationManager != NULL)
	{
		mYouGeoLocationClass = (jclass)(pEnv->NewGlobalRef(geographyLocationManager));
		m_YoumeGetGeographyLocation = pEnv->GetStaticMethodID(mYouGeoLocationClass, "GetGeographyLocation", "()I");
	}
    
    /*jclass dealImageManager = pEnv->FindClass("com/youme/im/DealImageSize");
    if (dealImageManager != NULL)
    {
        mDealImageClass = (jclass)(pEnv->NewGlobalRef(dealImageManager));
        m_resizeImage = pEnv->GetStaticMethodID(mDealImageClass,"ResizeImage","(Ljava/lang/String;Ljava/lang/String;)V");
    }*/
		
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload (JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;
    
    if (vm->GetEnv ((void **)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        return;
    }
    if (NULL == env)
    {
        return;
    }
    env->DeleteGlobalRef(s_voiceManager);
	env->DeleteGlobalRef(mYouGeoLocationClass);
}

// char* to jstring
jstring string2jstring (JNIEnv *env, const char *str)
{
    jstring rtstr = env->NewStringUTF (str);
    return rtstr;
}

std::string jstring2string (JNIEnv *env, jstring jstr)
{
    std::string strResult;
    jclass clsstring = env->FindClass ("java/lang/String");
    jstring strencode = env->NewStringUTF ("utf-8");
    jmethodID mid = env->GetMethodID (clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod (jstr, mid, strencode);
    jsize alen = env->GetArrayLength (barr);
    jbyte *ba = env->GetByteArrayElements (barr, JNI_FALSE);
    if (alen > 0)
    {
        strResult = std::string((const char*)ba,alen);
    }
    env->ReleaseByteArrayElements (barr, ba, 0);
    return strResult;
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setModel(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strModel= (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setSysVersion(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strSystemVersion = (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setDeviceIMEI(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strUUID= (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setCPUArch(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strCPUArchive= (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setPackageName(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strPackageName= (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setDocumentPath(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strDocumentPath= (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setCachePath(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strCachePath= (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_onNetWorkChanged(JNIEnv *env, jclass object, jint para)
{
    youmecommon::CNetworkService::Instance()->onNetWorkChanged ((youmecommon::NetworkType)para);
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setBrand(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strBrand=(jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setCPUChip(JNIEnv *env, jclass object, jstring para)
{
    g_AndroidSystemProvider->m_strCpuChip= (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_WriteLog(JNIEnv *env, jclass clazz, jint level, jstring text)
{
	std::string strText = jstring2string(env, text);
	switch (level)
	{
	case 0:
	{
		YouMe_LOG_Fatal(strText.c_str());
	}
		break;
	case 1:
	{
		YouMe_LOG_Error(strText.c_str());
	}
		break;
	case 2:
	{
		YouMe_LOG_Warning(strText.c_str());
	}
		break;
	case 3:
	{
		YouMe_LOG_Info(strText.c_str());
	}
		break;
	case 4:
	{
		YouMe_LOG_Debug(strText.c_str());
	}
		break;
	default:
		break;
	}	
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnRecordFinish(JNIEnv *env, jclass object, jint errorcode, jstring path, jlong serial, jstring text)
{
	if (NULL != g_AndroidSpeechListen)
	{
		g_AndroidSpeechListen->OnRecordFinish(errorcode, jstring2string(env,path), serial, jstring2string(env,text));
    }
}

JNIEXPORT jbyteArray JNICALL Java_com_youme_im_NativeEngine_OnRecordData(JNIEnv *env, jclass object, jbyteArray data, jint size, jboolean resample)
{
	if (NULL != g_AndroidSpeechListen)
	{
	   jbyte* pData = env->GetByteArrayElements(data, JNI_FALSE);
	   if (pData != NULL)
	   {
		   g_AndroidSpeechListen->OnAudioRecordData((unsigned char*)pData, size);
		   
		   if (resample)
		   {
			   unsigned int outSize = 0;
			   unsigned char* buffer = g_AndroidSpeechListen->ResampleAudioData((unsigned char*)pData, size, &outSize);
			   if (buffer != NULL)
			   {
				    env->ReleaseByteArrayElements(data, pData, 0);
					jbyteArray array = env->NewByteArray(outSize);
					env->SetByteArrayRegion(array, 0, outSize, (jbyte*)buffer);
					return array;
			   }
		   }
		   env->ReleaseByteArrayElements(data, pData, 0);
	   }
	   
	}
	return NULL;
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnPlayFinish(JNIEnv *env, jclass object, jint errorcode, jstring path)
{
	if (NULL != g_AndroidSpeechListen)
	{
		g_AndroidSpeechListen->OnPlayFinish(errorcode, jstring2string(env, path));
    }
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnPlayData(JNIEnv *env, jclass object, jbyteArray, jint)
{
	
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_NotifyVolumeChange(JNIEnv *env, jclass, jint volume, jint type)
{
    if (NULL != g_AndroidSpeechListen)
	{
        g_AndroidSpeechListen->OnRecordVolumeChange(volume, type);
    }
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setUpdateReadStatusCallbackFlag(JNIEnv * env, jclass clazz, jboolean flag)
{
    IM_UpdateReadStatusCallbackFlag(flag);
}

#ifndef NO_C_Interface
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_Init
        (JNIEnv *env, jclass clazz, jstring strAppKey, jstring strSecrect)
{
    std::string appKey = jstring2string(env, strAppKey);
    std::string secrect = jstring2string(env, strSecrect);
    return (jint)IM_Init(appKey.c_str(), secrect.c_str());
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_Uninit
        (JNIEnv * env, jclass clazz)
{
    IM_Uninit();
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_Login
        (JNIEnv *env, jclass clazz, jstring userID, jstring strPasswd,jstring strToken)
{
	std::string id = jstring2string(env, userID);
    std::string pw = jstring2string(env, strPasswd);
	std::string token = jstring2string(env, strToken);
    return (jint)IM_Login(id.c_str(), pw.c_str(),token.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_Logout
        (JNIEnv *env, jclass clazz)
{
    IM_Logout();
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendTextMessage
        (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jstring strContent, jstring strAttachParam, jobject requestID)
{
    std::string recvID = jstring2string(env, strRecvID);
    std::string content = jstring2string(env, strContent);
    std::string attachParam = jstring2string(env, strAttachParam);
    XUINT64 requestid = 0;
    jint errorcode = (jint)IM_SendTextMessage(recvID.c_str(), (YIMChatType)chatType, content.c_str(), attachParam.c_str(), &requestid);
//    YouMe_LOG_Debug("********requestid:%d\n", requestid);
    if (NULL == requestID)
        return 10000;
    jclass class_name = env->GetObjectClass(requestID);
    if (NULL == class_name)
        return 10000;
    jmethodID mid = env->GetMethodID(class_name, "setId", "(J)V");
    if (NULL == mid)
        return 10000;
    env->CallVoidMethod(requestID, mid, requestid);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendCustomMessage
        (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jbyteArray strCustomMessage, jint iBufferLen, jobject requestID)
{
    std::string recvID = jstring2string(env, strRecvID);
//    std::string customMsg = jstring2string(env, strCustomMessage);
    jboolean isCopy;
    jbyte* customMsg = env->GetByteArrayElements( strCustomMessage, &isCopy);
    XUINT64 requestid = 0;
    jint errorcode = (jint)IM_SendCustomMessage(recvID.c_str(), (YIMChatType)chatType, (char*)customMsg, (int)iBufferLen, &requestid);
    env->ReleaseByteArrayElements(strCustomMessage,customMsg,JNI_ABORT);
//    YouMe_LOG_Debug("********requestid:%d\n", requestid);
    if (NULL == requestID)
        return 10000;
    jclass class_name = env->GetObjectClass(requestID);
    if (NULL == class_name)
        return 10000;
    jmethodID mid = env->GetMethodID(class_name, "setId", "(J)V");
    if (NULL == mid)
        return 10000;
    env->CallVoidMethod(requestID, mid, requestid);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendFile
        (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jstring strFilePath, jstring strExtParam, jint iFileType, jobject requestID)
{
    std::string recvID = jstring2string(env, strRecvID);
    std::string filePath = jstring2string(env, strFilePath);
    std::string extParam = jstring2string(env, strExtParam);
    XUINT64 requestid = 0;
    jint errorcode = (jint)IM_SendFile(recvID.c_str(), (YIMChatType)chatType, filePath.c_str(), extParam.c_str(),(YIMFileType) iFileType, &requestid);
//    YouMe_LOG_Debug("********requestid:%d\n", requestid);
    if (NULL == requestID)
        return 10000;
    jclass class_name = env->GetObjectClass(requestID);
    if (NULL == class_name)
        return 10000;
    jmethodID mid = env->GetMethodID(class_name, "setId", "(J)V");
    if (NULL == mid)
        return 10000;
    env->CallVoidMethod(requestID, mid, requestid);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendAudioMessage
        (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jobject requestID)
{
    std::string recvID = jstring2string(env, strRecvID);
    XUINT64 requestid = 0;
    jint errorcode = (jint)IM_SendAudioMessage(recvID.c_str(), (YIMChatType)chatType, &requestid);
//    YouMe_LOG_Debug("********requestid:%d\n", requestid);
    if (NULL == requestID)
        return 10000;
    jclass class_name = env->GetObjectClass(requestID);
    if (NULL == class_name)
        return 10000;
    jmethodID mid = env->GetMethodID(class_name, "setId", "(J)V");
    if (NULL == mid)
        return 10000;
    env->CallVoidMethod(requestID, mid, requestid);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendOnlyAudioMessage
        (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jobject requestID)
{
    XUINT64 requestid = 0;
	std::string recvID = jstring2string(env, strRecvID);
    jint errorcode = (jint)IM_SendOnlyAudioMessage(recvID.c_str(), (YIMChatType)chatType, &requestid);
    if (NULL == requestID)
        return 10000;
    jclass class_name = env->GetObjectClass(requestID);
    if (NULL == class_name)
        return 10000;
    jmethodID mid = env->GetMethodID(class_name, "setId", "(J)V");
    if (NULL == mid)
        return 10000;
    env->CallVoidMethod(requestID, mid, requestid);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StopAudioMessage
        (JNIEnv * env, jclass clazz, jstring strParam)
{
    std::string param = jstring2string(env, strParam);
    return (jint)IM_StopAudioMessage(param.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_CancleAudioMessage
        (JNIEnv * env, jclass clazz)
{
    return (jint)IM_CancleAudioMessage();
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DownloadAudioFile
        (JNIEnv * env, jclass clazz, jlong iSerial, jstring strSavePath)
{
    std::string path = jstring2string(env, strSavePath);
    return (jint)IM_DownloadFile((XUINT64)iSerial, path.c_str());
}


JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_JoinChatRoom
        (JNIEnv * env, jclass clazz, jstring strRoomID)
{
    std::string rooID = jstring2string(env, strRoomID);
    return (jint)IM_JoinChatRoom(rooID.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_LeaveChatRoom
        (JNIEnv * env, jclass clazz, jstring strRoomID)
{
    std::string rooID = jstring2string(env, strRoomID);
    return (jint)IM_LeaveChatRoom(rooID.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_LeaveAllChatRooms
(JNIEnv * env, jclass clazz)
{
    return (jint)IM_LeaveAllChatRooms();
}

JNIEXPORT jbyteArray JNICALL Java_com_youme_im_NativeEngine_GetMessage
        (JNIEnv * env, jclass clazz)
{
    const XCHAR* strMsg = IM_GetMessage();

    std::string nativeString = std::string( strMsg != NULL ? strMsg : "" ); // has a bit of a code smell, there is probably a better way.
    // cite: http://stackoverflow.com/questions/27303316/c-stdstring-to-jstring-with-a-fixed-length
    jbyteArray array = env->NewByteArray(nativeString.length());
    env->SetByteArrayRegion(array,0,nativeString.length(),(jbyte*)nativeString.c_str());
    IM_PopMessage(strMsg);
    return array;
}


JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_PopMessage
        (JNIEnv * env, jclass clazz)
{
   // IM_PopMessage();
}


JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetSDKVer
        (JNIEnv * env, jclass clazz)
{
    return (jint)IM_GetSDKVer();
}


JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_SetServerZone
        (JNIEnv *env, jclass clazz, jint zone)
{
    IM_SetServerZone((ServerZone)zone);
}


JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_SetMode
        (JNIEnv * env, jclass clazz, jint iMode)
{
    IM_SetMode((int)iMode);
}


JNIEXPORT jstring JNICALL Java_com_youme_im_NativeEngine_GetFilterText
  (JNIEnv * env, jclass clazz, jstring strSource, jobject level)
{
    std::string source = jstring2string(env, strSource);
	int nLevel = 0;
    XCHAR* strRes = IM_GetFilterText(source.c_str(), &nLevel);
	
	if (NULL != level)
	{
		jclass class_name = env->GetObjectClass(level);
		if (NULL != class_name)
		{
			jmethodID mid = env->GetMethodID(class_name, "setValue", "(I)V");
			if (NULL != mid)
			{
				env->CallVoidMethod(level, mid, nLevel);
			}			
		}
	}
	
    if( strRes!= nullptr){
        jstring filteredStr = string2jstring(env, strRes);
        IM_DestroyFilterText(strRes);
        return  filteredStr;
    }
	
    return string2jstring(env, "");
}


JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnPause
        (JNIEnv * evn, jclass clazz, jboolean pauseReceiveMessage){
    IM_OnPause(pauseReceiveMessage);
}

JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnResume
        (JNIEnv * env, jclass clazz){
    IM_OnResume();
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendGift
        (JNIEnv * env, jclass clazz, jstring strAnchorId, jstring strChannel, jint iGiftID, jint iGiftCount, jstring strExtParam, jobject requestID){
    XUINT64 requestid = 0;
    std::string anchorId = jstring2string(env, strAnchorId);
    std::string channel = jstring2string(env, strChannel);
    std::string extParam = jstring2string(env, strExtParam);
    jint errorcode = (jint)IM_SendGift(anchorId.c_str(),channel.c_str(),iGiftID,iGiftCount,extParam.c_str(),&requestid);
    if (NULL == requestID)
        return 10000;
    jclass class_name = env->GetObjectClass(requestID);
    if (NULL == class_name)
        return 10000;
    jmethodID mid = env->GetMethodID(class_name, "setId", "(J)V");
    if (NULL == mid)
        return 10000;
    env->CallVoidMethod(requestID, mid, requestid);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_MultiSendTextMessage
        (JNIEnv * env, jclass clazz, jstring strRecivers, jstring strContent){
    std::string recvID = jstring2string(env, strRecivers);
    std::string content = jstring2string(env, strContent);
    jint errorcode = (jint)IM_MultiSendTextMessage(recvID.c_str(),content.c_str());
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetHistoryContact
        (JNIEnv * env, jclass clazz){
    jint errorcode = (jint)IM_GetRecentContacts();
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StartAudioSpeech
        (JNIEnv * env, jclass clazz, jobject requestID, jboolean bTranslate){
    XUINT64 requestid = 0;
    jint errorcode = (jint)IM_StartAudioSpeech(&requestid,bTranslate);
    if (NULL == requestID)
        return 10000;
    jclass class_name = env->GetObjectClass(requestID);
    if (NULL == class_name)
        return 10000;
    jmethodID mid = env->GetMethodID(class_name, "setId", "(J)V");
    if (NULL == mid)
        return 10000;
    env->CallVoidMethod(requestID, mid, requestid);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StopAudioSpeech
        (JNIEnv * env, jclass clazz){
    jint errorcode = (jint)IM_StopAudioSpeech();
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_ConvertAMRToWav
        (JNIEnv * env, jclass clazz, jstring strSrcPath, jstring strSrcDestPath){
    std::string srcPath = jstring2string(env, strSrcPath);
    std::string destPath = jstring2string(env, strSrcDestPath);
    jint errorcode = (jint)IM_ConvertAMRToWav(srcPath.c_str(),destPath.c_str());
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryHistoryMessage
        (JNIEnv * env, jclass clazz, jstring strTargetID, jint iChatType, jlong iStartMessageID, jint iCount, jint iDirection){
    std::string targetID = jstring2string(env, strTargetID);
    jint errorcode = (jint)IM_QueryHistoryMessage(targetID.c_str(),iChatType,(XUINT64)iStartMessageID,iCount,iDirection);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteHistoryMessage
        (JNIEnv * env, jclass clazz, jint iChatType, jlong iTime){
    jint errorcode = (jint)IM_DeleteHistoryMessage((YIMChatType)iChatType,iTime);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteHistoryMessageByID(JNIEnv *env, jclass clazz, jlong iMessageID){
	jint errorcode = (jint)IM_DeleteHistoryMessageByID(iMessageID);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteSpecifiedHistoryMessage(JNIEnv *env, jclass clazz, jstring targetID, jint chatType, jlongArray excludeMesList) {
    std::string strTargetID = jstring2string(env, targetID);
    jlong *arr = env->GetLongArrayElements(excludeMesList,NULL);
    jint length = env->GetArrayLength(excludeMesList);
    return (jint)IM_DeleteSpecifiedHistoryMessage(strTargetID.c_str(), (YIMChatType)chatType, (XUINT64*)arr, length);
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteHistoryMessageByTarget
(JNIEnv *env, jclass clazz, jstring targetID, jint iChatType, jlong startMessageID, jint count) {
    std::string strTargetID = jstring2string(env, targetID);
    jint errorcode = (jint)IM_DeleteHistoryMessageByTarget(strTargetID.c_str(), (YIMChatType)iChatType, startMessageID, count);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryRoomHistoryMessageFromServer(JNIEnv * env, jclass clazz, jstring roomID, jint count, jint direction)
{
    std::string strRoomID = jstring2string(env, roomID);
    jint errorcode = (jint)IM_QueryRoomHistoryMessageFromServer(strRoomID.c_str(), count ,direction);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetDownloadAudioMessageSwitch(JNIEnv * env, jclass clazz, jboolean download){
    return (jint)IM_SetDownloadAudioMessageSwitch(download);     
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetReceiveMessageSwitch
  (JNIEnv * env, jclass clazz, jstring strTargets, jboolean bAutoRecv){
	std::string targets = jstring2string(env, strTargets);
    jint errorcode = (jint)IM_SetReceiveMessageSwitch(targets.c_str(), bAutoRecv);
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetNewMessage(JNIEnv * env, jclass clazz, jstring strTargets){
	std::string targets = jstring2string(env, strTargets);
    jint errorcode = (jint)IM_GetNewMessage(targets.c_str());
    return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SwitchMsgTransType(JNIEnv * env, jclass clazz, jint transType){
    jint errorcode = (jint)IM_SwitchMsgTransType((YIMMsgTransType)transType);
    return errorcode;
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetUserInfo
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetUserInfo
        (JNIEnv * env, jclass clazz, jstring userInfo){
    std::string strUserInfo = jstring2string(env, userInfo);
    return (jint)IM_SetUserInfo(strUserInfo.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetUserInfo
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetUserInfo
        (JNIEnv * env, jclass clazz, jstring userID){
    std::string strUserID = jstring2string(env, userID);
    return (jint)IM_GetUserInfo(strUserID.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetRoomHistoryMessageSwitch
 * Signature: (Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetRoomHistoryMessageSwitch
        (JNIEnv * env, jclass clazz, jstring roomID, jboolean save){
    std::string strRoomID = jstring2string(env, roomID);
    return (jint)IM_SetRoomHistoryMessageSwitch(strRoomID.c_str(),save);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    StartPlayAudio
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StartPlayAudio
        (JNIEnv * env, jclass clazz, jstring audioPath){
    std::string strAudioPath = jstring2string(env, audioPath);
    return (jint)IM_StartPlayAudio(strAudioPath.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    StopPlayAudio
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StopPlayAudio
        (JNIEnv * env, jclass clazz){
    return (jint)IM_StopPlayAudio();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    IsPlaying
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_youme_im_NativeEngine_IsPlaying
        (JNIEnv * env, jclass classz){
    return (jboolean)IM_IsPlaying();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetVolume
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_SetVolume
(JNIEnv *env, jclass classz, jfloat volume){
    IM_SetVolume(volume);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetAudioCachePath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_youme_im_NativeEngine_GetAudioCachePath
        (JNIEnv * env, jclass classz){
    jstring path = string2jstring(env,IM_GetAudioCachePath());
    return path;
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    ClearAudioCachePath
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_youme_im_NativeEngine_ClearAudioCachePath
        (JNIEnv * env, jclass classz){
    return IM_ClearAudioCachePath();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    QueryUserStatus
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryUserStatus
        (JNIEnv * env, jclass classz, jstring userID){
    std::string strUserID = jstring2string(env, userID);
    return (jint)IM_QueryUserStatus(strUserID.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    OnUpdateLocation
 * Signature: (DD)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnUpdateLocation(JNIEnv *env, jclass classz, jint errorcode, jdouble longitude, jdouble latitude)
{
	if (g_AndroidLocationLisen != NULL)
	{	
		g_AndroidLocationLisen->OnUpdateLocation((LocationErrorcode)errorcode, longitude, latitude);
	}
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    TranslateText
 * Signature: (Lcom/youme/im/IMEngine/IntegerVal;Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_TranslateText(JNIEnv *env, jclass classz, jobject requestID, jstring text, jint destLanguage, jint srcLanguage)
{
	std::string strText = jstring2string(env, text);
	unsigned int iRequestID = 0;
    jint errorcode = (jint) IM_TranslateText(&iRequestID, strText.c_str(), (LanguageCode)destLanguage, (LanguageCode)srcLanguage);
	if (NULL != requestID)
	{
		jclass class_name = env->GetObjectClass(requestID);
		if (NULL != class_name)
		{
			jmethodID mid = env->GetMethodID(class_name, "setValue", "(I)V");
			if (NULL != mid)
			{
				env->CallVoidMethod(requestID, mid, iRequestID);
			}			
		}
	}
	return errorcode;
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetCurrentLocation
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetCurrentLocation(JNIEnv *env, jclass classz)
{
	return (jint)IM_GetCurrentLocation();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetNearbyObjects
 * Signature: (ILjava/lang/String;IZ)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetNearbyObjects(JNIEnv *env, jclass classz, jint count, jstring serverAreaID, jint districtLevel, jboolean resetStartDistance)
{
	std::string strServerAreaID = jstring2string(env, serverAreaID);
	return (jint) IM_GetNearbyObjects(count, strServerAreaID.c_str(), (DistrictLevel)districtLevel, resetStartDistance);
}

JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetDistance(JNIEnv *env, jclass classz, jstring userID)
{
	std::string strUserID = jstring2string(env, userID);
	return (jint) IM_GetDistance(strUserID.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetUpdateInterval
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_SetUpdateInterval(JNIEnv *env, jclass classz, jint interval)
{
	IM_SetUpdateInterval(interval);
}


JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DownloadFileByURL
        (JNIEnv * env, jclass classz, jstring url, jstring savePath, jint fileType){
    std::string strURL = jstring2string(env, url);
    std::string strSavePath = jstring2string(env, savePath);
    return (jint) IM_DownloadFileByURL(strURL.c_str(), strSavePath.c_str(),(YIMFileType)fileType);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetDownloadDir
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetDownloadDir(JNIEnv * env, jclass classz, jstring path)
{
	std::string strPath = jstring2string(env, path);
	return (jint) IM_SetDownloadDir(strPath.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetMicrophoneStatus
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_GetMicrophoneStatus(JNIEnv *env, jclass classz)
{
	  IM_GetMicrophoneStatus();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetSpeechRecognizeLanguage
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetSpeechRecognizeLanguage(JNIEnv *env, jclass classz, jint language)
{
	IM_SetSpeechRecognizeLanguage(language);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetOnlyRecognizeSpeechText
 * Signature: (Ljava/lang/Boolean;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetOnlyRecognizeSpeechText(JNIEnv *env, jclass classz, jboolean recognition)
{
    IM_SetOnlyRecognizeSpeechText(recognition);
}


/*
 * Class:     com_youme_im_NativeEngine
 * Method:    Accusation
 * Signature: (Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_Accusation(JNIEnv *env, jclass classz, jstring userID, jint source, jint reason, jstring description, jstring extraParam)
{
	std::string strUserID = jstring2string(env, userID);
	std::string strDescription = jstring2string(env, description);
	std::string strExtraParam = jstring2string(env, extraParam);
	IM_Accusation(strUserID.c_str(), (YIMChatType)source, reason, strDescription.c_str(), strExtraParam.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    QueryNotice
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryNotice(JNIEnv *, jclass)
{
	return IM_QueryNotice();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetForbiddenSpeakInfo
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetForbiddenSpeakInfo(JNIEnv *, jclass){
    return IM_GetForbiddenSpeakInfo();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetRoomMemberCount
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetRoomMemberCount(JNIEnv *env, jclass classz, jstring roomID) {
	std::string strRoomID = jstring2string(env, roomID);
	return IM_GetRoomMemberCount(strRoomID.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    BlockUser
 * Signature: (Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_BlockUser(JNIEnv *env, jclass classz, jstring roomID, jboolean block) {
	std::string strRoomID = jstring2string(env, roomID);
	return IM_BlockUser(strRoomID.c_str(), block);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    UnBlockAllUser
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_UnBlockAllUser(JNIEnv *, jclass) {
	return IM_UnBlockAllUser();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetBlockUsers
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetBlockUsers(JNIEnv *, jclass) {
	return IM_GetBlockUsers();
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetMessageRead
 * Signature: (JZ)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetMessageRead
(JNIEnv *env, jclass classz, jlong messageID, jboolean read)
{
    return IM_SetMessageRead(messageID, read);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SendMessageReadStatus
 * Signature: (Ljava/lang/String;IJ)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendMessageReadStatus
(JNIEnv *env, jclass classz, jstring userId, jint chatType, jlong messageID)
{
    std::string strUserID = jstring2string(env, userId);
    return IM_SendMessageReadStatus(strUserID.c_str(), chatType, messageID);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetAllMessageRead
 * Signature: (Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetAllMessageRead
(JNIEnv *env, jclass classz, jstring userID, jboolean read)
{
    std::string strUserID = jstring2string(env, userID);
    return IM_SetAllMessageRead(strUserID.c_str(),read);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetVoiceMsgPlayed
 * Signature: (JZ)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetVoiceMsgPlayed
(JNIEnv *env, jclass classz, jlong messageID, jboolean played)
{
    return IM_SetVoiceMsgPlayed(messageID,played);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetUserProfileInfo
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetUserProfileInfo
(JNIEnv *env, jclass classz, jstring profileInfo)
{
    
    std::string strProfileInfo = jstring2string(env, profileInfo);
    return (jint)IM_SetUserProfileInfo(strProfileInfo.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetUserProfilePhoto
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetUserProfilePhoto
(JNIEnv *env, jclass classz, jstring photoPath)
{
    std::string strPhotoPath = jstring2string(env, photoPath);
    return (jint)IM_SetUserProfilePhoto(strPhotoPath.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetUserProfileInfo
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetUserProfileInfo
(JNIEnv *env, jclass classz, jstring userID)
{
    std::string strUserID = jstring2string(env, userID);
    return (jint)IM_GetUserProfileInfo(strUserID.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SwitchUserStatus
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SwitchUserStatus
(JNIEnv *env, jclass classz, jstring userID, jint userStatus)
{
    std::string strUserID = jstring2string(env, userID);
    return (jint)IM_SwitchUserStatus(strUserID.c_str(), (YIMUserStatus)userStatus);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetAddPermission
 * Signature: (ZI)V
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetAddPermission
(JNIEnv *env, jclass classz, jboolean beFound, jint beAddPermission)
{
    return (jint)IM_SetAddPermission(beFound, (IMUserBeAddPermission)beAddPermission);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    FindUser
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_FindUser
(JNIEnv *env, jclass classz, jint findType, jstring target)
{
    std::string strTarget = jstring2string(env, target);
    return (jint)IM_FindUser(findType, strTarget.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    RequestAddFriend
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_RequestAddFriend
(JNIEnv *env, jclass classz, jstring users, jstring comments)
{
    std::string strUsers = jstring2string(env, users);
    std::string strComments = jstring2string(env, comments);
    return (jint)IM_RequestAddFriend(strUsers.c_str(), strComments.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    DealBeRequestAddFriend
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DealBeRequestAddFriend
(JNIEnv *env, jclass classz, jstring userID, jint dealResult, jlong reqID)
{
    std::string strUserID = jstring2string(env, userID);
    return (jint)IM_DealAddFriend(strUserID.c_str(), dealResult,reqID);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    DeleteFriend
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteFriend
(JNIEnv *env, jclass classz, jstring users, jint deleteType)
{
    std::string strUsers = jstring2string(env, users);
    return (jint)IM_DeleteFriend(strUsers.c_str(), deleteType);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    BlackFriend
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_BlackFriend
(JNIEnv *env, jclass classz, jint type, jstring users)
{
    std::string strUsers = jstring2string(env, users);
    return (jint)IM_BlackFriend(type, strUsers.c_str());
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    QueryFriends
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryFriends
(JNIEnv *env, jclass classz, jint type, jint startIndex, jint count)
{
    return (jint)IM_QueryFriends(type, startIndex, count);
}

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    QueryFriendRequestList
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryFriendRequestList
(JNIEnv *env, jclass classz, jint startIndex, jint count)
{
    return (jint)IM_QueryFriendRequestList(startIndex, count);
}


JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetLoginAddress(JNIEnv *env, jclass classz, jstring ip, jint port)
{
	std::string strIP= jstring2string(env, ip);
    return IM_SetLoginAddress(strIP.c_str(),port);
}

#endif


void YouMe_SetAndroidSpeechListen(IAudioListener* pListen)
{
    g_AndroidSpeechListen = pListen;
}

void YouMe_SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    
    jniWrap.m_pThreadJni->CallStaticVoidMethod(s_voiceManager, m_setAudioRecordParam, sampleRate, channel, sampleBitSize);
}

void YouMe_SetAudioRecordCacheDir(const char* path)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    
    return jniWrap.m_pThreadJni->CallStaticVoidMethod (s_voiceManager, m_setAudioRecordCacheDir,string2jstring(jniWrap.m_pThreadJni, path));
}

std::string YouMe_GetAudioRecordCacheDir()
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return "";
    }
    
    jstring dir = (jstring)jniWrap.m_pThreadJni->CallStaticObjectMethod(s_voiceManager, m_getAudioRecordCacheDir);
	std::string strAudioRecordCacheDir = jstring2string(jniWrap.m_pThreadJni, dir);
	return strAudioRecordCacheDir;
}

void YouMe_InitSpeechRecognizer(const char* appkey, const char* secret, int audioRecognizeType)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    return jniWrap.m_pThreadJni->CallStaticVoidMethod (s_voiceManager, m_initSpeechRecognizer,string2jstring(jniWrap.m_pThreadJni,appkey), string2jstring(jniWrap.m_pThreadJni,secret), audioRecognizeType);
}

void Youme_SetRecognizeLanguage(int language)
{
	JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    jniWrap.m_pThreadJni->CallStaticVoidMethod(s_voiceManager, m_setRecognizeLanguage, language);
}

void Youme_UpdateToken(const char* token)
{
	JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    jniWrap.m_pThreadJni->CallStaticVoidMethod(s_voiceManager, m_updateToken, string2jstring(jniWrap.m_pThreadJni, token));
}

int YouMe_StartSpeech(XUINT64 serial)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return -1;
    }
    
    return jniWrap.m_pThreadJni->CallStaticIntMethod (s_voiceManager, m_startSpeech,serial);
    
}

int YouMe_StartOnlySpeech(XUINT64 serial)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return -1;
    }
    
    return jniWrap.m_pThreadJni->CallStaticIntMethod(s_voiceManager, m_startOnlySpeech, serial);
    
}

int YouMe_StopSpeech()
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return -1;
    }
    
    return jniWrap.m_pThreadJni->CallStaticIntMethod (s_voiceManager, m_stopSpeech);
    
}

int YouMe_CancleSpeech()
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return -1;
    }
    
    return jniWrap.m_pThreadJni->CallStaticIntMethod (s_voiceManager, m_cancleSpeech);
}

void YouMe_UnInit()
{
	JNIEvnWrap jniWrap;
    if (NULL != jniWrap.m_pThreadJni)
    {
        jniWrap.m_pThreadJni->CallStaticIntMethod (s_voiceManager, m_uninit);
    }
}

void YouMe_SetPlayVolume(float volume)
{
	JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    
    jniWrap.m_pThreadJni->CallStaticVoidMethod(s_voiceManager, m_setPlayVolume, volume);
}

int YouMe_StartPlayAudio(const char* pszPath)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return -1;
    }
    
    return jniWrap.m_pThreadJni->CallStaticIntMethod(s_voiceManager, m_startPlayAudio,string2jstring(jniWrap.m_pThreadJni,pszPath));
}

int YouMe_StopPlayAudio()
{
	JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return -1;
    }
    return jniWrap.m_pThreadJni->CallStaticIntMethod(s_voiceManager, m_stopPlayAudio);
}

bool YouMe_IsPlaying()
{
	JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return false;
    }
    return jniWrap.m_pThreadJni->CallStaticBooleanMethod (s_voiceManager, m_isPlaying);
}

int Youme_GetMicrophoneStatus()
{
	JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return 0;
    }
    return jniWrap.m_pThreadJni->CallStaticIntMethod(s_voiceManager, m_getMicrophoneStatus);
}



int GetNetworkType()
{
	JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return 0;
    }
    return jniWrap.m_pThreadJni->CallStaticIntMethod (s_voiceManager, m_getNetworkType);
}



void YouMe_SetAndroidLocationListen(ILocationListen* listen)
{
    g_AndroidLocationLisen = listen;
}

int Youme_GetCurrentLocation()
{
	JNIEvnWrap jniWrap;
	if (NULL == jniWrap.m_pThreadJni)
	{
		return -1;
	}
	return jniWrap.m_pThreadJni->CallStaticIntMethod(mYouGeoLocationClass, m_YoumeGetGeographyLocation);
}

void YouMe_ResizeImage(std::string srcImagePath, std::string resizedSavePath)
{
    /*JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    jniWrap.m_pThreadJni->CallStaticVoidMethod(mDealImageClass, m_resizeImage, string2jstring(jniWrap.m_pThreadJni, srcImagePath.c_str()), string2jstring(jniWrap.m_pThreadJni, resizedSavePath.c_str()));*/
}