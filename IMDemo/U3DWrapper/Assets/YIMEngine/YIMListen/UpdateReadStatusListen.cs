using System;

namespace YIMEngine
{
	public interface UpdateReadStatusListen
	{
		/*
        * 功能：收到对端已读消息后，回调此函数，将msgID及之前的未读消息设置为已读
        * @param recvId: 接收端用户Id
        * @param chatType: 聊天类型
        * @param msgId: 最后一条已读消息的消息Id
        */
		void onRead(string recvId, int chatType, ulong msgId);
	}
}

