using UnityEngine;
namespace YIMEngine
{

    public class IMUserInfo{
        /// <summary>
        /// 昵称
        /// </summary>
        public string NickName { get; set; }
        /// <summary>
        /// 游戏服
        /// </summary>
        public string ServerArea { get; set; }
        /// <summary>
        /// 游戏服id
        /// </summary>
        public string ServerAreaID { get; set; }
        /// <summary>
        /// 大区
        /// </summary>
        public string Location { get; set; }
        /// <summary>
        /// 大区id
        /// </summary>
        public string LocationID { get; set; }
        /// <summary>
        /// 平台名称，比如：应用宝
        /// </summary>
        public string Platform { get; set; }
        /// <summary>
        /// 平台id
        /// </summary>
        public string PlatformID { get; set; }
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
            IMExtraUserInfoInternal jsonObj = new IMExtraUserInfoInternal();
            jsonObj.extra = this.Extra;
			jsonObj.level = this.Level.ToString();
			jsonObj.vip_level = this.VipLevel.ToString();
            jsonObj.location = this.Location;
            jsonObj.nickname = this.NickName;
            jsonObj.server_area = this.ServerArea;
            jsonObj.server_area_id = this.ServerAreaID;
            jsonObj.location_id = this.LocationID;
            jsonObj.platform = this.Platform;
            jsonObj.platform_id = this.PlatformID;
            return JsonMapper.ToJson(jsonObj);
        }

        public IMUserInfo ParseFromJsonString(string jsonStr){
            if (string.IsNullOrEmpty(jsonStr)){
                return this;
            }
            try{
                IMExtraUserInfoInternal jsonObj = JsonMapper.ToObject<IMExtraUserInfoInternal>(jsonStr);
                this.Extra = jsonObj.extra;
                this.Level = int.Parse( jsonObj.level );
                this.VipLevel = int.Parse(jsonObj.vip_level);
                this.Location = jsonObj.location;
                this.NickName = jsonObj.nickname;
                this.ServerArea = jsonObj.server_area;
                this.LocationID = jsonObj.location_id;
                this.PlatformID = jsonObj.platform_id;
                this.ServerAreaID = jsonObj.server_area_id;
                this.Platform = jsonObj.platform;
            }catch(System.Exception e){	
				Debug.Log(e.Message);
			}
            return this;
        }
    }

    public class IMExtraUserInfoInternal
    {

        public string nickname { get; set; }
       
        public string server_area { get; set; }
        public string server_area_id { get; set; }
       
        public string location { get; set; }
        public string location_id { get; set; }

        public string platform { get; set; }
        public string platform_id { get; set; }
        
        public string level { get; set; }
        
        public string vip_level { get; set; }
        
        public string extra { get; set; }
    }
}
