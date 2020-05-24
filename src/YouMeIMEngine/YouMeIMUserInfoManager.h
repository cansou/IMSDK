
#ifndef YouMeIMUserInfoManager_hpp
#define YouMeIMUserInfoManager_hpp

#include <stdio.h>
#include <YouMeIMEngine/YIM.h>
#include <YouMeIMEngine/YouMeIMManager.h>

struct ImageUploadInfo
{
    XUINT64 msgSerial;
    YIMFileType fileType;
    unsigned int fileSize;    
    XString localPath;
    XString uploadURL;
    XString downloadURL;
    XString extension;
    std::map<std::string, std::string> httpHead;
    std::map<std::string, std::string> extend;
    
    ImageUploadInfo() : msgSerial(0), fileType(FileType_Image), fileSize(0){}
};

class YouMeIMUserInfoManager: public YIMUserProfileManager,
      public IManagerBase
{
    friend class YouMeIMManager;
public:
    YouMeIMUserInfoManager(YouMeIMManager *pIManager);
    ~YouMeIMUserInfoManager();
    
    void SetCallback(IYIMUserProfileCallback* pCallback);
    
    /*
     * 功能：设置用户基本资料
     * @param userSettingInfo：用户基本信息
     * @return 错误码
     */
    YIMErrorcode SetUserProfileInfo(const IMUserSettingInfo &userSettingInfo) override;
    
    /*
     * 功能：设置用户头像
     * @param photoUrl：本地图片绝对路径
     * @return 错误码
     */
    YIMErrorcode SetUserProfilePhoto(const XCHAR* photoPath) override;
    
    /*
     * 功能：获取用户基本资料
     * @param userID：指定用户ID,参数值为空时获取自己的基本资料
     * @return 错误码
     */
    YIMErrorcode GetUserProfileInfo(const XCHAR* userID = __XT("")) override;
    
    /*
     * 功能：切换用户状态
     * @param userID：用户ID
     * @param userStatus：用户状态，在线 | 隐身 | 离线
     * @return 错误码
     */
    YIMErrorcode SwitchUserStatus(const XCHAR* userID, YIMUserStatus userStatus) override;
    
    /*
     * 功能：设置好友添加权限
     * @param displayPermission：是否被别人查找到，true-能被查找，false-不能被查找
     * @param addPermission：被其它用户添加的权限
     * @return 错误码
     */
    YIMErrorcode SetAddPermission(bool beFound, IMUserBeAddPermission beAddPermission) override;

	void StartUserStatusHeartbeat();
	void StopUserStatusHeartbeat();

private:
    virtual void OnRecvPacket(ServerPacket& serverPacket) override;
    
    YIMErrorcode SetPhotoUrl(XString photoUrl);
    
    void OnGetRcUserInfoRsp(ServerPacket& serverPacket);
    void OnUpdateRcUserInfoRsp(ServerPacket& serverPacket);
    void OnUpdateRcUserOnlineStateRsp(ServerPacket& serverPacket);
    
    void UploadThreadProc();
    YIMErrorcode RequestUploadToken(ImageUploadInfo& uploadInfo);
    void OnGetImageUploadTokenRsp(ServerPacket& serverPacket);
	void OnUserInfoChangeNotify(ServerPacket& serverPacket);

	void RelationChainHeartBeatThread();
	void RequestRelationHeartBeat();
    
    IYIMUserProfileCallback* m_pCallback;
    
    //上传图片
    bool m_bExitUpload;
    bool m_uploadImage;
    bool hasUploadSuccess;
    std::thread m_uploadThread;
    youmecommon::CXSemaphore m_uploadSemaphore;
    std::mutex m_uploadInfoLock;
    std::map<XUINT64, std::shared_ptr<ImageUploadInfo> > m_imageUploadMap;

    std::mutex m_setPhotoUrlMutex; //设置头像锁
    bool m_hasSetPhotoUrl;  //调用设置头像
    std::mutex m_userInfoMutex; //当前用户基本信息锁
    std::mutex m_updateUserInfoMutex; //更新用户基本信息锁
    IMUserProfileInfo m_updateUserInfo;
    IMUserProfileInfo m_curUserInfo;
    XString m_curQueryUserID;

	std::thread m_heartBeatThread;
	youmecommon::CXCondWait m_heartBeatWait;
	std::mutex m_heartBeatThreadMutex;
};

#endif /* YouMeIMUserInfoManager_hpp */
