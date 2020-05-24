package com.youme.imsdk;

public class YIMMessageBodyFile implements IYIMMessageBodyBase {

    private String fileName;
    private int fileSize;
    private int fileType;
    private String fileExtension;
    private String extParam;
    private String localPath;


    public String getFileExtension() {
        return fileExtension;
    }

    public void setFileExtension(String fileExtension) {
        this.fileExtension = fileExtension;
    }

    public String getExtParam() {
        return extParam;
    }

    public void setExtParam(String extParam) {
        this.extParam = extParam;
    }

    public int getFileType() {
        return fileType;
    }

    public void setFileType(int fileType) {
        this.fileType = fileType;
    }

    public int getFileSize() {
        return fileSize;
    }

    public void setFileSize(int fileSize) {
        this.fileSize = fileSize;
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public void setLocalPath(String path){
        this.localPath = path;
    }

    public String getLocalPath(){
        return this.localPath;
    }
}
