using YIMEngine;

namespace YouMe
{
    public class Config
    {
        private string _cachePath;

        public ServerZone ServerZone { get; set; }
        public LogLevel   LogLevel   { get; set; }

        public string CachePath { 
			set { 
				_cachePath = value;
                SetAudioCachePath(_cachePath);
			}
		}

		private void SetAudioCachePath(string cachePath)
		{ 		   
		    IMAPI.Instance().SetAudioCachePath(cachePath);
			Log.e("初始化 设置语音缓存目录" + cachePath);
		}
    }
}