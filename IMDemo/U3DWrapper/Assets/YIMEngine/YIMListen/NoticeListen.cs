
namespace YIMEngine
{
	public interface NoticeListen
	{
        void OnRecvNotice(YIMEngine.Notice notice);
	    void OnCancelNotice(ulong noticeID, string channelID);
	}

}