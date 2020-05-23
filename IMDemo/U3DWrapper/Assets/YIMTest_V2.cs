using UnityEngine;
using System.Collections.Generic;
using YouMe;
using YIMEngine;


//#if UNITY_IOS || UNITY_ANDROID
namespace U3dTest
{
    public class YIMTestV2 : MonoBehaviour        
    {

        private Vector2 m_Position = Vector2.zero;
        private string m_InGameLog = "";

		IMClient IM;
        // Use this for initialization
        void Start()
        {
			IM = IMClient.Instance;		
			YouMe.StatusCode code = IM.Initialize ("YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0", "r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE=", new Config () {
				ServerZone = ServerZone.China,
				CachePath = UnityEngine.Application.temporaryCachePath + "/TestIMAudioCache/"
			});
			if (code == StatusCode.Success) {
				Debug.Log("Init success.");
				// 注册接收新消息的方法
		        IM.SetReceiveMessageListener( OnReceiveMessage );
				// 下载监听
		        IM.SetDownloadListener(OnDownload);
		        // 断线事件监听
		        IM.SetDisconnectListener((disconnectEvt)=>{
		            Debug.Log("断线了");
		        });
		        // 被踢下线事件监听
		        IM.SetKickOffListener((kickoffEvt)=>{
		            Debug.Log("被踢下线了");
		        });
		        // 注册重连监听
		        IM.SetReconnectListener(OnReconnect);
		        // 取消公告监听
		        IM.SetCancelNoticeListener(OnCancelNotice);
		        // 其它用户进出房间监听
		        IM.SetOtherUserChannelEventListener(OnOtherUserChannelEvent);
		        // 接收公告监听
		        IM.SetRecvNoticeListener(OnRecvNotice);
		        // 录音音量值监听
		        IM.SetRecvRecordVolumeListener(OnRecordVolume);
		        // 获取当前位置监听
		        IM.SetGetCurrentLocationListener(OnGetCurrentLocation);
		        // 举报结果监听
		        IM.SetAccusationListener(OnAccusationResult);

			} else {
				Debug.Log("Init fail，errorcode："+code.ToString());
			}		        
        }

