using System.Collections.Generic;

namespace YIMEngine
{
	public interface FriendListen
	{
        /*
	    * 功能：查找添加好友（获取用户简要信息）
	    * @param findType：查找类型	0：按ID查找	1：按昵称查找
	    * @param target：对应查找类型用户ID或昵称
	    * @return 错误码
	    */
        void OnFindUser(YIMEngine.ErrorCode errorcode, List<UserBriefInfo> users);

	    /*
	    * 功能：请求添加好友回调
	    * @param errorcode：错误码
	    * @param userID：用户ID
	    */
        void OnRequestAddFriend(YIMEngine.ErrorCode errorcode, string userID);

	    /*
	    * 功能：被邀请添加为好友通知（需要验证）
	    * @param userID：用户ID
	    * @param comments：备注或验证信息
		* @param reqID：唯一请求id，如果收到同一个人多次请求，是不同的reqID
	    * commonts：显示用户信息可以根据userID查询
	    */
        void OnBeRequestAddFriendNotify(string userID, string comments, ulong reqID);

		/*
	    * 功能：被添加为好友通知（不需要验证）
	    * @param userID：用户ID
	    * @param comments：备注或验证信息
	    */
	    void OnBeAddFriendNotify(string userID, string comments);

		/*
	    * 功能：处理被请求添加好友回调
	    * @param errorcode：错误码
	    * @param userID：用户ID
	    * @param comments：备注或验证信息
	    * @param dealResult：处理结果	0：同意	1：拒绝
	    */
		void OnDealBeRequestAddFriend(YIMEngine.ErrorCode errorcode, string userID, string comments, int dealResult);

		/*
	    * 功能：请求添加好友结果通知(需要好友验证，待被请求方处理后回调)
	    * @param userID：用户ID
	    * @param comments：备注或验证信息
	    * @param dealResult：处理结果	0：同意	1：拒绝
	    */
	    void OnRequestAddFriendResultNotify(string userID, string comments, int dealResult);	   

	    /*
	    * 功能：删除好友结果回调
	    * @param errorcode：错误码
	    * @param userID：用户ID
	    */
        void OnDeleteFriend(YIMEngine.ErrorCode errorcode, string userID);

	    /*
	    * 功能：被好友删除通知
	    * @param userID：用户ID
	    */
        void OnBeDeleteFriendNotify(string userID);

	    /*
	    * 功能：拉黑或解除拉黑好友回调
	    * @param errorcode：错误码
	    * @param type：0：拉黑	1：解除拉黑
	    * @param userID：用户ID
	    */
        void OnBlackFriend(YIMEngine.ErrorCode errorcode, int type, string userID);
	   
	    /*
	    * 功能：查询我的好友回调
	    * @param errorcode：错误码
	    * @param type：0：正常好友	1：被拉黑好友
	    * @param startIndex：起始序号
	    * @param friends：好友列表
	    */
        void OnQueryFriends(YIMEngine.ErrorCode errorcode, int type, int startIndex, List<UserBriefInfo> friends);

	    /*
	    * 功能：查询好友请求列表回调
	    * @param errorcode：错误码
	    * @param startIndex：起始序号
	    * @param validateList：验证消息列表
	    */
        void OnQueryFriendRequestList(YIMEngine.ErrorCode errorcode, int startIndex, List<FriendRequestInfo> requestList);
	}
}