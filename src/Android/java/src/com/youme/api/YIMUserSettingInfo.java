package com.youme.api;

import com.google.gson.annotations.SerializedName;

public class YIMUserSettingInfo {	
	private String nickName;	
	private Integer sex;	
	private String signature;	
	private String country;	
	private String province;	
	private String city;	
	private String extraInfo;
	
    public YIMUserSettingInfo(){
		this.nickName = "";
		this.sex = 0;
		this.signature = "";
		this.country = "";
		this.province = "";
		this.city = "";
		this.extraInfo = "";
	}
    
    public String getNickName() {
		return nickName;
	}

	public void setNickName(String nickName) {
		this.nickName = nickName;
	}

	public Integer getSex() {
		return sex;
	}

	public void setSex(Integer sex) {
		this.sex = sex;
	}

	public String getSignature() {
		return signature;
	}

	public void setSignature(String signature) {
		this.signature = signature;
	}

	public String getCountry() {
		return country;
	}

	public void setCountry(String country) {
		this.country = country;
	}

	public String getProvince() {
		return province;
	}

	public void setProvince(String province) {
		this.province = province;
	}

	public String getCity() {
		return city;
	}

	public void setCity(String city) {
		this.city = city;
	}

	public String getExtraInfo() {
		return extraInfo;
	}

	public void setExtraInfo(String extraInfo) {
		this.extraInfo = extraInfo;
	}		
}