        // Update is called once per frame
        void Update()
        {

        }
        void showStatus(string msg)
        {
            m_InGameLog += msg;
            m_InGameLog += "\r\n";
        }
        void OnGUI()
        {

            int inset = Screen.width / 20;
            int space = Screen.width / 30;
            int btnsOneRow = 3;
            int btnWidth = (Screen.width - inset * 2 - space * (btnsOneRow - 1)) / btnsOneRow;
            int btnHeight = btnWidth / 3;

            int labelX = inset;
            int labelY = inset + (btnHeight + space) * 8;
            int labelWidth = Screen.width - labelX * 2;
            int labelHeight = Screen.height - labelY;

            GUI.BeginGroup(new Rect(labelX, labelY, labelWidth, labelHeight));

            m_Position = GUILayout.BeginScrollView(m_Position, GUILayout.Width(labelWidth), GUILayout.Height(labelHeight));
            GUILayout.Label(m_InGameLog);
            GUILayout.EndScrollView();

            GUI.EndGroup();

            if (GUI.Button(new Rect(inset, inset, btnWidth, btnHeight), "login"))
            {
				IM.Login("123","",(evt)=>{
		            if(evt.Code == StatusCode.Success){
						Debug.Log("Login success.");               

		                string cachePath = IMClient.Instance.GetAudioCachePath();
						Debug.Log("GetAudioCachePath："+cachePath);

						string dirpath = UnityEngine.Application.temporaryCachePath + "/TestIMAudioDownloadDir/";
		                StatusCode down_code = IMClient.Instance.SetDownloadDir(dirpath);
		                if (down_code!=StatusCode.Success){
							Debug.Log("SetDownloadDir fail，code："+down_code.ToString());
		                }else{
							Debug.Log("SetDownloadDir success，path:"+dirpath);
		                }
						
		                IMClient.Instance.GetMicrophoneStatus((status_code, status)=>{
							Debug.Log("GetMicrophoneStatus callback，code:"+status_code.ToString()+", device_status:"+status.ToString());
		                });
		                              
		            }else{		                
						Debug.Log("Login fail, errorcode："+evt.Code);
		            }
		        });
            }

            if (GUI.Button(new Rect(inset + btnWidth + space, inset, btnWidth, btnHeight), "logout"))
            {                
                showStatus("logout");
                YIMEngine.IMAPI.Instance().Logout();
				IM.Logout((evt)=>{		            
					Debug.Log("Logout,code:"+evt.Code.ToString()+", userID:"+evt.UserID);
		        });
            }

			if (GUI.Button(new Rect(inset, inset + btnHeight + space, btnWidth, btnHeight), "joinchatroom"))
            {
				//进入聊天频道
                IM.JoinChannel(new IMChannel("1001"),(channelEvt)=>{
                    if(channelEvt.Code == StatusCode.Success){                        
                        IMClient.Instance.GetRoomMemberCount(new IMChannel("1001"),(chanel_code,channelID,count)=>{
							Debug.Log("GetRoomMemberCount callback, code:"+chanel_code.ToString()+",channelID:"+channelID+", count:"+count.ToString());
                        });         						

						Debug.Log("SetRoomHistoryMessageSwitch,channelID:1001.");
                        var roomIDs = new List<string>();
                        roomIDs.Add("1001");
                        IMClient.Instance.SetRoomHistoryMessageSwitch(roomIDs,true);

                    }else{
						Debug.Log("Enter channel:1001 fail.");
                    }
                }); 
				Debug.Log("Start entering channel:1001.");
            }

			if (GUI.Button(new Rect(inset+ btnWidth + space, inset + btnHeight + space, btnWidth, btnHeight), "leavechatroom"))
            {                
                showStatus("leavechatroom");//               
				IMClient.Instance.LeaveChannel(new IMChannel("1001"),(channelEvt)=>{
					if(channelEvt.Code == StatusCode.Success){                       
						Debug.Log("Join channel:"+channelEvt.ChannelID+" success.");                        

                    }else{
						Debug.Log("Leave channel:"+ channelEvt.ChannelID+ " fail, errorcode: "+channelEvt.Code.ToString());
                    }
				});
            }

			if (GUI.Button(new Rect(inset, inset + (btnHeight + space) * 2, btnWidth, btnHeight), "sendmessage"))
            {
				var msg = IM.SendTextMessage("123",ChatType.PrivateChat,"欢迎接收消息！","",(code,textMsgObj)=>{
		            if (code == StatusCode.Success)
		            {
						Debug.Log("Send:" + textMsgObj.Content + " success.");
		            }else{
						Debug.Log("Send:" + textMsgObj.Content + " fail.");
		            }
		        });               

				Debug.Log("Start Record.");
				StatusCode ret = IM.StartRecordAudio ("123", ChatType.PrivateChat, "",true);
				if (ret == StatusCode.Success) {
					Debug.Log("Start record success.");
				} else {
					Debug.Log("Start record fail.");
				}
            }

			if (GUI.Button(new Rect(inset + btnWidth + space, inset + (btnHeight + space) * 2, btnWidth, btnHeight), "sendcustommessage"))
            {
                Debug.Log("sendcustommessage");
                showStatus("sendcustommessage");				
                string strText = "112345";

				IM.SendCustomMessage("1001",ChatType.PrivateChat,System.Text.Encoding.UTF8.GetBytes(strText),(code,cusMes)=>{
					Debug.Log("sendmessage,errorcode: " + code.ToString());
				});   
                
            }

            if (GUI.Button(new Rect(inset, inset + (btnHeight + space) * 3, btnWidth, btnHeight), "StopAudio"))
            {                
				IMClient.Instance.StopRecordAndSendAudio ((code, audioMsg) => {
					if (audioMsg.SendStatus == SendStatus.Sending) {
						Debug.Log("Sending voice.");
					} else if(audioMsg.SendStatus == SendStatus.Sended) {
						Debug.Log("Send voice success.");
					} else if(audioMsg.SendStatus == SendStatus.Fail) {
						Debug.Log("Send voice fail.");
					} else if (code != StatusCode.Success) {
						Debug.Log("Stop record fail.");
					}
				});
            }

			if (GUI.Button(new Rect(inset+ btnWidth + space, inset + (btnHeight + space) * 3, btnWidth, btnHeight), "QueryUserStatus"))
            {				
                IMClient.Instance.QueryUserStatus("123",(code,userstatus)=>{
					Debug.Log("QueryUserStatus 123,code:"+code.ToString()+",status:"+userstatus.ToString());
                });

            }

			if (GUI.Button(new Rect(inset, inset + (btnHeight + space) * 4, btnWidth, btnHeight), "Clear"))
            {
                m_InGameLog = "";
            }

			if (GUI.Button(new Rect(inset + btnWidth + space, inset + (btnHeight + space) * 4, btnWidth, btnHeight), "filter"))
            {
                int level = 0;
				string strResult = IMClient.GetFilterText("这是江泽明de胡锦涛哦的法轮功的水电费水电费",level);
                showStatus("result:" + strResult + " level:" + level);				
            }


			if (GUI.Button(new Rect(inset + (btnWidth + space) * 2, inset + (btnHeight + space) * 4, btnWidth, btnHeight), "GetForbid"))
			{
				IMClient.Instance.GetForbiddenSpeakInfo((forbid_code, speakInfoList)=>{
					int lissize = speakInfoList.Count;
					Debug.Log("GetForbiddenSpeakInfo callback,code:"+forbid_code.ToString()+",the count of list:"+lissize);
	                if(lissize>0){
	                   foreach(var t in speakInfoList){
							Debug.Log("The forbidden info,channelID:"+t.ChannelID+",isForbidRoom："+t.IsForbidRoom);
	                   }
	                }
                });
			}
		}
		      

