using System;
using YouMe;
using YIMEngine;
using System.Collections.Generic;

public abstract class IMMessage{
	private MessageBodyType messageType;
	private string senderID;
	private string reciverID;
	private ChatType chatType;
	private bool isReceiveFromServer;
	private SendStatus sendStatus;
	private uint sendTime;
    /// <summary>
    /// 会话期间的消息唯一id
    /// </summary>
    private ulong requestID;
	private uint distance;
//    private bool isRead;

	public MessageBodyType MessageType{ set { this.messageType = value;} get { return this.messageType;} }

	public string SenderID{ set { this.senderID = value;} get { return this.senderID;} }    

    public string ReciverID{ set { this.reciverID = value;} get { return this.reciverID;} }

    public ChatType ChatType{ set { this.chatType = value;} get { return this.chatType;} }

	public bool IsReceiveFromServer{ set { this.isReceiveFromServer = value;} get { return this.isReceiveFromServer;} }

	public SendStatus SendStatus{ set { this.sendStatus = value;} get { return this.sendStatus;} }

	public uint SendTime{ set { this.sendTime = value; } get { return this.sendTime;}}

    public ulong RequestID{ set { this.requestID = value; } get { return this.requestID;}}

    public uint Distance{ set { this.distance = value; } get { return this.distance;}}

//    public bool IsRead{ set { this.isRead = value; } get { return this.isRead;}}
}

public enum MessageDownloadStatus{
    NOTDOWNLOAD = 0,
    DOWNLOADING = 1, 
    DOWNLOADED = 2, 
    DOWNLOAD_FAIL = 3
}

public enum SendStatus{
    NotStartSend = 0,
    Sending = 1,
    Sended = 2,
    Fail = 3,
}

public class AudioMessage:IMMessage{

    private MessageDownloadStatus downloadStatus;
	private string audioFilePath;
	private string recognizedText;
	private string extraParam;
	private int audioDuration;

    public AudioMessage(string sender,string reciverID,ChatType chatType,string extraParam,bool isFromServer){
        this.SenderID = sender;
		this.MessageType= MessageBodyType.Voice;
        this.ReciverID = reciverID;
        this.ChatType = chatType;
        this.extraParam = extraParam;       
        this.IsReceiveFromServer = isFromServer;

        if(isFromServer){
            this.SendStatus = SendStatus.Sended;
        }else{
            this.SendStatus = SendStatus.NotStartSend;
        }
    }

    public string ExtraParam{ get { return this.extraParam;} }

    public MessageDownloadStatus MessageDownloadStatus { set { this.downloadStatus = value; } get { return this.downloadStatus; } }

    public string AudioFilePath { set { this.audioFilePath = value; } get { return this.audioFilePath; } }

    public string RecongnizeText { set { this.recognizedText = value; } get { return this.recognizedText; } }

    public int AudioDuration{ set { this.audioDuration = value; } get { return this.audioDuration; } }	

	/// <summary>
    /// 播放语音消息  
	/// </summary>
	/// <param name="audioPath">语音消息路径</param>
	/// <param name="playCallback">语音播放回调</param>
	/// <param name="volume">可选参数，语音播放的音量值，范围：0.0-1.0 默认是1.0f</param>
	public void PlayAudio(Action<StatusCode, string> playCallback, float volume = 1.0f)
	{		
		if (!string.IsNullOrEmpty (audioFilePath)) {
			IMClient.Instance.StartPlayAudio(audioFilePath,(code, filePath)=>
			{			   
			   playCallback(code, filePath);
			}, volume);
		} else {
			playCallback(StatusCode.Fail, "");
		}        
    }

	/// <summary>
	/// 停止语音播放
	/// </summary>
	/// <returns> YouMe.StatusCode </returns>
    public StatusCode StopPlay ()
	{
		return IMClient.Instance.StopPlayAudio ();
	}

    public void PlayInQueue(){}    	

