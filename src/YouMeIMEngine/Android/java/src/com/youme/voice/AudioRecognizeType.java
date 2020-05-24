package com.youme.voice;


//语音识别方式
public enum AudioRecognizeType
{
	RECOGNIZETYPE_NO, 	 		//无语音识别
	RECOGNIZETYPE_IFLY_RECORD,  //讯飞录音
	RECOGNIZETYPE_IFLY_STREAM, 	//讯飞音频流
	RECOGNIZETYPE_USC,  		//云知声
	RECOGNIZETYPE_ALI			//阿里
};