		void OnReceiveMessage (IMMessage msg)
		{
			if (msg.MessageType == MessageBodyType.Voice) {
				var audioMsg = (AudioMessage)msg;

				Debug.Log("Receive voice message, senderID: " + audioMsg.SenderID + ", receiverID: " + audioMsg.ReciverID + ",messageID:"+ audioMsg.RequestID+", content： " + audioMsg.RecongnizeText + ",AudioDuration: " + audioMsg.AudioDuration+", isFromServer: " + audioMsg.IsReceiveFromServer + ",audioFilePath:"+audioMsg.AudioFilePath);
				IMClient.Instance.SetMessageRead(msg.RequestID,true);
				audioMsg.Download ((code, audioMsgObj) => {
					if (code == StatusCode.Success) {

						StatusCode stop_code = audioMsgObj.StopPlay ();
						if (stop_code != StatusCode.Success) {
							Debug.Log("StopPlay fail，code:" + stop_code.ToString ());
						} else {
							Debug.Log("StopPlay success.");
						}
						Debug.Log("Download the voice success，senderID: " + audioMsgObj.SenderID + ", receiverID: " + audioMsgObj.ReciverID + "，语音文本： " + audioMsgObj.RecongnizeText + ",AudioDuration: " + audioMsgObj.AudioDuration+", isFromServer: " + audioMsgObj.IsReceiveFromServer + ",audioFilePath:"+audioMsgObj.AudioFilePath);

						audioMsgObj.PlayAudio ((playcode, playPath) => {
							if (playcode == StatusCode.Success) {
								Debug.Log("Play complete");
							} else {
								Debug.Log("Play fail,errorcode:" + playcode.ToString ());
							}
						});
					}else{
						Debug.Log("Download the voice fail，errorcode:" + code.ToString());
					}                
				});
			} else if (msg.MessageType == MessageBodyType.TXT) {
			    var txtMsg = (TextMessage)msg;
				Debug.Log("Receive txt message, content:" + txtMsg.Content);
				
			} else if (msg.MessageType == MessageBodyType.File) {				
				var fileMsg = (FileMessage)msg;
				Debug.Log("Receive file message,SenderID: " + fileMsg.SenderID+", receiverID: " +fileMsg.ReciverID +",文件名：" + fileMsg.FileName + ",filesize: " + fileMsg.FileSize + ",fileType: " + fileMsg.FileType + ",isReceiveFromServer: " + fileMsg.IsReceiveFromServer + ", extra: " + fileMsg.ExtraParam + ",extension: " + fileMsg.Extension);
				IMClient.Instance.SetMessageRead(msg.RequestID,true);
				string savepath = UnityEngine.Application.temporaryCachePath + "/TestIMAudioDownloadDir/测试连麦URL.rtf";
	
				IMClient.Instance.DownloadFile (fileMsg.RequestID, savepath, (down_code,message,path) => {
				    var fileMsgObj = (FileMessage)message;			    		        
					Debug.Log("DownloadFile callback，code:" + down_code.ToString () + ",SenderID: " + fileMsgObj.SenderID+", receiverID: " +fileMsgObj.ReciverID +",文件名：" + fileMsgObj.FileName + ",filesize: " + fileMsgObj.FileSize + ",fileType: " + fileMsgObj.FileType + ",isReceiveFromServer: " + fileMsgObj.IsReceiveFromServer + ", extra: " + fileMsgObj.ExtraParam + ",extension: " + fileMsgObj.Extension + ", savepath:" + path);		        
				});
			} else if (msg.MessageType == MessageBodyType.Gift) {
				var giftMsg = (GiftMessage)msg;
				Debug.Log("Receive gift message，AnchorID:" + giftMsg.AnchorID + ",count:" + giftMsg.GiftCount + ",giftID:" + giftMsg.GiftID);
			} else if (msg.MessageType == MessageBodyType.CustomMesssage) {
			    var cusMsg = (CustomMessage)msg;
				
				Debug.Log("Receive custom message,content:"+System.Text.Encoding.UTF8.GetString (cusMsg.Content));
			}
		}

