
namespace YIMEngine
{
	public interface LoginListen
	{
		void OnLogin(YIMEngine.ErrorCode errorcode,string strYouMeID);
		void OnLogout();
		void OnKickOff();
	}

}