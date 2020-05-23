namespace YouMe {

    public enum ReconnectEventType {
        START_RECONNECT,      // 开始重连
        END_RECONNECT         // 重连结束 
    }

    public enum ReconnectEventResult {        
		RECONNECTRESULT_SUCCESS,            // 重连成功
		RECONNECTRESULT_FAIL_AGAIN,         // 重连失败，再次重连
		RECONNECTRESULT_FAIL,               // 重连失败
		RECONNECTRESULT_STARTING_RECONNECT  //重连中
    }

	public class IMReconnectEvent
    {        
        private ReconnectEventType _type;
		private ReconnectEventResult _result; 
               
        public ReconnectEventType EventType{ get { return _type; } }
		public ReconnectEventResult Result{ get { return _result; } }

		public IMReconnectEvent (ReconnectEventType eType, ReconnectEventResult result)
		{
		    _type = eType;
		    _result = result;
		}
    }
}
