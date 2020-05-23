
namespace YIMEngine
{
	public interface DownloadListen
	{
		void OnDownload( YIMEngine.ErrorCode errorcode, YIMEngine.MessageInfoBase message, string strSavePath);	

		void OnDownloadByUrl( YIMEngine.ErrorCode errorcode, string strFromUrl, string strSavePath );
	}
}