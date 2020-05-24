#include <YouMeIMEngine/pb/youme_query_userinfo.pb.h>
#include <YouMeIMEngine/pb/youme_update_userinfo.pb.h>
#include <YouMeIMEngine/pb/youme_update_userstatus.pb.h>
#include <YouMeIMEngine/pb/youme_upload_token.pb.h>

#include <YouMeCommon/json/json.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/DownloadUploadManager.h>
#include "YouMeIMCommonDef.h"

#include <YouMeIMEngine/YouMeIMUserInfoManager.h>

#define NICK_NAME_MAX_LEN 64
#define SIGNATURE_MAX_LEN 120
#define PHOTO_URL_MAX_LEN 500
// photo max size 100kb
#define PHOTO_MAX_SIZE 100

YouMeIMUserInfoManager::YouMeIMUserInfoManager(YouMeIMManager *pIManager):IManagerBase(pIManager)
,m_uploadImage(false)
,hasUploadSuccess(false)
{
    m_pCallback = NULL;
    m_curUserInfo.userID = pIManager->m_strCurUserID;
    m_bExitUpload = false;
    m_hasSetPhotoUrl = false;
    m_uploadThread = std::thread(&YouMeIMUserInfoManager::UploadThreadProc, this);
    m_curQueryUserID = pIManager->m_strCurUserID;
}

YouMeIMUserInfoManager::~YouMeIMUserInfoManager()
{
    {
        std::lock_guard<std::mutex> lock(m_uploadInfoLock);
        m_imageUploadMap.clear();
    }
    if (m_uploadThread.joinable())
    {
        m_bExitUpload = true;
        m_uploadSemaphore.Increment();
        m_uploadThread.join();
    }
	StopUserStatusHeartbeat();
    m_pCallback = NULL;
    m_hasSetPhotoUrl = false;
    m_uploadImage = false;
}

void YouMeIMUserInfoManager::SetCallback(IYIMUserProfileCallback *pCallback)
{
    m_pCallback = pCallback;
}

YIMErrorcode YouMeIMUserInfoManager::RequestUploadToken(ImageUploadInfo& uploadInfo)
{
    XString srtFileMD5 = youmecommon::CCryptUtil::MD5File(uploadInfo.localPath);
    YOUMEServiceProtocol::GetUploadTokenReq req;
    req.set_version(PROTOCOL_VERSION_HTTPS);
    req.set_file_size(uploadInfo.fileSize);
    req.set_file_md5(XStringToUTF8(srtFileMD5));
    req.set_file_suffix(XStringToUTF8(uploadInfo.extension));
    std::string strData;
    req.SerializeToString(&strData);
    XUINT64 msgSerial = 0;
    std::map<std::string, std::string> extend;
    extend["uploadSerial"] = CStringUtilT<char>::to_string(uploadInfo.msgSerial);
    if (m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_UPLOAD_TOKEN, strData.c_str(), (int)strData.length(), extend, msgSerial))
    {
        return YIMErrorcode_Success;
    }
    return YIMErrorcode_NetError;
}