		void OnDownload (StatusCode code, AudioMessage message, string savePath)
		{
			Debug.Log ("Download callback.");
			var audioMes = (AudioMessage)message;
			Debug.Log ("senderID: " + audioMes.SenderID + ", receiverID: " + audioMes.ReciverID + "，voice content： " + audioMes.RecongnizeText + ",AudioDuration: " + audioMes.AudioDuration + ", isFromServer: " + audioMes.IsReceiveFromServer + ",audioFilePath:" + audioMes.AudioFilePath);
				
			if (code == StatusCode.Success) {
				StatusCode stop_code = audioMes.StopPlay ();
				if (stop_code != StatusCode.Success) {
					Debug.Log ("StopPlay fail，code:" + stop_code.ToString ());
				} else {
					Debug.Log ("StopPlay success.");
				}
				Debug.Log ("Download success,start play audio.");
				audioMes.PlayAudio ((playcode, playPath) => {
					if (playcode == StatusCode.Success) {
						Debug.Log ("Play complete.");
					} else {
						Debug.Log ("Play fail,errorcode:" + playcode.ToString ());
					}
				});
			} else {
				Debug.Log ("Download fail.");
			}
		}

		void OnReconnect (IMReconnectEvent reconEvent)
		{
			Debug.Log("Reconnect callback，reconnect type：" + reconEvent.EventType.ToString() + ", the result of reconnect：" + reconEvent.Result.ToString());
		}

		void OnCancelNotice (ulong noticeID, string channelID)
		{
			Debug.Log("CancelNotice callback，noticeID：" + noticeID.ToString() + ", channelID:" + channelID.ToString());
		}

		void OnOtherUserChannelEvent(OtherUserChannelEvent otherEvent)
		{
			Debug.Log("OtherUserJoinLeaveChannel callback，eventType:" + otherEvent.EventType.ToString() + ", userID:" + otherEvent.UserID + ",channelID:" + otherEvent.ChannelID);
		}

		void OnRecvNotice(YIMEngine.Notice notice)
		{
			Debug.Log("RecvNotice callback，noticeID:"+notice.NoticeID+", channelID:"+notice.ChannelID+",content:"+notice.Content);
		}

		void OnRecordVolume (float volume)
		{
			Debug.Log("RecordVolumeChange callback，volume："+volume.ToString());
		}

		void OnGetCurrentLocation(StatusCode code, GeographyLocation location)
		{
			Debug.Log("GetCurrentLocation callback，code:"+ code.ToString()+", location country:"+location.Country+", city:"+location.City);
		}

		void OnAccusationResult (StatusCode code, IMAccusationInfo accuInfo)
		{
			Debug.Log("Accusation callback,errorcode:"+code.ToString()+",accusationDealResult:"+accuInfo.AccusationDealResult);
		}
	}
}
//#endif
