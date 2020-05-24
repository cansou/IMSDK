package com.youme.imsdk;

import com.google.gson.annotations.SerializedName;

public class YIMExtraGifParam  {

    @SerializedName("nickname")
    private  String nickName;
    @SerializedName("server_area")
    private  String serverArea;
    @SerializedName("location")
    private  String location;
    @SerializedName("score")
    private  String score;
    @SerializedName("level")
    private  String level;
    @SerializedName("vip_level")
    private  String vipLevel;
    @SerializedName("extra")
    private  String extra;

    public YIMExtraGifParam(){

    }

    public YIMExtraGifParam(String nickName,String serverArea, String location,Long score,Integer level,Integer vipLevel,String extra){
        this.nickName = nickName;
        this.serverArea = serverArea;
        this.location = location;
        this.score = score.toString();
        this.level = level.toString();
        this.vipLevel = vipLevel.toString();
        this.extra = extra;
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

    /**
     * 游戏大区名
     * @return
     */
    public String getLocation() {
        return location;
    }

    /**
     * 积分
     * @return
     */
    public Long getScore() {
        return Long.getLong(score,0);
    }

    /**
     * 玩家角色等级
     * @return
     */
    public Integer getLevel() {
        return Integer.getInteger(level,0);
    }

    /**
     * 玩家vip等级
     * @return
     */
    public Integer getVipLevel() {
        return Integer.getInteger( vipLevel,0 );
    }

    /**
     * 自定义扩展参数
     * @return
     */
    public String getExtra() {
        return extra;
    }
}
