package com.youme.imsdk;

public class YIMMessageBodyGift implements IYIMMessageBodyBase {

    // for gift msg
    private Integer giftID;

    private Integer giftCount;

    private String anchor;
    private YIMExtraGifParam extParam;


    public Integer getGiftID() {
        return giftID;
    }

    public void setGiftID(Integer giftID) {
        this.giftID = giftID;
    }

    public Integer getGiftCount() {
        return giftCount;
    }

    public void setGiftCount(Integer giftCount) {
        this.giftCount = giftCount;
    }

    public String getAnchor() {
        return anchor;
    }

    public void setAnchor(String anchor) {
        this.anchor = anchor;
    }

    public YIMExtraGifParam getExtParam() {
        return extParam;
    }

    public void setExtParam(YIMExtraGifParam extParam) {
        this.extParam = extParam;
    }
}
