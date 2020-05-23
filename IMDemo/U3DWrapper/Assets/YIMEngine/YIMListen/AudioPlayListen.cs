
namespace YIMEngine
{
    public interface AudioPlayListen
    {
        void OnPlayCompletion(YIMEngine.ErrorCode errorcode, string path);
        void OnGetMicrophoneStatus(YIMEngine.AudioDeviceStatus status);
    }

}