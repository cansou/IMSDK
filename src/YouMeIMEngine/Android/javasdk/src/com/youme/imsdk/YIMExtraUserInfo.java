package com.youme.imsdk;

import com.google.gson.annotations.SerializedName;

public class YIMExtraUserInfo  {

    @SerializedName("nickname")
    private  String nickName;
    @SerializedName("server_area")
    private  String serverArea;
    @SerializedName("location")
    private  String location;
    @SerializedName("platform")
    private  String platform;
    @SerializedName("level")
    private  String level;
    @SerializedName("vip_level")
    private  String vipLevel;
    @SerializedName("extra")
    private  String extra;
    @SerializedName("server_area_id")
    private  String serverAreaID;
    @SerializedName("platform_id")
    private  String platformID;
    @SerializedName("location_id")
    private  String locationID;

    private String userID;

    public YIMExtraUserInfo(){

    }

    public YIMExtraUserInfo(String nickName,String serverArea, String location, String platform,int level,int vipLevel,String serverAreaID,String platformID,String locationID,String extra){
        this.nickName = nickName;
        this.serverArea = serverArea;
        this.location = location;
        this.platform = platform;
        this.level = Integer.toString(level);
        this.vipLevel = Integer.toString(vipLevel);
        this.extra = extra;
        this.serverAreaID = serverAreaID;
        this.platformID = platformID;
        this.locationID = locationID;
    }

    /**
     * 昵称
     * @return
     */
    public String getNickName() {
        return nickName;
    }

    /**
     * 游戏服务名
     * @return
     */
    public String getServerArea() {
        return serverArea;
    }
    
    public String getServerAreaID() {
        return serverAreaID;
    }

    /**
     * 游戏大区名
     * @return
     */
    public String getLocation() {
        return location;
    }

    /**
     * 玩家角色等级
     * @return
     */
    public int getLevel() {
        return Integer.getInteger(level,0);
    }

    /**
     * 玩家vip等级
     * @return
     */
    public int getVipLevel() {
        return Integer.getInteger(vipLevel,0);
    }

    /**
     * 自定义扩展参数
     * @return
     */
    public String getExtra() {
        return extra;
    }
    
    public String getPlatformID() {
        return platformID;
    }

    public String getPlatform() {
        return platform;
    }
    
    public String getLocationID() {
        return locationID;
    }

    public String getUserID() {
        return userID;
    }

    public void setUserID(String userID) {
        this.userID = userID;
    }
}
