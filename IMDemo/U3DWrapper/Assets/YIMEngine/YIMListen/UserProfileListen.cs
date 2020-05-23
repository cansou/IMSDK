using System;

namespace YIMEngine
{
	public interface UserProfileListen
	{
		/*
        * 功能：查询用户信息回调
        * @param errorcode：错误码
        * @param userInfo：用户信息
        */
		void OnQueryUserInfo(YIMEngine.ErrorCode errorcode, IMUserProfileInfo userInfo);

		/*
        * 功能：设置用户信息回调
        * @param errorcode：错误码
        */
		void OnSetUserInfo(YIMEngine.ErrorCode errorcode);

		/*
        * 功能：切换用户在线状态回调
        * @param errorcode：错误码
        */
		void OnSwitchUserOnlineState(YIMEngine.ErrorCode errorcode);

		/*
        * 功能：设置头像回调
        * @param photoUrl：图片下载路径
        * @param errorcode：错误码
        */
		void OnSetPhotoUrl(YIMEngine.ErrorCode errorcode, string photoUrl);

		/*
	     * 功能：用户信息变更通知
	     * @param users：用户ID
	     */
	    void OnUserInfoChangeNotify(string userID);
	}
}

