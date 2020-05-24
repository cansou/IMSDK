package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class SpeechMessageInfo {
    @SerializedName("DownloadURL")
    private String downloadURL;
    @SerializedName("Duration")
    private Integer duration;
    @SerializedName("FileSize")
    private Integer fileSize;
    @SerializedName("LocalPath")
    private String localPath;
    @SerializedName("RequestID")
    private String requestID;
    @SerializedName("Text")
    private String text;

    public String getDownloadURL() {
        return downloadURL;
    }

    public void setDownloadURL(String downloadURL) {
        this.downloadURL = downloadURL;
    }

    public int getDuration() {
        return duration;
    }

    public void setDuration(int duration) {
        this.duration = duration;
    }

    public int getFileSize() {
        return fileSize;
    }

    public void setFileSize(int fileSize) {
        this.fileSize = fileSize;
    }

    public String getLocalPath() {
        return localPath;
    }

    public void setLocalPath(String localPath) {
        this.localPath = localPath;
    }

    public long getRequestID() {
        return Long.parseLong(requestID);
    }

    public void setRequestID(long requestID) {
        this.requestID = String.valueOf(requestID);
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }
}