	/// <summary>
    /// 下载语音消息。
    /// </summary>
	/// <param name="downloadCallback">下载语音消息的回调通知</param>
	/// <param name="targetPath">可选参数，语音消息下载的存放路径，默认值为“”；若未设置下载目录且参数使用默认值，下载路径就是默认的下载路径；
	/// 若设置了下载目录且参数使用默认值，下载路径为设置的下载目录；若参数不是默认值，下载路径就是参数指定的路径  </param>    
	public void Download (Action<StatusCode,AudioMessage> downloadCallback, string targetPath = "")
	{
		if (!IsReceiveFromServer) {
			Log.e ("只能下载从服务器收到的语音消息，自己发送的语音消息不需要下载。");
			return;
		}

		if (this.downloadStatus == MessageDownloadStatus.DOWNLOADED) {
			if (downloadCallback != null)
				downloadCallback (StatusCode.Success, this);
			return;
		}
		this.downloadStatus = MessageDownloadStatus.DOWNLOADING;
		string downloadPath = GetUniqAudioPath();
		if (IMClient.Instance._downloadDirPath=="" && targetPath == "") {
			targetPath = downloadPath;
		}

        IMClient.Instance.DownloadFile( this.RequestID, targetPath, (StatusCode code,IMMessage messageObj,string savePath)=>{
            if( code == StatusCode.Success ){
                this.downloadStatus = MessageDownloadStatus.DOWNLOADED;
            }else{
                this.downloadStatus = MessageDownloadStatus.DOWNLOAD_FAIL;
            }
            this.audioFilePath = savePath;			
            if( downloadCallback!=null ) downloadCallback( code, this);
        });
    }

    public string GetUniqAudioPath()
    {
        return UnityEngine.Application.temporaryCachePath + "/YoumeIMAudioCache/"+ RequestID + ".wav";
    }
}

public class TextMessage:IMMessage
{
	private string content;
	private string extraParam;

    public TextMessage(string sender,string reciver,ChatType chatType,string content,string extraParam, bool isFromServer)
    {
        this.MessageType = MessageBodyType.TXT;
        this.SenderID = sender;
        this.ReciverID = reciver;
        this.ChatType = chatType;
        this.content = content;
        this.IsReceiveFromServer = isFromServer;
		this.extraParam = extraParam;
        if(isFromServer){
            this.SendStatus = SendStatus.Sended;
        }else{
            this.SendStatus = SendStatus.NotStartSend;
        }
    }
	   
    public string Content{ get { return this.content;}}

	public string ExtraParam{ get { return this.extraParam;}}
}

public class GiftMessage:IMMessage
{	
	private int iGiftCount;
	private int iGiftID;
	private ExtraGifParam strParam;

    public GiftMessage (string sender, string reciver, int giftID, int giftCount, ExtraGifParam param, bool isFromServer)
	{
		this.MessageType = MessageBodyType.Gift;
		this.SenderID = sender;
		this.ReciverID = reciver;
		this.ChatType = ChatType.RoomChat;
		this.iGiftCount = giftCount;
		this.iGiftID = giftID;
		this.strParam = param;
		this.IsReceiveFromServer = isFromServer;
		if (isFromServer) {
			this.SendStatus = SendStatus.Sended;
		} else {
			this.SendStatus = SendStatus.NotStartSend;
		}
	}

	public int GiftCount{ get { return this.iGiftCount;}}

	public int GiftID{ get { return this.iGiftID;}}

	public ExtraGifParam ExtraParam{ get { return this.strParam;}}

	public string AnchorID{ get { return this.ReciverID;} }
    
}

public class CustomMessage:IMMessage
{
	private byte[] content;

