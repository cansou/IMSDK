
namespace YouMe{
    
    public class IMChannel : IChannel
    {
        string channelID;

        public string ChannelID{
            get{
                return channelID;
            }
        }

        public IMChannel(string channelID){
            this.channelID = channelID;
        }
    }
}