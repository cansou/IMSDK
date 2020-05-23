
using System;

namespace YouMe{
    
    public enum ConnectEventType{
        CONNECTED,
        DISCONNECTED,
        CONNECT_FAIL,
        KICKED,
        OFF_LINE //掉线
    }

    public class IMConnectEvent
    {
        private StatusCode _code;
        private ConnectEventType _type;
        private string _userID;

        public StatusCode Code { 
            get{
                return _code;
            } 
        }

        public ConnectEventType EventType{ get { return _type; } }
        public string UserID{
            get{
                return _userID;
            } 
        }

        public IMConnectEvent(StatusCode code,ConnectEventType type,string userID){
            _code = code;
            _type = type;
            _userID = userID;
        }

    }

    public class ConnectEvent{
        private StatusCode _code;
        private string _userID;
        public string UserID{
            get{
                return _userID;
            } 
        }
        public StatusCode Code { 
            get{
                return _code;
            } 
        }
         public ConnectEvent(StatusCode code,string userID){
            _code = code;
            _userID = userID;
        }
    }

    public class LoginEvent:ConnectEvent{
        public LoginEvent(StatusCode code,string userID):base(code,userID){
        }
    }

    public class LogoutEvent:ConnectEvent{
        public LogoutEvent(StatusCode code,string userID):base(code,userID){
        }
    }

    public class KickOffEvent:ConnectEvent{
        public KickOffEvent(StatusCode code,string userID):base(code,userID){
        }
    }
    
    public class DisconnectEvent:ConnectEvent{
        public DisconnectEvent(StatusCode code,string userID):base(code,userID){
        }
    }
}