	public CustomMessage (string sender, string reciver, ChatType chatType, byte[] content, bool isFromServer)
	{
		this.MessageType = MessageBodyType.CustomMesssage;
		this.SenderID = sender;
		this.ReciverID = reciver;
		this.ChatType = chatType;
		this.content = content;
		this.IsReceiveFromServer = isFromServer;
		if (isFromServer){
			this.SendStatus = SendStatus.Sended;
		}else{
			this.SendStatus = SendStatus.NotStartSend;
		}
	}

    public byte[] Content{ get { return this.content;}}  	
}

public class FileMessage:IMMessage
{
	private FileType fileType;
	private string fileName;
	private int fileSize;
	private string extraParam;
	private string strExtension;

	public FileMessage(string sender,string reciver,ChatType chatType, string extra, FileType fileType, bool isFromServer) 
	{		
		this.MessageType = MessageBodyType.File;
		this.SenderID = sender;
		this.ReciverID = reciver;
		this.ChatType = chatType;
		this.extraParam = extra;
		this.fileType = fileType;
		this.IsReceiveFromServer = isFromServer;
		if(isFromServer){
			this.SendStatus = SendStatus.Sended;
		}else{
			this.SendStatus = SendStatus.NotStartSend;
		}
	}
	  
    public string ExtraParam{ get { return this.extraParam;}} 

    public FileType FileType{ get { return this.fileType;}} 
	
    public string FileName{ set { this.fileName = value; } get {return this.fileName; }}

    public int FileSize{ set { this.fileSize = value; } get { return this.fileSize; }}

    public string Extension{ set { this.strExtension = value; } get { return this.strExtension;}}
}

public class SpeechInfo
{
    ulong _requestID;
    string _downloadURL;
    int _duration;
    int _fileSize;
    string _localPath;
    string _text;
    bool _hasUpload;
	
    public SpeechInfo (ulong requestID)
	{
		_requestID = requestID;
	}

	public ulong RequestID{ get { return _requestID;}}
	public string DownloadURL{ set { _downloadURL = value; } get { return _downloadURL;}}
	public int Duration{ set { _duration = value; } get { return _duration; }}
	public int FileSize{ set { _fileSize = value; } get { return _fileSize;}}
	public string LocalPath{ set { _localPath = value; } get { return _localPath;}}
	public string Text{ set { _text = value; } get { return _text;}}
	public bool HasUpload{ set { _hasUpload = value; } get { return _hasUpload;}}
}

// move

public class IMHistoryMessageInfo
{	
	string _targetID;
	int _remain;
	List<HistoryMsg> _historyMsgList;

	public IMHistoryMessageInfo (string targetID, int remain, List<HistoryMsg> hisMsgList)
	{	   
	    _targetID = targetID;
	    _remain = remain;
	    _historyMsgList = hisMsgList;
	}

	public string TargetID{ get { return _targetID;} }
	public int Remain{ get { return _remain;} }
	public List<HistoryMsg> HistoryMesList{get {return _historyMsgList;} }
}

public class IMNearbyObjectInfo
{    
	List<RelativeLocation> _neighbourList;
	uint _startDistance;
	uint _endDistance;

	public IMNearbyObjectInfo (List<RelativeLocation> neighbourList, uint startDistance, uint endDistance)
	{		
	    _neighbourList = neighbourList;
	    _startDistance = startDistance;
	    _endDistance = endDistance;
	}

	public List<RelativeLocation> NeighbourList{ get { return _neighbourList;} }
	public uint StartDistance{ get { return _startDistance;} }
	public uint EndDistance{ get { return _endDistance;} }
}

public class IMAccusationInfo
{    
    AccusationDealResult _accDelResult;
    string _userID;
	uint _accusationTime;

	public IMAccusationInfo (AccusationDealResult result, string userID, uint time)
	{	  
	   _accDelResult = result;
	   _userID = userID;
	   _accusationTime = time;
	}

	public AccusationDealResult AccusationDealResult{ get { return _accDelResult; } }
	public string UserID{ get { return _userID; } }
	public uint AccusationTime{ get { return _accusationTime; } }
}
