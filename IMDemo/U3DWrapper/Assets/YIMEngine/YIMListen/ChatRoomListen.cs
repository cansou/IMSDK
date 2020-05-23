
namespace YIMEngine
{
	public interface ChatRoomListen
	{
		void OnJoinRoom(YIMEngine.ErrorCode errorcode,string strChatRoomID);
		void OnLeaveRoom(YIMEngine.ErrorCode errorcode,string strChatRoomID);
		void OnLeaveAllRooms(YIMEngine.ErrorCode errorcode);
	    void OnUserJoinChatRoom(string strRoomID, string strUserID);
	    void OnUserLeaveChatRoom(string strRoomID, string strUserID);
        void OnGetRoomMemberCount(YIMEngine.ErrorCode errorcode, string strRoomID, uint count);
	}

}