YIMErrorcode YouMeIMUserInfoManager::SetUserProfileInfo(const IMUserSettingInfo &userSettingInfo)
{
    YouMe_LOG_Info(__XT("enter"));
    
    if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
    {
        return YIMErrorcode_NotLogin;
    }
    
    IMUserSettingInfo curUserSettingInfo = m_curUserInfo.settingInfo;
    YOUMEServiceProtocol::UpdateRcUserInfoReq req;
    req.set_version(PROTOCOL_VERSION);
    if (userSettingInfo.nickName != curUserSettingInfo.nickName)
    {
        if (userSettingInfo.nickName.length() > NICK_NAME_MAX_LEN)
        {
            return YIMErrorcode_NickNameTooLong;
        }
        req.set_nickname(XStringToUTF8(userSettingInfo.nickName));
    }
    if (userSettingInfo.sex != curUserSettingInfo.sex)
    {
        req.set_gender(userSettingInfo.sex);
    }
    if (userSettingInfo.personalSignature != curUserSettingInfo.personalSignature)
    {
        if (userSettingInfo.personalSignature.length() > SIGNATURE_MAX_LEN)
        {
            return YIMErrorcode_SignatureTooLong;
        }
        req.set_signature(XStringToUTF8(userSettingInfo.personalSignature));
    }
    if (userSettingInfo.country != curUserSettingInfo.country)
    {
        req.set_country(XStringToUTF8(userSettingInfo.country));
    }
    if (userSettingInfo.province != curUserSettingInfo.province)
    {
        req.set_province(XStringToUTF8(userSettingInfo.province));
    }
    if (userSettingInfo.city != curUserSettingInfo.city)
    {
        req.set_city(XStringToUTF8(userSettingInfo.city));
    }
    
    if (userSettingInfo.extraInfo != curUserSettingInfo.extraInfo)
    {
        req.set_extand_json(XStringToUTF8(userSettingInfo.extraInfo));
    }  
    
    std::string strData;
    req.SerializeToString(&strData);
    XUINT64 msgSerail = 0;
    std::map<std::string, std::string> extend;
    if (! m_pIManager->SendData(YOUMEServiceProtocol::CMD_UPDATE_USER_PROFILE, strData.c_str(), (int)strData.length(), extend, msgSerail))
    {
        YouMe_LOG_Error(__XT("send update user profile failed"));
        if (m_pCallback != NULL)
        {
            m_pCallback->OnSetUserInfo(YIMErrorcode_NetError);
        }
    }
    std::lock_guard<std::mutex> lock(m_updateUserInfoMutex);
    m_updateUserInfo = m_curUserInfo;
    m_updateUserInfo.settingInfo = userSettingInfo;
    YouMe_LOG_Info(__XT("Leave"));
    return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMUserInfoManager::SetUserProfilePhoto(const XCHAR* photoPath)
{
    YouMe_LOG_Info(__XT("enter"));
    if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
    {
        return YIMErrorcode_NotLogin;
    }
    
    if (XStrLen(photoPath) > PHOTO_URL_MAX_LEN)
    {
        return YIMErrorcode_PhotoUrlTooLong;
    }
    
    youmecommon::CXFile file;
    if (file.LoadFile(photoPath, youmecommon::CXFile::Mode_OpenExist_ReadOnly) != 0)
    {
        YouMe_LOG_Error(__XT("image file not exist %s"), photoPath);
        return YIMErrorcode_FileNotExist;
    }
    unsigned int iFileSize = static_cast<unsigned int>(file.GetFileSize());
    file.Close();
    
    unsigned int imageSize = (int)(iFileSize/1024)+1;
    if (imageSize > PHOTO_MAX_SIZE)
    {
        return YIMErrorcode_PhotoSizeTooLarge;
    }
    
    XString strExtension = youmecommon::CXFile::GetFileExt(photoPath);
    
    XUINT64 ullSerial = m_pIManager->GetUniqueSerial();
    std::shared_ptr<ImageUploadInfo> pUploadInfo(new ImageUploadInfo);
    pUploadInfo->msgSerial = ullSerial;    
    pUploadInfo->fileType = FileType_Image;
    pUploadInfo->localPath = photoPath;
    pUploadInfo->fileSize = iFileSize;
    pUploadInfo->extension = strExtension;
    
    {
        std::lock_guard<std::mutex> lock(m_uploadInfoLock);
        m_imageUploadMap[ullSerial] = pUploadInfo;
    }
    
    YIMErrorcode errorCode = RequestUploadToken(*pUploadInfo);
    if (errorCode == YIMErrorcode_Success)
    {
        m_uploadImage = true;
    }else{
        std::map<XUINT64, std::shared_ptr<ImageUploadInfo> >::iterator itr = m_imageUploadMap.find(ullSerial);
        if (itr != m_imageUploadMap.end())
        {
            m_imageUploadMap.erase(itr);
        }
    }
    
    YouMe_LOG_Info(__XT("Leave"));
    return errorCode;
}

YIMErrorcode YouMeIMUserInfoManager::GetUserProfileInfo(const XCHAR *userID)
{
    YouMe_LOG_Info(__XT("enter"));
    if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
    {
        return YIMErrorcode_NotLogin;
    }
    
    YOUMEServiceProtocol::GetRcUserInfoReq req;
    req.set_version(PROTOCOL_VERSION);
    XString userid = XString(userID);
    if (userid.empty())
    {
        req.set_user_id(XStringToUTF8(m_pIManager->m_strCurUserID));
    }else{
        req.set_user_id(XStringToUTF8(userid));
        m_curQueryUserID = userid;
    }

    std::string strData;
    req.SerializeToString(&strData);
    XUINT64 msgSerail = 0;
    std::map<std::string, std::string> extend;
    if (! m_pIManager->SendData(YOUMEServiceProtocol::CMD_QUERY_USER_PROFILE, strData.c_str(), (int)strData.length(), extend, msgSerail))
    {
        YouMe_LOG_Error(__XT("send query user profile failed"));
        return YIMErrorcode_NetError;
    }
    YouMe_LOG_Info(__XT("Leave"));
    return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMUserInfoManager::SwitchUserStatus(const XCHAR *userID, YIMUserStatus userStatus)
{
    YouMe_LOG_Info(__XT("Enter SwitchUserStatus, userID: %s, userStatus: %d"),userID,userStatus);
    if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
    {
        return YIMErrorcode_NotLogin;
    }
    if (userStatus == m_curUserInfo.onlineState) {
        return YIMErrorcode_TheSameParam;
    }
    
    YOUMEServiceProtocol::UpdateRcUserOnlineStateReq req;
    req.set_version(PROTOCOL_VERSION);
    
    YOUMEServiceProtocol::RC_ONLINE_STATE state = YOUMEServiceProtocol::RC_ONLINE_STATE::RC_ONLINE;
    if (userStatus == STATUS_ONLINE) {
        state = YOUMEServiceProtocol::RC_ONLINE_STATE::RC_ONLINE;
    }else if (userStatus == STATUS_OFFLINE){
        state = YOUMEServiceProtocol::RC_ONLINE_STATE::RC_OFFLINE;
    }else if (userStatus == STATUS_INVISIBLE){
        state = YOUMEServiceProtocol::RC_ONLINE_STATE::RC_HIDDEN;
    }
    
    req.set_online_state(state);
    
    std::string strData;
    req.SerializeToString(&strData);
    XUINT64 msgSerail = 0;
    std::map<std::string, std::string> extend;
    if (! m_pIManager->SendData(YOUMEServiceProtocol::CMD_UPDATE_ONLINE_STATUS, strData.c_str(), (int)strData.length(), extend, msgSerail))
    {
        YouMe_LOG_Error(__XT("send update user online state failed"));
        return YIMErrorcode_NetError;
    }
    std::lock_guard<std::mutex> lock(m_updateUserInfoMutex);
    m_updateUserInfo = m_curUserInfo;
    m_updateUserInfo.onlineState = userStatus;
    YouMe_LOG_Info(__XT("Leave"));
    return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMUserInfoManager::SetAddPermission(bool beFound, IMUserBeAddPermission beAddPermission)
{
    YouMe_LOG_Info(__XT("enter SetAddPermission,beFound: %d, beAddPermission: %d"),beFound, beAddPermission);
    if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
    {
        return YIMErrorcode_NotLogin;
    }
    IMUserFoundPermission tmp;
    if (beFound) {
        tmp = CAN_BE_FOUND;
    }else{
        tmp = CAN_NOT_BE_FOUND;
    }
    
    if ((m_curUserInfo.foundPermission == tmp) && (m_curUserInfo.beAddPermission == beAddPermission))
    {
        return YIMErrorcode_TheSameParam;
    }
    YOUMEServiceProtocol::UpdateRcUserInfoReq req;
    req.set_version(PROTOCOL_VERSION);
    int privacy = 0;
    if (beFound)
    {
        //低字节低4位表示查找显示权限：1-显示，2-不显示
        //低字节高4位表示好友添加权限：1-不允许添加，2-需要验证，3-允许添加不需要验证
        switch (beAddPermission)
        {
            case NOT_ALLOW_ADD:
                privacy = (1 << 4) + 1;
                break;
            case NEED_VALIDATE:
                privacy = (2 << 4) + 1;
                break;
            case NO_ADD_PERMISSION:
                privacy = (3 << 4) + 1;
            default:
                break;
        }
    }else{
        switch (beAddPermission)
        {
            case NOT_ALLOW_ADD:
                privacy = (1 << 4) + 2;
                break;
            case NEED_VALIDATE:
                privacy = (2 << 4) + 2;
                break;
            case NO_ADD_PERMISSION:
                privacy = (3 << 4) + 2;
            default:
                break;
        }
    }
    
    req.set_privacy(privacy);
    
    std::string strData;
    req.SerializeToString(&strData);
    XUINT64 msgSerail = 0;
    std::map<std::string, std::string> extend;
    if (! m_pIManager->SendData(YOUMEServiceProtocol::CMD_UPDATE_USER_PROFILE, strData.c_str(), (int)strData.length(), extend, msgSerail))
    {
        YouMe_LOG_Error(__XT("send update user profile failed"));
        return YIMErrorcode_NetError;
    }
    std::lock_guard<std::mutex> lock(m_updateUserInfoMutex);
    m_updateUserInfo = m_curUserInfo;
    m_updateUserInfo.beAddPermission = beAddPermission;
    if (beFound) {
        m_updateUserInfo.foundPermission = CAN_BE_FOUND;
    }else{
        m_updateUserInfo.foundPermission = CAN_NOT_BE_FOUND;
    }
    YouMe_LOG_Info(__XT("Leave"));
    return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMUserInfoManager::SetPhotoUrl(XString photoUrl)
{
    YouMe_LOG_Info(__XT("Enter"));
    if (photoUrl != m_curUserInfo.photoURL)
    {
        YOUMEServiceProtocol::UpdateRcUserInfoReq req;
        req.set_version(PROTOCOL_VERSION);
        req.set_avatar(XStringToUTF8(photoUrl));
        
        std::string strData;
        req.SerializeToString(&strData);
        XUINT64 msgSerail = 0;
        std::map<std::string, std::string> extend;
        if (! m_pIManager->SendData(YOUMEServiceProtocol::CMD_UPDATE_USER_PROFILE, strData.c_str(), (int)strData.length(), extend, msgSerail))
        {
            YouMe_LOG_Error(__XT("set photoUrl,send update user profile failed"));
            return YIMErrorcode_NetError;
        }
    }else{
        return YIMErrorcode_TheSameParam;
    }
    std::lock_guard<std::mutex> lock(m_updateUserInfoMutex);
    m_updateUserInfo = m_curUserInfo;
    m_updateUserInfo.photoURL = photoUrl;
    
    std::lock_guard<std::mutex> lockSet(m_setPhotoUrlMutex);
    m_hasSetPhotoUrl = true;
    YouMe_LOG_Info(__XT("Leave"));
    return YIMErrorcode_Success;
}

void YouMeIMUserInfoManager::OnRecvPacket(ServerPacket& serverPacket)
{
    switch (serverPacket.commondID)
    {
        case YOUMEServiceProtocol::CMD_UPDATE_USER_PROFILE:
        {
            OnUpdateRcUserInfoRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_QUERY_USER_PROFILE:
        {
            OnGetRcUserInfoRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_UPDATE_ONLINE_STATUS:
        {
            OnUpdateRcUserOnlineStateRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_GET_UPLOAD_TOKEN:
        {
            m_uploadImage = false;
            OnGetImageUploadTokenRsp(serverPacket);
        }
            break;
		case YOUMEServiceProtocol::CMD_HXR_USER_INFO_CHANGE_NOTIFY:
		{
			OnUserInfoChangeNotify(serverPacket);
		}
			break;
        default:
            break;
    }
}

void YouMeIMUserInfoManager::OnGetRcUserInfoRsp(ServerPacket &serverPacket)
{
    YouMe_LOG_Info(__XT("enter"));
    if (NULL == m_pCallback)
    {
        YouMe_LOG_Error(__XT("user info callback is null"));
        return;
    }
    
    YIMErrorcode errorCode = YIMErrorcode_Success;
    IMUserProfileInfo userInfo;
    if (serverPacket.result == -1)
    {
        errorCode = YIMErrorcode_TimeOut;
        userInfo.userID = m_curQueryUserID;
        YouMe_LOG_Warning(__XT("query user info timeout"));
    }
    else
    {
        YOUMEServiceProtocol::GetRcUserInfoRsp rsp;
        if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
        {
            YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
            errorCode = YIMErrorcode_ServerError;
            userInfo.userID = m_curQueryUserID;
        }
        else
        {
            int nRet = rsp.ret();
            if (nRet == 0)
            {
                YouMe_LOG_Info(__XT("query user info success"));
                YOUMEServiceProtocol::RcUserInfo userProfile = rsp.user_info();
                userInfo.userID = UTF8TOXString(userProfile.user_id());
				userInfo.settingInfo.nickName = UTF8TOXString(userProfile.nickname());
                if (userProfile.gender() == 0) {
                    userInfo.settingInfo.sex = SEX_UNKNOWN;
                }else if (userProfile.gender() == 1){
                    userInfo.settingInfo.sex = SEX_MALE;
                }else {
                    userInfo.settingInfo.sex = SEX_FEMALE;
                }
				userInfo.photoURL = UTF8TOXString(userProfile.avatar());
				userInfo.settingInfo.personalSignature = UTF8TOXString(userProfile.signature());
				userInfo.settingInfo.country = UTF8TOXString(userProfile.country());
				userInfo.settingInfo.province = UTF8TOXString(userProfile.province());
				userInfo.settingInfo.city = UTF8TOXString(userProfile.city());
                
                int privacy = userProfile.privacy();
                switch (privacy)
                {
                    case 17:
                        userInfo.beAddPermission = NOT_ALLOW_ADD;
                        userInfo.foundPermission = CAN_BE_FOUND;
                        break;
                    case 18:
                        userInfo.beAddPermission = NOT_ALLOW_ADD;
                        userInfo.foundPermission = CAN_NOT_BE_FOUND;
                        break;
                    case 33:
                        userInfo.beAddPermission = NEED_VALIDATE;
                        userInfo.foundPermission = CAN_BE_FOUND;
                        break;
                    case 34:
                        userInfo.beAddPermission = NEED_VALIDATE;
                        userInfo.foundPermission = CAN_NOT_BE_FOUND;
                        break;
                    case 49:
                        userInfo.beAddPermission = NO_ADD_PERMISSION;
                        userInfo.foundPermission = CAN_BE_FOUND;
                        break;
                    case 50:
                        userInfo.beAddPermission = NO_ADD_PERMISSION;
                        userInfo.foundPermission = CAN_NOT_BE_FOUND;
                        break;
                    default:
                        break;
                }
                YOUMEServiceProtocol::RC_ONLINE_STATE state = userProfile.online_state();

                if (state == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_ONLINE) {
                    userInfo.onlineState = STATUS_ONLINE;
                }else if (state == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_OFFLINE){
                    userInfo.onlineState = STATUS_OFFLINE;
                }else if (state == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_HIDDEN){
                    userInfo.onlineState = STATUS_INVISIBLE;
                }
				userInfo.settingInfo.extraInfo = UTF8TOXString(userProfile.extand_json());
                
                std::lock_guard<std::mutex> lock(m_userInfoMutex);
                m_curUserInfo = userInfo;
                
                YouMe_LOG_Info(__XT("OnGetRcUserInfoRsp，userID: %s, onlineState: %d, nickname: %s, sex: %d, signature: %s, country: %s, city: %s, photoUrl: %s, addPermission:%d, showPermission:%d"),userInfo.userID.c_str(),userInfo.onlineState,userInfo.settingInfo.nickName.c_str(),userInfo.settingInfo.sex,userInfo.settingInfo.personalSignature.c_str(),userInfo.settingInfo.country.c_str(),userInfo.settingInfo.city.c_str(),userInfo.photoURL.c_str(),userInfo.beAddPermission,userInfo.foundPermission);
            }
            else
            {
                YouMe_LOG_Error(__XT("query user info error(%d)"), nRet);
                if(nRet == YOUMEServiceProtocol::EC_USER_HAS_NOT_REGISTER)
                {
                    errorCode = YIMErrorcode_HasNotRegisterUserInfo;
                }
                else
                {
                    errorCode = YIMErrorcode_QueryUserInfoFail;
                }
                userInfo.userID = m_curQueryUserID;
                userInfo.onlineState = STATUS_OFFLINE;
            }
        }
    }
    m_pCallback->OnQueryUserInfo(errorCode,userInfo);
}

void YouMeIMUserInfoManager::OnUpdateRcUserInfoRsp(ServerPacket &serverPacket)
{
    YouMe_LOG_Info(__XT("enter"));
    if (NULL == m_pCallback)
    {
        YouMe_LOG_Error(__XT("user info callback is null"));
        return;
    }
    
    YIMErrorcode errorCode = YIMErrorcode_Success;
    if (serverPacket.result == -1)
    {
        errorCode = YIMErrorcode_TimeOut;
        YouMe_LOG_Warning(__XT("set user info timeout"));
    }
    else
    {
        YOUMEServiceProtocol::UpdateRcUserInfoRsp rsp;
        if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
        {
            YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
            errorCode = YIMErrorcode_ServerError;
        }
        else
        {
            int nRet = rsp.ret();
            if (nRet == 0)
            {
                YouMe_LOG_Info(__XT("set user info success"));
                std::lock_guard<std::mutex> lock(m_userInfoMutex);
                m_curUserInfo = m_updateUserInfo;
                YouMe_LOG_Info(__XT("OnUpdateRcUserInfoRsp，userID: %s, onlineState: %d, nickname: %s, sex: %d, signature: %s, country: %s, city: %s, photoUrl: %s, addPermission:%d, showPermission:%d"),m_curUserInfo.userID.c_str(),m_curUserInfo.onlineState,m_curUserInfo.settingInfo.nickName.c_str(),m_curUserInfo.settingInfo.sex,m_curUserInfo.settingInfo.personalSignature.c_str(),m_curUserInfo.settingInfo.country.c_str(),m_curUserInfo.settingInfo.city.c_str(),m_curUserInfo.photoURL.c_str(),m_curUserInfo.beAddPermission,m_curUserInfo.foundPermission);
            }
            else
            {
                YouMe_LOG_Error(__XT("set user info error(%d)"), nRet);
                errorCode = YIMErrorcode_SetUserInfoFail;
            }
        }
    }
    if (m_hasSetPhotoUrl)
    {
        std::lock_guard<std::mutex> lock(m_setPhotoUrlMutex);
        m_hasSetPhotoUrl = false;
        m_pCallback->OnSetPhotoUrl(errorCode,m_curUserInfo.photoURL.c_str());
    }else{
        m_pCallback->OnSetUserInfo(errorCode);
    }
    
}

void YouMeIMUserInfoManager::OnUpdateRcUserOnlineStateRsp(ServerPacket &serverPacket)
{
    YouMe_LOG_Info(__XT("enter"));
    if (NULL == m_pCallback)
    {
        YouMe_LOG_Error(__XT("user info callback is null"));
        return;
    }
    
    YIMErrorcode errorCode = YIMErrorcode_Success;
    if (serverPacket.result == -1)
    {
        errorCode = YIMErrorcode_TimeOut;
        YouMe_LOG_Warning(__XT("update user online state timeout"));
    }
    else
    {
        YOUMEServiceProtocol::UpdateRcUserOnlineStateRsp rsp;
        if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
        {
            YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
            errorCode = YIMErrorcode_ServerError;
        }
        else
        {
            int nRet = rsp.ret();
            if (nRet == 0)
            {
                YouMe_LOG_Info(__XT("update user online state success"));
                std::lock_guard<std::mutex> lock(m_userInfoMutex);
                m_curUserInfo = m_updateUserInfo;
                
                YouMe_LOG_Info(__XT("OnUpdateRcUserOnlineStateRsp，userID: %s, onlineState: %d, nickname: %s, sex: %d, signature: %s, country: %s, city: %s, photoUrl: %s, beAddPermission:%d, foundPermission:%d"),m_curUserInfo.userID.c_str(),m_curUserInfo.onlineState,m_curUserInfo.settingInfo.nickName.c_str(),m_curUserInfo.settingInfo.sex,m_curUserInfo.settingInfo.personalSignature.c_str(),m_curUserInfo.settingInfo.country.c_str(),m_curUserInfo.settingInfo.city.c_str(),m_curUserInfo.photoURL.c_str(),m_curUserInfo.beAddPermission,m_curUserInfo.foundPermission);
            }
            else
            {
                YouMe_LOG_Error(__XT("update user online state error(%d)"), nRet);
                errorCode = YIMErrorcode_UpdateUserOnlineStateFail;
            }
        }
    }
    m_pCallback->OnSwitchUserOnlineState(errorCode);
}

void YouMeIMUserInfoManager::OnGetImageUploadTokenRsp(ServerPacket& serverPacket)
{
    YouMe_LOG_Info(__XT("get image upload token"));
    
    YIMErrorcode errorCode = YIMErrorcode_UnknowError;
    XUINT64 ullUploadSerial = 0;
    do
    {
        std::map<std::string, std::string>::const_iterator itrSerial = serverPacket.extend.find("uploadSerial");
        if (itrSerial == serverPacket.extend.end())
        {
            YouMe_LOG_Error(__XT("image uploadSerial not found %llu"), serverPacket.reqSerial);
            errorCode = YIMErrorcode_UnknowError;
            break;
        }
        ullUploadSerial = CStringUtilT<char>::str_to_uint64(itrSerial->second);
        
        if (serverPacket.result == -1)
        {
            YouMe_LOG_Error(__XT("get token timeout serial:%llu"), ullUploadSerial);
            errorCode = YIMErrorcode_TimeOut;
            break;
        }
        
        YOUMEServiceProtocol::GetUploadTokenRsp rsp;
        if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
        {
            YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
            errorCode = YIMErrorcode_ServerError;
            break;
        }
        if (rsp.ret() != 0)
        {
            YouMe_LOG_Error(__XT("get image upload token error(%d) serial:%llu"), rsp.ret(), ullUploadSerial);
            errorCode = YIMErrorcode_ServerError;
            break;
        }
        
        std::lock_guard<std::mutex> lock(m_uploadInfoLock);
        std::map<XUINT64, std::shared_ptr<ImageUploadInfo> >::iterator itr = m_imageUploadMap.find(ullUploadSerial);
        if (itr == m_imageUploadMap.end())
        {
            YouMe_LOG_Error(__XT("has not found in upload map serial:%llu"), ullUploadSerial);
            errorCode = YIMErrorcode_UnknowError;
            break;
        }
        
        if (rsp.headers_size() > 0)
        {
            errorCode = YIMErrorcode_Success;
            
            itr->second->uploadURL = UTF8TOXString(rsp.token());
            itr->second->downloadURL = UTF8TOXString(rsp.download_url());
            
            for (int i = 0; i < rsp.headers_size(); ++i)
            {
                itr->second->httpHead.insert(std::map<std::string, std::string>::value_type(rsp.headers(i).name(), rsp.headers(i).value()));
            }
            YouMe_LOG_Info(__XT("get upload image token success"));
            m_uploadSemaphore.Increment();
        }
        else
        {
            YouMe_LOG_Error(__XT("upload image has no http head serial:%llu"), ullUploadSerial);
            errorCode = YIMErrorcode_ServerError;
        }
    } while (0);
    
    if (errorCode != YIMErrorcode_Success && ullUploadSerial != 0)
    {
        YIMFileType fileType = FileType_Other;
        {
            std::lock_guard<std::mutex> lock(m_uploadInfoLock);
            std::map<XUINT64, std::shared_ptr<ImageUploadInfo> >::iterator itr = m_imageUploadMap.find(ullUploadSerial);
            if (itr != m_imageUploadMap.end())
            {
                fileType = itr->second->fileType;
                m_imageUploadMap.erase(itr);
            }
        }
    }
}

void YouMeIMUserInfoManager::OnUserInfoChangeNotify(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		return;
	}
	YOUMEServiceProtocol::RcUserInfoUpdateNotifyRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
	XString userID = UTF8TOXString(rsp.snder_id());
	m_pCallback->OnUserInfoChangeNotify(userID.c_str());
}

void YouMeIMUserInfoManager::UploadThreadProc()
{
    while (m_uploadSemaphore.Decrement())
    {
        if (m_bExitUpload)
        {
            break;
        }
        
        ImageUploadInfo uploadInfo;
        {
            std::lock_guard<std::mutex> lock(m_uploadInfoLock);
            if (m_imageUploadMap.size() == 0)
            {
                continue;
            }
            for (std::map<XUINT64, std::shared_ptr<ImageUploadInfo> >::iterator itr = m_imageUploadMap.begin(); itr != m_imageUploadMap.end(); ++itr)
            {
                if (!itr->second->uploadURL.empty())
                {
                    uploadInfo = *(itr->second);
                    m_imageUploadMap.erase(itr);
                    break;
                }
            }
        }
        
        std::string strResponse;
        YouMe_LOG_Info(__XT("upload image start"));
        bool bRet = CDownloadUploadManager::UploadFile(uploadInfo.uploadURL, uploadInfo.localPath, uploadInfo.httpHead, strResponse);
        YouMe_LOG_Info(__XT("upload image end path:%s result:%s"), uploadInfo.localPath.c_str(), bRet ? __XT("success") : __XT("failed"));
        
        
        YIMErrorcode errorCode = YIMErrorcode_Success;
        if (bRet)
        {
            if (uploadInfo.fileType == FileType_Image)
            {
                errorCode = SetPhotoUrl(uploadInfo.downloadURL);
                if ((m_pCallback != NULL) && (errorCode != YIMErrorcode_Success))
                {
                    m_pCallback->OnSetPhotoUrl(errorCode,__XT(""));
                }
            }
        }
        else
        {
            errorCode = YIMErrorcode_UploadFailed;
            YouMe_LOG_Error(__XT("set photoUrl error(%d)"), errorCode);
            
            if (m_pCallback != NULL)
            {
                m_pCallback->OnSetPhotoUrl(errorCode,__XT(""));
            }
        }
        
    }
    YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMUserInfoManager::RelationChainHeartBeatThread()
{
	YouMe_LOG_Info(__XT("enter"));

	RequestRelationHeartBeat();

	int heartbeatInterval = 10;
	if (m_pIManager != NULL)
	{
		heartbeatInterval = m_pIManager->GetConfigure<int>(CONFIG_RELATION_HEARTBEAT, 0);
	}
	heartbeatInterval *= 1000;
	
	while (true)
	{
		if (youmecommon::WaitResult_Timeout != m_heartBeatWait.WaitTime(heartbeatInterval))
		{
			YouMe_LOG_Info(__XT("received upper level notityfy thread exit"));
			break;
		}
		RequestRelationHeartBeat();
	}

	YouMe_LOG_Info(__XT("exit"));
}

void YouMeIMUserInfoManager::RequestRelationHeartBeat()
{
	YOUMEServiceProtocol::RcHeartbeatReq req;
	req.set_version(PROTOCOL_VERSION);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_RELATION_CHAIN_HEARTBEAT, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
	}
}

void YouMeIMUserInfoManager::StartUserStatusHeartbeat()
{
	// 关系链用户状态变化检测，服务端与IM登录登出分开，故需单独处理，以客户端心跳为依据
    int heartbeatInterval = m_pIManager->GetConfigure<int>(CONFIG_RELATION_HEARTBEAT, 0);
    if(heartbeatInterval > 0){
        std::lock_guard<std::mutex> lock(m_heartBeatThreadMutex);
        m_heartBeatWait.SetSignal();
        if (m_heartBeatThread.joinable())
        {
            m_heartBeatThread.join();
        }
        m_heartBeatWait.Reset();
        m_heartBeatThread = std::thread(&YouMeIMUserInfoManager::RelationChainHeartBeatThread, this);
    }
}


void YouMeIMUserInfoManager::StopUserStatusHeartbeat()
{
	std::lock_guard<std::mutex> lock(m_heartBeatThreadMutex);
	m_heartBeatWait.SetSignal();
	if (m_heartBeatThread.joinable())
	{
		m_heartBeatThread.join();
	}
}
