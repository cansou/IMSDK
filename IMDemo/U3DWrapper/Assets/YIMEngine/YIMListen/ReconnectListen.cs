
namespace YIMEngine
{
	public interface ReconnectListen
	{
		// 开始重连
		void OnStartReconnect();
		// 收到重连结果
		void OnRecvReconnectResult(ReconnectResult result);
	}
}

