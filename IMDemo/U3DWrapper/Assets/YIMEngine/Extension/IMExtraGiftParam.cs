
namespace YIMEngine
{

    public class ExtraGifParam{
        /// <summary>
        /// 昵称
        /// </summary>
        public string NickName { get; set; }
        /// <summary>
        /// 游戏服
        /// </summary>
        public string ServerArea { get; set; }
        /// <summary>
        /// 大区
        /// </summary>
        public string Location { get; set; }
        /// <summary>
        /// 积分
        /// </summary>
        public long Score { get; set; }
        /// <summary>
        /// 角色等级
        /// </summary>
        public int Level { get; set; }
        /// <summary>
        /// vip等级
        /// </summary>
        public int VipLevel { get; set; }
        /// <summary>
        /// 扩展信息
        /// </summary>
        public string Extra { get; set; }

        public override string ToString(){
            return ToJsonString();
        }

        public string ToJsonString()
        {
            ExtraGifParamInternal jsonObj = new ExtraGifParamInternal();
            jsonObj.extra = this.Extra;
			jsonObj.level = this.Level.ToString();
			jsonObj.vip_level = this.VipLevel.ToString();
            jsonObj.location = this.Location;
            jsonObj.nickname = this.NickName;
			jsonObj.score = this.Score.ToString();
            jsonObj.server_area = this.ServerArea;
            return JsonMapper.ToJson(jsonObj);
        }

        public ExtraGifParam ParseFromJsonString(string jsonStr){
            ExtraGifParamInternal jsonObj = JsonMapper.ToObject<ExtraGifParamInternal>(jsonStr);
            this.Extra = jsonObj.extra;
            this.Level = int.Parse( jsonObj.level );
            this.VipLevel = int.Parse(jsonObj.vip_level);
            this.Location = jsonObj.location;
            this.NickName = jsonObj.nickname;
            this.Score = int.Parse( jsonObj.score );
            this.ServerArea = jsonObj.server_area;
            return this;
        } 
    }

    public class ExtraGifParamInternal
    {

        public string nickname { get; set; }
       
        public string server_area { get; set; }
       
        public string location { get; set; }
        
        public string score { get; set; }
        
        public string level { get; set; }
        
        public string vip_level { get; set; }
        
        public string extra { get; set; }
    }
}