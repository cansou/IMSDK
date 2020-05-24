﻿/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_youme_im_NativeEngine */

#ifndef _Included_com_youme_im_NativeEngine
#define _Included_com_youme_im_NativeEngine
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setModel
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setModel
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setSysVersion
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setSysVersion
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setDeviceIMEI
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setDeviceIMEI
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setCPUArch
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setCPUArch
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setPackageName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setPackageName
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setDocumentPath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setDocumentPath
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setCachePath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setCachePath
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    onNetWorkChanged
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_onNetWorkChanged
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setBrand
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setBrand
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setCPUChip
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setCPUChip
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    WriteLog
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_WriteLog
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    OnRecordFinish
 * Signature: (ILjava/lang/String;JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnRecordFinish
  (JNIEnv *, jclass, jint, jstring, jlong, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    OnRecordData
 * Signature: ([BIZ)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_youme_im_NativeEngine_OnRecordData
  (JNIEnv *, jclass, jbyteArray, jint, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    OnPlayFinish
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnPlayFinish
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    OnPlayData
 * Signature: ([BI)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnPlayData
  (JNIEnv *, jclass, jbyteArray, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    Init
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_Init
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    Uninit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_Uninit
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    setUpdateReadStatusCallbackFlag
 * Signature: ()Z
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_setUpdateReadStatusCallbackFlag(JNIEnv *, jclass, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    Login
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_Login
  (JNIEnv *, jclass, jstring, jstring, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    Logout
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_Logout
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SendTextMessage
 * Signature: (Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Lcom/youme/im/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendTextMessage
  (JNIEnv *, jclass, jstring, jint, jstring, jstring, jobject);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SendCustomMessage
 * Signature: (Ljava/lang/String;I[BILcom/youme/im/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendCustomMessage
  (JNIEnv *, jclass, jstring, jint, jbyteArray, jint, jobject);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SendFile
 * Signature: (Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;ILcom/youme/im/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendFile
  (JNIEnv *, jclass, jstring, jint, jstring, jstring, jint, jobject);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SendAudioMessage
 * Signature: (Ljava/lang/String;ILcom/youme/im/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendAudioMessage
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SendOnlyAudioMessage
 * Signature: (Ljava/lang/String;ILcom/youme/im/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendOnlyAudioMessage
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    StopAudioMessage
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StopAudioMessage
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    CancleAudioMessage
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_CancleAudioMessage
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    DownloadAudioFile
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DownloadAudioFile
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    JoinChatRoom
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_JoinChatRoom
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    LeaveChatRoom
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_LeaveChatRoom
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    LeaveAllChatRooms
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_LeaveAllChatRooms
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetMessage
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_youme_im_NativeEngine_GetMessage
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    PopMessage
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_PopMessage
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetSDKVer
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetSDKVer
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetServerZone
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_SetServerZone
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_SetMode
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetFilterText
 * Signature: (Ljava/lang/String;Lcom/youme/im/IMEngine/IntegerVal;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_youme_im_NativeEngine_GetFilterText
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    OnPause
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnPause
  (JNIEnv *, jclass, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    OnResume
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnResume
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SendGift
 * Signature: (Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;Lcom/youme/im/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendGift
  (JNIEnv *, jclass, jstring, jstring, jint, jint, jstring, jobject);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    MultiSendTextMessage
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_MultiSendTextMessage
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetHistoryContact
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetHistoryContact
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    StartAudioSpeech
 * Signature: (Lcom/youme/im/IMEngine/MessageRequestId;Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StartAudioSpeech
  (JNIEnv *, jclass, jobject, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    StopAudioSpeech
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StopAudioSpeech
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    ConvertAMRToWav
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_ConvertAMRToWav
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    QueryHistoryMessage
 * Signature: (Ljava/lang/String;IJII)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryHistoryMessage
  (JNIEnv *, jclass, jstring, jint, jlong, jint, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    DeleteHistoryMessage
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteHistoryMessage
  (JNIEnv *, jclass, jint, jlong);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    DeleteHistoryMessageByID
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteHistoryMessageByID
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    DeleteSpecifiedHistoryMessage
 * Signature: (Ljava/lang/String;I[J)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteSpecifiedHistoryMessage
  (JNIEnv *, jclass, jstring, jint, jlongArray);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    DeleteHistoryMessageByTarget
 * Signature: (Ljava/lang/String;IJI)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteHistoryMessageByTarget
  (JNIEnv *, jclass, jstring, jint, jlong, jint);

 /*
 * Class:     com_youme_im_NativeEngine
 * Method:    QueryRoomHistoryMessageFromServer
 * Signature: (Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryRoomHistoryMessageFromServer
  (JNIEnv *, jclass, jstring, jint, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetDownloadAudioMessageSwitch
 * Signature: (Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetDownloadAudioMessageSwitch
  (JNIEnv *, jclass, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetReceiveMessageSwitch
 * Signature: (Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetReceiveMessageSwitch
  (JNIEnv *, jclass, jstring, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetNewMessage
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetNewMessage
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine_SwitchMsgTransType
 * Method:    SwitchMsgTransType
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SwitchMsgTransType
(JNIEnv * , jclass , jint );

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetUserInfo
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetUserInfo
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetUserInfo
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetUserInfo
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetRoomHistoryMessageSwitch
 * Signature: (Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetRoomHistoryMessageSwitch
  (JNIEnv *, jclass, jstring, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    StartPlayAudio
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StartPlayAudio
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    StopPlayAudio
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_StopPlayAudio
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    IsPlaying
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_youme_im_NativeEngine_IsPlaying
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetVolume
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_SetVolume
  (JNIEnv *, jclass, jfloat);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetAudioCachePath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_youme_im_NativeEngine_GetAudioCachePath
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    ClearAudioCachePath
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_youme_im_NativeEngine_ClearAudioCachePath
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    QueryUserStatus
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryUserStatus
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    OnUpdateLocation
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_OnUpdateLocation
  (JNIEnv *, jclass, jint, jdouble, jdouble);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    TranslateText
 * Signature: (Lcom/youme/im/IMEngine/IntegerVal;Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_TranslateText
  (JNIEnv *, jclass, jobject, jstring, jint, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetCurrentLocation
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetCurrentLocation
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetNearbyObjects
 * Signature: (ILjava/lang/String;IZ)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetNearbyObjects
  (JNIEnv *, jclass, jint, jstring, jint, jboolean);
  
/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetDistance
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetDistance
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetUpdateInterval
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_SetUpdateInterval
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    DownloadFileByURL
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DownloadFileByURL
  (JNIEnv *, jclass, jstring, jstring, jint fileType);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetDownloadDir
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetDownloadDir
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetMicrophoneStatus
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_GetMicrophoneStatus
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetSpeechRecognizeLanguage
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetSpeechRecognizeLanguage
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    SetOnlyRecognizeSpeechText
 * Signature: (Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetOnlyRecognizeSpeechText
  (JNIEnv *, jclass, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    Accusation
 * Signature: (Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_Accusation
  (JNIEnv *, jclass, jstring, jint, jint, jstring, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    QueryNotice
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryNotice
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetForbiddenSpeakInfo
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetForbiddenSpeakInfo
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetRoomMemberCount
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetRoomMemberCount
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    BlockUser
 * Signature: (Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_BlockUser
  (JNIEnv *, jclass, jstring, jboolean);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    UnBlockAllUser
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_UnBlockAllUser
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    GetBlockUsers
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetBlockUsers
  (JNIEnv *, jclass);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    SetMessageRead
     * Signature: (JZ)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetMessageRead
    (JNIEnv *, jclass, jlong, jboolean);

/*
* Class:     com_youme_im_NativeEngine
* Method:    SendMessageReadStatus
* Signature: (Ljava/lang/String;IJ)I
*/
JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SendMessageReadStatus
  (JNIEnv *env, jclass classz, jstring userId, jint chatType, jlong messageID);

/*
 * Class:     com_youme_im_NativeEngine
 * Method:    NotifyVolumeChange
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_youme_im_NativeEngine_NotifyVolumeChange
  (JNIEnv *, jclass, jint, jint);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    SetUserProfileInfo2
     * Signature: (Ljava/lang/String;)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetUserProfileInfo
    (JNIEnv *, jclass, jstring);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    SetUserProfilePhoto
     * Signature: (Ljava/lang/String;)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetUserProfilePhoto
    (JNIEnv *, jclass, jstring);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    GetUserProfileInfo
     * Signature: (Ljava/lang/String;)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_GetUserProfileInfo
    (JNIEnv *, jclass, jstring);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    SwitchUserStatus
     * Signature: (Ljava/lang/String;I)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SwitchUserStatus
    (JNIEnv *, jclass, jstring, jint);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    SetAddPermission
     * Signature: (ZI)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetAddPermission
    (JNIEnv *, jclass, jboolean, jint);

	
	/*
	 * Class:     com_youme_im_NativeEngine
	 * Method:    SetLoginAddress
	 * Signature: (Ljava/lang/String;I)I
	 */
	JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetLoginAddress
	  (JNIEnv *, jclass, jstring, jint);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    FindUser
     * Signature: (ILjava/lang/String;)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_FindUser
    (JNIEnv *, jclass, jint, jstring);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    RequestAddFriend
     * Signature: (Ljava/lang/String;Ljava/lang/String;)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_RequestAddFriend
    (JNIEnv *, jclass, jstring, jstring);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    DealBeRequestAddFriend
     * Signature: (Ljava/lang/String;I)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DealBeRequestAddFriend
    (JNIEnv *, jclass, jstring, jint);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    DeleteFriend
     * Signature: (Ljava/lang/String;I)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_DeleteFriend
    (JNIEnv *, jclass, jstring, jint);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    BlackFriend
     * Signature: (ILjava/lang/String;)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_BlackFriend
    (JNIEnv *, jclass, jint, jstring);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    QueryFriends
     * Signature: (III)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryFriends
    (JNIEnv *, jclass, jint, jint, jint);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    QueryFriendRequestList
     * Signature: (II)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_QueryFriendRequestList
    (JNIEnv *, jclass, jint, jint);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    SetAllMessageRead
     * Signature: (Ljava/lang/String;Z)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetAllMessageRead
    (JNIEnv *, jclass, jstring, jboolean);
    
    /*
     * Class:     com_youme_im_NativeEngine
     * Method:    SetVoiceMsgPlayed
     * Signature: (JZ)I
     */
    JNIEXPORT jint JNICALL Java_com_youme_im_NativeEngine_SetVoiceMsgPlayed
    (JNIEnv *, jclass, jlong, jboolean);
    

#ifdef __cplusplus
}
#endif
#endif
