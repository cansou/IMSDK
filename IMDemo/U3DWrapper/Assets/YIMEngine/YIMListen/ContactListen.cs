using System.Collections.Generic;


namespace YIMEngine
{
	public interface ContactListen
	{	
		/// <summary>
        /// 获取最近联系人id列表的结果通知
        /// </summary>
        /// <param name="contactLists">用户id列表</param>
        void OnGetContact(List<ContactsSessionInfo> contactLists);

        void OnGetUserInfo(ErrorCode code, string userID, IMUserInfo userInfo);

        void OnQueryUserStatus(ErrorCode code, string userID, UserStatus status);
    }

}