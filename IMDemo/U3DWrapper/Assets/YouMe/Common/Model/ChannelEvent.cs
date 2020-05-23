namespace YouMe
{
    public enum ChannelEventType{
        JOIN_SUCCESS,
        LEAVE_SUCCESS,    
        JOIN_FAIL,
        LEAVE_FAIL, 
		LEAVE_ALL_SUCCESS,
        LEAVE_ALL_FAIL              
    }
    public class ChannelEvent{

        StatusCode _code;
        ChannelEventType _eventType;
        string _channelID;

        public StatusCode Code { get{
                return _code;
            } }
        public ChannelEventType EventType{ get{
                return _eventType;
            } }
        public string ChannelID{ get{
                return _channelID;
            } }

        public ChannelEvent(StatusCode code,ChannelEventType eType,string channelID){
            _code = code;
            _eventType = eType;
            _channelID = channelID;
        }
    }


    public enum OtherUserChannelEventType{
        JOIN_CHANNEL,
        LEAVE_CHANNEL
    }

    public class OtherUserChannelEvent{
        string _channelID;
        string _userID;
        OtherUserChannelEventType _eventType;

        public string ChannelID { get { return _channelID; } }
        public string UserID { get { return _userID; } }
        public OtherUserChannelEventType EventType { get { return _eventType; } }

        public OtherUserChannelEvent (OtherUserChannelEventType eType, string channelID, string userID)
		{
			_eventType = eType;
			_channelID = channelID;
			_userID = userID;
		}
    }
}