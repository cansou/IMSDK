//
//  IYouMeCInterface.cpp
//  youme_voice_engine
//
//  Created by YouMe.im on 15/12/10.
//  Copyright © 2015年 tencent. All rights reserved.
//

#include "IYouMeCInterface.h"
#include <YouMeCommon/Log.h>
#include <memory>
#include <mutex>
#include <YouMeCommon/XAny.h>
#include <YouMeCommon/XSharedArray.h>
#include <YouMeCommon/XVariant.h>
#include <YouMeCommon/json/json.h>
#include <YouMeCommon/XSemaphore.h>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeIMEngine/YouMeIMMessageManager.h>
#include <YouMeCommon/StringUtil.hpp>
bool s_YouMeInit = false;

extern void YouMe_ResizeImage(std::string srcImagePath, std::string resizedSavePath);

#ifndef NO_C_Interface
YIMManager* g_pIMManager = NULL;
std::mutex* g_contentMutex;
bool g_updateReadStatusFlag = false;
youmecommon::CXSemaphore* g_messageSemap;
//用一个map 保存解析器，根据不同的命令定制解析器的内容。
struct ParserContent
{
	ParserContent(YOUMEServiceProtocol::COMMOND_TYPE commandType)
	{
		m_commandType = commandType;
        m_errorcode = YIMErrorcode_Success;
	}
	//协议
	YOUMEServiceProtocol::COMMOND_TYPE m_commandType;
	//按顺序保存值
	std::vector<youmecommon::CXAny> m_values;
	//错误码
	YIMErrorcode m_errorcode;
};

std::list<XString> g_parserList;
bool InsertToMessageList(const std::shared_ptr<ParserContent>& content)
{
    youmecommon::Value jsonRoot;
    jsonRoot["Command"] = content->m_commandType;
    jsonRoot["Errorcode"] = content->m_errorcode;
    if(content->m_commandType != YOUMEServiceProtocol::CMD_RECORD_VOLUME)
    {
        YouMe_LOG_Info(__XT("insert message queue,current total:%lu command:%d code:%d"), g_parserList.size(),content->m_commandType, content->m_errorcode);
    }
    switch (content->m_commandType) {
		case YOUMEServiceProtocol::CMD_SND_VOICE_MSG:
        {
            jsonRoot["RequestID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[0]));
            jsonRoot["Text"] = content->m_values[1].Get();
			jsonRoot["LocalPath"] = content->m_values[2].Get(); 
			jsonRoot["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[3]);
			jsonRoot["SendTime"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[4]);
            jsonRoot["IsForbidRoom"] = youmecommon::CXAny::XAny_Cast<bool>(content->m_values[5]);
            jsonRoot["reasonType"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[6]);
            jsonRoot["forbidEndTime"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[7]));
            jsonRoot["messageID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[8]));

        }
			break;
		case YOUMEServiceProtocol::CMD_STOP_SEND_AUDIO:
		{
			jsonRoot["RequestID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[0]));
			jsonRoot["Text"] = content->m_values[1].Get();
			jsonRoot["LocalPath"] = content->m_values[2].Get();
			jsonRoot["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[3]);
		}
			break;
		case YOUMEServiceProtocol::CMD_DOWNLOAD:
        {
            jsonRoot["SavePath"] = content->m_values[0].Get();
            
            jsonRoot["ChatType"] = youmecommon::CXAny::XAny_Cast<YIMChatType>(content->m_values[1]);
            jsonRoot["SenderID"] = content->m_values[2].Get();
            jsonRoot["ReceiveID"] = content->m_values[3].Get();
            jsonRoot["Serial"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[4]));
            YIMMessageBodyType bodyType = youmecommon::CXAny::XAny_Cast<YIMMessageBodyType>(content->m_values[5]);
            jsonRoot["MessageType"] = bodyType;
            jsonRoot["CreateTime"] =youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[6]);
            jsonRoot["Distance"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[7]);
			jsonRoot["IsRead"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[8]);

            if(bodyType == MessageBodyType_Voice)
            {
                jsonRoot["Text"] = content->m_values[9].Get();
                jsonRoot["Param"] = content->m_values[10].Get();
                jsonRoot["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[11]);
            }
            else if (bodyType == MessageBodyType_File)
            {
                jsonRoot["FileName"] = content->m_values[9].Get();
                jsonRoot["FileSize"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[10]);
                jsonRoot["FileType"] = (int)youmecommon::CXAny::XAny_Cast<YIMFileType>(content->m_values[11]);
                jsonRoot["FileExtension"] = content->m_values[12].Get();
                jsonRoot["ExtraParam"] = content->m_values[13].Get();
            }
        }
            break;
        case YOUMEServiceProtocol::CMD_DOWNLOAD_URL:
        {
            jsonRoot["FromUrl"] = content->m_values[0].Get();
            jsonRoot["SavePath"] = content->m_values[1].Get();
            jsonRoot["AudioTime"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[2]);
        }
            break;
		case YOUMEServiceProtocol::CMD_LOGIN:
        {
			jsonRoot["UserID"] = content->m_values[0].Get();
        }
            break;
		case YOUMEServiceProtocol::CMD_SEND_MESSAGE_STATUS:
        {
			jsonRoot["RequestID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[0]));
			jsonRoot["SendTime"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[1]);
            jsonRoot["IsForbidRoom"] = youmecommon::CXAny::XAny_Cast<bool>(content->m_values[2]);
            jsonRoot["reasonType"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[3]);
            jsonRoot["forbidEndTime"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[4]));
            jsonRoot["messageID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[5]));
        }
            break;
        case YOUMEServiceProtocol::CMD_RECV_MESSAGE:
        {
            jsonRoot["ChatType"] = youmecommon::CXAny::XAny_Cast<YIMChatType>(content->m_values[0]);
			jsonRoot["SenderID"] = content->m_values[1].Get();
			jsonRoot["ReceiveID"] = content->m_values[2].Get();
			jsonRoot["Serial"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[3]));
            YIMMessageBodyType bodyType = youmecommon::CXAny::XAny_Cast<YIMMessageBodyType>(content->m_values[4]);
            jsonRoot["MessageType"] = bodyType;
            jsonRoot["CreateTime"] =youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[5]);
			jsonRoot["Distance"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[6]);
			jsonRoot["IsRead"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[7]);
            if(bodyType == MessageBodyType_TXT)
            {
                jsonRoot["Content"] = content->m_values[8].Get();
                jsonRoot["AttachParam"] = content->m_values[9].Get();
            }
            else if(bodyType == MessageBodyType_CustomMesssage)
            {
                //对二进制数据进行base64
                std::string strBase64Output;
                youmecommon::CCryptUtil::Base64Encoder(youmecommon::CXAny::XAny_Cast<std::string>(content->m_values[8]), strBase64Output);
                jsonRoot["Content"] = strBase64Output;
                
            }
            else if(bodyType == MessageBodyType_Voice)
            {
                 jsonRoot["Text"] = content->m_values[8].Get();
				 jsonRoot["Param"] = content->m_values[9].Get();
				 jsonRoot["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[10]);
            }
			else if (bodyType == MessageBodyType_Gift)
			{
				jsonRoot["GiftID"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[8]);
				jsonRoot["GiftCount"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[9]);
				jsonRoot["Anchor"] = content->m_values[10].Get();
				jsonRoot["Param"] = content->m_values[11].Get();
			}
			else if (bodyType == MessageBodyType_File)
			{
				/*
					content->m_values.push_back(XString(pFile->GetFileName()));
					content->m_values.push_back(pFile->GetFileSize());
					content->m_values.push_back(pFile->GetFileType());
					content->m_values.push_back(XString(pFile->GetFileExtension()));
					content->m_values.push_back(XString(pFile->GetExtraParam()));*/

				jsonRoot["FileName"] = content->m_values[8].Get();
				jsonRoot["FileSize"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[9]);
				jsonRoot["FileType"] = (int)youmecommon::CXAny::XAny_Cast<YIMFileType>(content->m_values[10]);
				jsonRoot["FileExtension"] = content->m_values[11].Get();
				jsonRoot["ExtraParam"] = content->m_values[12].Get();
			}
        }
            break;
        case YOUMEServiceProtocol::CMD_ENTER_ROOM:
        {
			jsonRoot["GroupID"] = content->m_values[0].Get();
        }
            break;
		case YOUMEServiceProtocol::CMD_LEAVE_ROOM:
		{
			jsonRoot["GroupID"] = content->m_values[0].Get();
		}
			break;
        case YOUMEServiceProtocol::CMD_LEAVE_ALL_ROOM:
        {
            
        }
            break;
		case YOUMEServiceProtocol::CMD_USER_ENTER_ROOM:
		{
			jsonRoot["ChannelID"] = content->m_values[0].Get();
			jsonRoot["UserID"] = content->m_values[1].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_USER_LEAVE_ROOM:
		{
			jsonRoot["ChannelID"] = content->m_values[0].Get();
			jsonRoot["UserID"] = content->m_values[1].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_ROOM_INFO:
		{
			jsonRoot["RoomID"] = content->m_values[0].Get();
			jsonRoot["Count"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[1]);
		}
			break;
		case YOUMEServiceProtocol::CMD_LOGOUT:
		{

		}
			break;
		case YOUMEServiceProtocol::CMD_KICK_OFF:
		{

		}
			break;
		case YOUMEServiceProtocol::CMD_STOP_AUDIOSPEECH:
		{
			jsonRoot["RequestID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[0]));;
			jsonRoot["FileSize"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[1]);
			jsonRoot["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[2]);
			jsonRoot["Text"] = content->m_values[3].Get();
			jsonRoot["LocalPath"] = content->m_values[4].Get();
			jsonRoot["DownloadURL"] = content->m_values[5].Get();
		}
			break; 
		case YOUMEServiceProtocol::CMD_QUERY_HISTORY_MESSAGE:
		{
			jsonRoot["TargetID"] = content->m_values[0].Get();
			jsonRoot["Remain"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[1]);

			youmecommon::Value messageList(youmecommon::arrayValue);
			for (int i = 2; i < (int)content->m_values.size(); ++i)
			{
				std::vector<youmecommon::CXAny> messageInfo = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i]);
				youmecommon::Value messageItem;
				messageItem["ChatType"] = youmecommon::CXAny::XAny_Cast<YIMChatType>(messageInfo[0]);
				messageItem["SenderID"] = messageInfo[1].Get();
				messageItem["ReceiveID"] = messageInfo[2].Get();
				messageItem["Serial"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(messageInfo[3]));
				YIMMessageBodyType bodyType = youmecommon::CXAny::XAny_Cast<YIMMessageBodyType>(messageInfo[4]);
				messageItem["MessageType"] = bodyType;
                messageItem["CreateTime"]=youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[5]);
				messageItem["Distance"] = youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[6]);
				messageItem["IsRead"] = youmecommon::CXAny::XAny_Cast<int>(messageInfo[7]);
				if (bodyType == MessageBodyType_TXT)
				{
					messageItem["Content"] = messageInfo[8].Get();
                    messageItem["Param"] = messageInfo[9].Get();
				}
				else if (bodyType == MessageBodyType_CustomMesssage)
				{
					//对二进制数据进行base64
					std::string strBase64Output;
					youmecommon::CCryptUtil::Base64Encoder(youmecommon::CXAny::XAny_Cast<std::string>(messageInfo[8]), strBase64Output);
					messageItem["Content"] = strBase64Output;
				}
				else if (bodyType == MessageBodyType_Voice)
				{
					messageItem["Text"] = messageInfo[8].Get();
					messageItem["Param"] = messageInfo[9].Get();
					messageItem["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[10]);
					messageItem["LocalPath"] = messageInfo[11].Get();
                    messageItem["IsPlayed"] = youmecommon::CXAny::XAny_Cast<int>(messageInfo[12]);
                }else if (bodyType == MessageBodyType_File){
                    
                    messageItem["Param"] = messageInfo[8].Get();
                    messageItem["LocalPath"] = messageInfo[9].Get();
                    messageItem["FileName"] = messageInfo[10].Get();
                    messageItem["FileExtension"] = messageInfo[11].Get();
                    messageItem["FileSize"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[12]);
                    messageItem["FileType"] = youmecommon::CXAny::XAny_Cast<YIMFileType>(messageInfo[13]);                    
                }
				messageList[i - 2] = messageItem;
			}
			jsonRoot["messageList"] = messageList;
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_ROOM_HISTORY_MSG:
		{
			jsonRoot["RoomID"] = content->m_values[0].Get();
			jsonRoot["Remain"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[1]);

			youmecommon::Value messageList(youmecommon::arrayValue);
			for (int i = 2; i < (int)content->m_values.size(); ++i)
			{
				std::vector<youmecommon::CXAny> messageInfo = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i]);
				youmecommon::Value messageItem;

				messageItem["ChatType"] = youmecommon::CXAny::XAny_Cast<YIMChatType>(messageInfo[0]);
				messageItem["SenderID"] = messageInfo[1].Get();
				messageItem["ReceiveID"] = messageInfo[2].Get();
				messageItem["Serial"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(messageInfo[3]));
				YIMMessageBodyType bodyType = youmecommon::CXAny::XAny_Cast<YIMMessageBodyType>(messageInfo[4]);
				messageItem["MessageType"] = bodyType;
				messageItem["CreateTime"] = youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[5]);
				messageItem["Distance"] = youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[6]);
				messageItem["IsRead"] = youmecommon::CXAny::XAny_Cast<int>(messageInfo[7]);
				if (bodyType == MessageBodyType_TXT)
				{
					messageItem["Content"] = messageInfo[8].Get();
                    messageItem["AttachParam"] = messageInfo[9].Get();
				}
				else if (bodyType == MessageBodyType_CustomMesssage)
				{
					//对二进制数据进行base64
					std::string strBase64Output;
					youmecommon::CCryptUtil::Base64Encoder(youmecommon::CXAny::XAny_Cast<std::string>(messageInfo[8]), strBase64Output);
					messageItem["Content"] = strBase64Output;
				}
				else if (bodyType == MessageBodyType_Voice)
				{
					messageItem["Text"] = messageInfo[8].Get();
					messageItem["Param"] = messageInfo[9].Get();
					messageItem["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[10]);
				}
				else if (bodyType == MessageBodyType_Gift)
				{
					messageItem["GiftID"] = youmecommon::CXAny::XAny_Cast<int>(messageInfo[8]);
					messageItem["GiftCount"] = youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[9]);
					messageItem["Anchor"] = messageInfo[10].Get();
					messageItem["Param"] = messageInfo[11].Get();
				}
				else if (bodyType == MessageBodyType_File)
				{
					messageItem["FileName"] = messageInfo[8].Get();
					messageItem["FileSize"] = youmecommon::CXAny::XAny_Cast<unsigned int>(messageInfo[9]);
					messageItem["FileType"] = (int)youmecommon::CXAny::XAny_Cast<YIMFileType>(messageInfo[10]);
					messageItem["FileExtension"] = messageInfo[11].Get();
					messageItem["ExtraParam"] = messageInfo[12].Get();
				}

				messageList[i - 2] = messageItem;
			}
			jsonRoot["MessageList"] = messageList;
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_RENCENT_CONTACTS:
		{
			youmecommon::Value contactArray(youmecommon::arrayValue);
			for (int i = 0; i < (int)content->m_values.size(); ++i)
			{
				std::vector<youmecommon::CXAny> contactMessageInfo = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i]);
				youmecommon::Value contactItem;
				contactItem["ContactID"] = contactMessageInfo[0].Get();
				contactItem["MessageType"] = youmecommon::CXAny::XAny_Cast<int>(contactMessageInfo[1]);
				contactItem["MessageContent"] = contactMessageInfo[2].Get();
				contactItem["CreateTime"] = youmecommon::CXAny::XAny_Cast<unsigned int>(contactMessageInfo[3]);
                contactItem["NotReadMsgNum"] = youmecommon::CXAny::XAny_Cast<unsigned int>(contactMessageInfo[4]);
                contactItem["LocalPath"] = contactMessageInfo[5].Get();
                
				contactArray[i] = contactItem;
			}
			jsonRoot["contacts"] = contactArray;
		}
			break;
		case YOUMEServiceProtocol::CMD_RECEIVE_MESSAGE_NITIFY:
		{
			jsonRoot["ChatType"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[0]);
			jsonRoot["TargetID"] = content->m_values[1].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_TRANSLATE_COMPLETE:
		{
			jsonRoot["RequestID"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[0]);
			jsonRoot["Text"] = content->m_values[1].Get();
			jsonRoot["SrcLangCode"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[2]);
			jsonRoot["DestLangCode"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[3]);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_USR_INFO:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
			jsonRoot["UserInfo"] = content->m_values[1].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_QUERY_USER_STATUS:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
			jsonRoot["Status"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[1]);
		}
			break;
		case YOUMEServiceProtocol::CMD_AUDIO_PLAY_COMPLETE:
		{
			jsonRoot["Path"] = content->m_values[0].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_MICROPHONE_STATUS:
		{
			jsonRoot["Status"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[0]);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_DISTRICT:
		{
			jsonRoot["DistrictCode"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[0]);
			jsonRoot["Country"] = content->m_values[1].Get();
			jsonRoot["Province"] = content->m_values[2].Get();
			jsonRoot["City"] = content->m_values[3].Get();
			jsonRoot["DistrictCounty"] = content->m_values[4].Get();
			jsonRoot["Street"] = content->m_values[5].Get();
			jsonRoot["Longitude"] = youmecommon::CXAny::XAny_Cast<double>(content->m_values[6]);
			jsonRoot["Latitude"] = youmecommon::CXAny::XAny_Cast<double>(content->m_values[7]);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_PEOPLE_NEARBY:
		{
			jsonRoot["StartDistance"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[0]);
			jsonRoot["EndDistance"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[1]);
			youmecommon::Value neighbourList(youmecommon::arrayValue);
			for (int i = 0; i < (int)content->m_values.size() - 2; ++i)
			{
				std::vector<youmecommon::CXAny> location = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i + 2]);
				youmecommon::Value item;
				item["UserID"] = location[0].Get();
				item["Distance"] = youmecommon::CXAny::XAny_Cast<unsigned int>(location[1]);
				item["Longitude"] = youmecommon::CXAny::XAny_Cast<double>(location[2]);
				item["Latitude"] = youmecommon::CXAny::XAny_Cast<double>(location[3]);
				item["Country"] = location[4].Get();
				item["Province"] = location[5].Get();
				item["City"] = location[6].Get();
				item["DistrictCounty"] = location[7].Get();
				item["Street"] = location[8].Get();

				neighbourList[i] = item;
			}
			jsonRoot["NeighbourList"] = neighbourList;
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_DISTANCE:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
			jsonRoot["Distance"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[1]);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_TIPOFF_MSG:
		{
			jsonRoot["Result"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[0]);
			jsonRoot["UserID"] = content->m_values[1].Get();
			jsonRoot["AccusationTime"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[2]);
		}
			break;
		case YOUMEServiceProtocol::CMD_RECV_NOTICE:
		{
			jsonRoot["NoticeID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[0]));
			jsonRoot["ChannelID"] = content->m_values[1].Get();
			jsonRoot["NoticeType"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[2]);
			jsonRoot["NoticeContent"] = content->m_values[3].Get();
			jsonRoot["LinkText"] = content->m_values[4].Get();
			jsonRoot["LinkAddress"] = content->m_values[5].Get();
			jsonRoot["BeginTime"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[6]);
			jsonRoot["EndTime"] = youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[7]);
		}
			break;
		case YOUMEServiceProtocol::CMD_CANCEL_NOTICE:
		{
			jsonRoot["NoticeID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[0]));
			jsonRoot["ChannelID"] = content->m_values[1].Get();
		}
			break;
        case YOUMEServiceProtocol::CMD_GET_FORBID_RECORD:
        {
            youmecommon::Value forbidList(youmecommon::arrayValue);
            for (int i = 0; i < (int)content->m_values.size() ; ++i)
            {
                std::vector<youmecommon::CXAny> forbidInfo = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i]);
                youmecommon::Value item;
                item["ChannelID"] = forbidInfo[0].Get();
                item["IsForbidRoom"] = youmecommon::CXAny::XAny_Cast<bool>(forbidInfo[1]);
                item["reasonType"] = youmecommon::CXAny::XAny_Cast<int>(forbidInfo[2]);
                item["forbidEndTime"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(forbidInfo[3]));
                
                forbidList[i] = item;
            }
            jsonRoot["ForbiddenSpeakList"] = forbidList;
        }
            break;
		case YOUMEServiceProtocol::CMD_SET_MASK_USER_MSG:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
			jsonRoot["Block"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[1]);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_MASK_USER_MSG:
		{
			youmecommon::Value userList(youmecommon::arrayValue);
			for (int i = 0; i < (int)content->m_values.size(); ++i)
			{
				youmecommon::Value userID(content->m_values[i].Get());
				userList[i] = content->m_values[i].Get();// userID;
			}
			jsonRoot["UserList"] = userList;
		}
			break;
		case YOUMEServiceProtocol::CMD_CLEAN_MASK_USER_MSG:
		{

		}
			break;

        case YOUMEServiceProtocol::CMD_GET_SPEECH_TEXT:
        {             
			jsonRoot["RequestID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[0]));
            jsonRoot["Text"] = content->m_values[1].Get();
        }
            break;
        case YOUMEServiceProtocol::CMD_START_RECONNECT:
        {
            
        }
            break;
        case YOUMEServiceProtocol::CMD_GET_RECONNECT_RESULT:
        {
            jsonRoot["Result"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[0]);
        }
            break;
        case YOUMEServiceProtocol::CMD_RECORD_VOLUME:
        {
            jsonRoot["Volume"] = youmecommon::CXAny::XAny_Cast<float>(content->m_values[0]);
        }
            break;
		case YOUMEServiceProtocol::CMD_FIND_FRIEND_BY_ID:
		{
			youmecommon::Value userList(youmecommon::arrayValue);
			for (int i = 0; i < (int)content->m_values.size(); ++i)
			{
				std::vector<youmecommon::CXAny> userInfo = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i]);
				youmecommon::Value item;
				item["UserID"] = userInfo[0].Get();
				item["Nickname"] = userInfo[1].Get();
				item["Status"] = youmecommon::CXAny::XAny_Cast<int>(userInfo[2]);
				userList[i] = item;
			}
			jsonRoot["UserList"] = userList;
		}
			break;
		case YOUMEServiceProtocol::CMD_REQUEST_ADD_FRIEND:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_REQUEST_ADD_FRIEND_NOTIFY:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
			jsonRoot["Comments"] = content->m_values[1].Get();
            jsonRoot["ReqID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[2]));
		}
			break;
		case YOUMEServiceProtocol::CMD_BE_ADD_FRIENT:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
			jsonRoot["Comments"] = content->m_values[1].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_DEAL_ADD_FRIEND:
		case YOUMEServiceProtocol::CMD_ADD_FRIENT_RESULT_NOTIFY:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
			jsonRoot["Comments"] = content->m_values[1].Get();
			jsonRoot["DealResult"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[2]);
		}
			break;
		case YOUMEServiceProtocol::CMD_DELETE_FRIEND:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_BE_DELETE_FRIEND_NOTIFY:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_BLACK_FRIEND:
		{
			jsonRoot["Type"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[0]);
			jsonRoot["UserID"] = content->m_values[1].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_BE_BLACK_FRIEND_NOTIFY:
		{
			jsonRoot["UserID"] = content->m_values[0].Get();
		}
			break;
		case YOUMEServiceProtocol::CMD_QUERY_FRIEND_LIST:
		{
			youmecommon::Value userList(youmecommon::arrayValue);
			jsonRoot["Type"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[0]);
			jsonRoot["StartIndex"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[1]);
			for (int i = 2; i < (int)content->m_values.size(); ++i)
			{
				std::vector<youmecommon::CXAny> userInfo = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i]);
				youmecommon::Value item;
				item["UserID"] = userInfo[0].Get();
				item["Nickname"] = userInfo[1].Get();
				item["Status"] = youmecommon::CXAny::XAny_Cast<int>(userInfo[2]);
				userList[i - 2] = item;
			}
			jsonRoot["UserList"] = userList;
		}
			break;
		case YOUMEServiceProtocol::CMD_QUERY_FRIEND_REQUEST_LIST:
		{
			youmecommon::Value userList(youmecommon::arrayValue);
			jsonRoot["StartIndex"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[0]);
			for (int i = 1; i < (int)content->m_values.size(); ++i)
			{
				std::vector<youmecommon::CXAny> userInfo = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i]);
				youmecommon::Value item;
				item["AskerID"] = userInfo[0].Get();
				item["AskerNickname"] = userInfo[1].Get();
				item["InviteeID"] = userInfo[2].Get();
				item["InviteeNickname"] = userInfo[3].Get();
				item["ValidateInfo"] = userInfo[4].Get();
				item["Status"] = youmecommon::CXAny::XAny_Cast<int>(userInfo[5]);
				item["CreateTime"] = youmecommon::CXAny::XAny_Cast<unsigned int>(userInfo[6]);
                item["ReqID"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(userInfo[7]));
				userList[i - 1] = item;
			}
			jsonRoot["UserList"] = userList;
		}
			break;
        case YOUMEServiceProtocol::CMD_HXR_USER_INFO_CHANGE_NOTIFY:
        {
            jsonRoot["UserID"] = content->m_values[0].Get();
        }
            break;
        case YOUMEServiceProtocol::CMD_SET_USER_PROFILE:
        {
            
        }
            break;
        case YOUMEServiceProtocol::CMD_GET_USER_PROFILE:
        {
            jsonRoot["UserID"] = content->m_values[0].Get();
            jsonRoot["PhotoUrl"] = content->m_values[1].Get();
            jsonRoot["OnlineState"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[2]);
            jsonRoot["BeAddPermission"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[3]);
            jsonRoot["FoundPermission"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[4]);
            jsonRoot["NickName"] = content->m_values[5].Get();
            jsonRoot["Sex"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[6]);
            jsonRoot["Signature"] = content->m_values[7].Get();
            jsonRoot["Country"] = content->m_values[8].Get();
            jsonRoot["Province"] = content->m_values[9].Get();
            jsonRoot["City"] = content->m_values[10].Get();
            jsonRoot["ExtraInfo"] = content->m_values[11].Get();
        }
            break;
        case YOUMEServiceProtocol::CMD_SET_USER_PHOTO:
        {
            jsonRoot["PhotoUrl"] = content->m_values[0].Get();
        }
            break;
        case YOUMEServiceProtocol::CMD_SWITCH_USER_STATE:
        {
            
        }
            break;
		case YOUMEServiceProtocol::CMD_UPLOAD_PROGRESS:

				jsonRoot["RequestId"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[0]));
				jsonRoot["Percent"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<float>(content->m_values[1]));
			break;
		case YOUMEServiceProtocol::CMD_SET_READ_STATUS:
			jsonRoot["RecvId"] = content->m_values[0].Get();
			jsonRoot["ChatType"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<int>(content->m_values[1]));
			jsonRoot["MsgId"] = CStringUtil::to_string(youmecommon::CXAny::XAny_Cast<XUINT64>(content->m_values[2]));
			break;
        default:
            YouMe_LOG_Warning(__XT("no matched command:%d"),content->m_commandType);
            break;
    }
    
    {
        std::lock_guard<std::mutex> lock(*g_contentMutex);
        g_parserList.push_back(jsonRoot.toSimpleString());
    }
    g_messageSemap->Increment();
    return true;
}
//这里返回的是一个常量的地址，注意不能把数据从 list 中弹出，否则c# 接受的数据会出错。
const XCHAR* HeadMessage()
{
    std::lock_guard<std::mutex> lock(*g_contentMutex);
    if (g_parserList.size() == 0) {
        return NULL;
    }
   
	XCHAR* pszTmpBuffer = new XCHAR[g_parserList.begin()->length() + 1];
	XStrCpy_S(pszTmpBuffer, int(g_parserList.begin()->length() + 1), g_parserList.begin()->c_str());
	g_parserList.pop_front();
	return pszTmpBuffer;
}

//清空消息
void CleanMessage()
{
    std::lock_guard<std::mutex> lock(*g_contentMutex);
    g_parserList.clear();
}

bool GetMessage(IYIMMessage* message, std::vector<youmecommon::CXAny>& content)
{
	content.push_back(message->GetChatType());
	content.push_back(XString(message->GetSenderID()));
	content.push_back(XString(message->GetReceiveID()));
	content.push_back(message->GetMessageID());
	IYIMMessageBodyBase* pbodyBase = message->GetMessageBody();
	content.push_back(pbodyBase->GetMessageType());
	content.push_back(message->GetCreateTime());
	content.push_back(message->GetDistance());
	content.push_back(static_cast<int>(message->IsRead()));

	switch (pbodyBase->GetMessageType())
	{
	case MessageBodyType_TXT:
	{
		IYIMMessageBodyText* pContent = (IYIMMessageBodyText*)pbodyBase;
		content.push_back(XString(pContent->GetMessageContent()));
        content.push_back(XString(pContent->GetAttachParam()));
	}
	break;
	case MessageBodyType_CustomMesssage:
	{
		IYIMMessageBodyCustom* pContent = (IYIMMessageBodyCustom*)pbodyBase;
		content.push_back(pContent->GetCustomMessage());
	}
	break;
	case MessageBodyType_Voice:
	{
		IYIMMessageBodyAudio* pContent = (IYIMMessageBodyAudio*)pbodyBase;
		content.push_back(XString(pContent->GetText()));
		content.push_back(XString(pContent->GetExtraParam()));
		content.push_back(pContent->GetAudioTime());
	}
	break;
	case MessageBodyType_Gift:
	{
		IYIMMessageGift* pContent = (IYIMMessageGift*)pbodyBase;
		content.push_back(pContent->GetGiftID());
		content.push_back(pContent->GetGiftCount());
		content.push_back(XString(pContent->GetAnchor()));
		content.push_back(XString(pContent->GetExtraParam()));
	}
	break;
	case MessageBodyType_File:
	{
		IYIMMessageBodyFile* pFile = (IYIMMessageBodyFile*)pbodyBase;
		content.push_back(XString(pFile->GetFileName()));
		content.push_back(pFile->GetFileSize());
		content.push_back(pFile->GetFileType());
		content.push_back(XString(pFile->GetFileExtension()));
		content.push_back(XString(pFile->GetExtraParam()));
	}
	break;
	default:
		return false;
	}
	return true;
}

class InterCallback :public IYIMLoginCallback,
	public IYIMMessageCallback,
	public IYIMChatRoomCallback,
    public IYIMDownloadCallback,
	public IYIMUpdateReadStatusCallback,
	public IYIMContactCallback,
	public IYIMAudioPlayCallback,
	public IYIMLocationCallback,
	public IYIMNoticeCallback,
    public IYIMReconnectCallback,
	public IYIMFriendCallback,
    public IYIMUserProfileCallback
{
public:
    virtual ~InterCallback()
    {
        
    }
public:
	//所有的回掉都是把东西按顺序组合好之后塞进去，根据每种命令的类型确定数据类型
	virtual void OnLogin(YIMErrorcode errorcode, const XCHAR * userID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_LOGIN));
		content->m_errorcode = errorcode;
		content->m_values.push_back( XString(userID) );
        InsertToMessageList(content);
	}

	virtual void OnLogout(YIMErrorcode errorcode) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_LOGOUT));
		content->m_errorcode = errorcode;
		 InsertToMessageList(content);
	}

	virtual void OnKickOff() override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_KICK_OFF));
		content->m_errorcode = YIMErrorcode_Success;
		InsertToMessageList(content);
	}

    void OnDownload( YIMErrorcode errorcode, std::shared_ptr<IYIMMessage> msg, const XCHAR * savePath ) override
    {
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_DOWNLOAD ));
        content->m_errorcode = errorcode;

        content->m_values.push_back( XString(savePath) );
        
        content->m_values.push_back(msg->GetChatType());
        content->m_values.push_back(XString(msg->GetSenderID()));
        content->m_values.push_back(XString(msg->GetReceiveID()));
        content->m_values.push_back(msg->GetMessageID());
        IYIMMessageBodyBase* pbodyBase = msg->GetMessageBody();
        content->m_values.push_back(pbodyBase->GetMessageType());
        content->m_values.push_back(msg->GetCreateTime());
        content->m_values.push_back(msg->GetDistance());
		content->m_values.push_back(static_cast<int>(msg->IsRead()));
        
        switch (pbodyBase->GetMessageType())
        {
            case MessageBodyType_Voice:
            {
                IYIMMessageBodyAudio* pContent = (IYIMMessageBodyAudio*)pbodyBase;
                content->m_values.push_back(XString(pContent->GetText()));
                content->m_values.push_back(XString(pContent->GetExtraParam()));
                content->m_values.push_back(pContent->GetAudioTime());
            }
                break;
            case MessageBodyType_File:
            {
                IYIMMessageBodyFile* pFile = (IYIMMessageBodyFile*)pbodyBase;
                content->m_values.push_back(XString(pFile->GetFileName()));
                content->m_values.push_back(pFile->GetFileSize());
                content->m_values.push_back(pFile->GetFileType());
                content->m_values.push_back(XString(pFile->GetFileExtension()));
                content->m_values.push_back(XString(pFile->GetExtraParam()));
            }
                break;
            default:
                assert(false);
                break;
        }
        InsertToMessageList(content);
        
    }
    
    void OnDownloadByUrl( YIMErrorcode errorcode, const XCHAR* strFromUrl, const XCHAR* savePath,int iAudioTime) override
    {
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_DOWNLOAD_URL));
        content->m_errorcode = errorcode;
        content->m_values.push_back( XString(strFromUrl) );
        content->m_values.push_back( XString(savePath) );
        content->m_values.push_back( (unsigned int)iAudioTime );
        InsertToMessageList(content);
    }

	virtual void OnStartSendAudioMessage(XUINT64 requestID, YIMErrorcode errorcode, const XCHAR* text, const XCHAR * audioPath, unsigned int audioTime)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_STOP_SEND_AUDIO));
		content->m_errorcode = errorcode;
		content->m_values.push_back(requestID);
		content->m_values.push_back(XString(text));
		content->m_values.push_back(XString(audioPath));
		content->m_values.push_back(audioTime);
		InsertToMessageList(content);
	}
	
	virtual void OnSendAudioMessageStatus(XUINT64 requestID, YIMErrorcode errorcode, const XCHAR* text, const XCHAR * audioPath, unsigned int audioTime, unsigned int sendTime,
		bool isForbidRoom, int reasonType, XUINT64 forbidEndTime, XUINT64 messageID)override
    {
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_SND_VOICE_MSG));
        content->m_errorcode = errorcode;
		content->m_values.push_back(requestID);
        content->m_values.push_back(XString(text));
		content->m_values.push_back(XString(audioPath));
		content->m_values.push_back(audioTime);
		content->m_values.push_back(sendTime);
        content->m_values.push_back(isForbidRoom);
        content->m_values.push_back(reasonType);
        content->m_values.push_back(forbidEndTime);
        content->m_values.push_back(messageID);
        InsertToMessageList(content);
    }

	virtual void OnSendMessageStatus(XUINT64 requestID, YIMErrorcode errorcode, unsigned int sendTime, bool isForbidRoom, int reasonType, XUINT64 forbidEndTime, XUINT64 messageID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_SEND_MESSAGE_STATUS));
		content->m_errorcode = errorcode;
		content->m_values.push_back(requestID);
		content->m_values.push_back(sendTime);
        content->m_values.push_back(isForbidRoom);
        content->m_values.push_back(reasonType);
        content->m_values.push_back(forbidEndTime);
        content->m_values.push_back(messageID);
		 InsertToMessageList(content);
	}

	virtual void OnUploadProgress(XUINT64 msgSerial, float percent) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_UPLOAD_PROGRESS));
		content->m_values.push_back(msgSerial);
		content->m_values.push_back(percent);
		InsertToMessageList(content);
	}

	virtual void OnUpdateReadStatus(const XCHAR* recvId, int chatType, XUINT64 msgSerial) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_SET_READ_STATUS));
		content->m_values.push_back(XString(recvId));
		content->m_values.push_back(chatType);
		content->m_values.push_back(msgSerial);
		InsertToMessageList(content);
	}

	virtual void OnStopAudioSpeechStatus(YIMErrorcode errorcode, std::shared_ptr<IAudioSpeechInfo> audioSpeechInfo) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_STOP_AUDIOSPEECH));
		content->m_errorcode = errorcode;
		content->m_values.push_back(audioSpeechInfo->GetRequestID());
		content->m_values.push_back(audioSpeechInfo->GetFileSize());
		content->m_values.push_back(audioSpeechInfo->GetAudioTime());
		content->m_values.push_back(XString(audioSpeechInfo->GetText()));
		content->m_values.push_back(XString(audioSpeechInfo->GetLocalPath()));
		content->m_values.push_back(XString(audioSpeechInfo->GetDownloadURL()));
		InsertToMessageList(content);
	}

	virtual void OnRecvMessage(std::shared_ptr<IYIMMessage> message) override
	{
		assert(NULL != message);
		if (NULL == message)
		{
			return;
		}
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_RECV_MESSAGE));
		if (GetMessage(message.get(), content->m_values))
		{
			InsertToMessageList(content);
		}
	}
    
    //语音文本识别回调
	virtual void OnGetRecognizeSpeechText(XUINT64 requestID, YIMErrorcode errorcode, const XCHAR* text) override
    {        
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_SPEECH_TEXT));
        content->m_errorcode = errorcode;
		content->m_values.push_back(requestID);
        content->m_values.push_back(XString(text));
        InsertToMessageList(content);
    }

	virtual void OnJoinChatRoom(YIMErrorcode errorcode, const XCHAR* chatRoomID) override
	{
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_ENTER_ROOM));
        content->m_errorcode = errorcode;    
        content->m_values.push_back(XString(chatRoomID));
        InsertToMessageList(content);
	}

	virtual void OnLeaveChatRoom(YIMErrorcode errorcode,  const XCHAR* chatRoomID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_LEAVE_ROOM));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(chatRoomID));
		InsertToMessageList(content);
	}
    
    virtual void OnLeaveAllChatRooms(YIMErrorcode errorcode) override
    {
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_LEAVE_ALL_ROOM));
        content->m_errorcode = errorcode;
        InsertToMessageList(content);
    }

	virtual void OnUserJoinChatRoom(const XCHAR* chatRoomID, const XCHAR* userID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_USER_ENTER_ROOM));
		content->m_errorcode = YIMErrorcode_Success;
		content->m_values.push_back(XString(chatRoomID));
		content->m_values.push_back(XString(userID));
		InsertToMessageList(content);
	}

	virtual void OnUserLeaveChatRoom(const XCHAR* chatRoomID, const XCHAR* userID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_USER_LEAVE_ROOM));
		content->m_errorcode = YIMErrorcode_Success;
		content->m_values.push_back(XString(chatRoomID));
		content->m_values.push_back(XString(userID));
		InsertToMessageList(content);
	}

	void OnGetRoomMemberCount(YIMErrorcode errorcode, const XCHAR * chatRoomID, unsigned int count) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_ROOM_INFO));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(chatRoomID));
		content->m_values.push_back(count);
		InsertToMessageList(content);
	}

	virtual void OnQueryHistoryMessage(YIMErrorcode errorcode, const XCHAR*  targetID, int remain, std::list<std::shared_ptr<IYIMMessage> > messageList) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_QUERY_HISTORY_MESSAGE));
		content->m_errorcode = errorcode;
		if (errorcode == YIMErrorcode_Success)
		{
			content->m_values.push_back( XString(targetID) );
			content->m_values.push_back(remain);
			for (std::list< std::shared_ptr<IYIMMessage> >::iterator itr = messageList.begin(); itr != messageList.end(); ++itr)
			{
				IYIMMessage* pMessage = (*itr).get();
				std::vector<youmecommon::CXAny> messageValues;
				messageValues.push_back(pMessage->GetChatType());
				messageValues.push_back(XString(pMessage->GetSenderID()));
				messageValues.push_back(XString(pMessage->GetReceiveID()));
				messageValues.push_back(pMessage->GetMessageID());
				IYIMMessageBodyBase* pbodyBase = pMessage->GetMessageBody();
				messageValues.push_back(pbodyBase->GetMessageType());
                messageValues.push_back(pMessage->GetCreateTime());
				messageValues.push_back(pMessage->GetDistance());
				messageValues.push_back(static_cast<int>(pMessage->IsRead()));
				switch (pbodyBase->GetMessageType())
				{
				case MessageBodyType_TXT:
				{
					IYIMMessageBodyText* pContent = (IYIMMessageBodyText*)pbodyBase;
					messageValues.push_back(XString(pContent->GetMessageContent()));
                    messageValues.push_back(XString(pContent->GetAttachParam()));
				}
				break;
				case MessageBodyType_CustomMesssage:
				{
					IYIMMessageBodyCustom* pContent = (IYIMMessageBodyCustom*)pbodyBase;
					messageValues.push_back(pContent->GetCustomMessage());
				}
				break;
				case MessageBodyType_Voice:
				{
					IYIMMessageBodyAudio* pContent = (IYIMMessageBodyAudio*)pbodyBase;
					messageValues.push_back(XString(pContent->GetText()));
					messageValues.push_back(XString(pContent->GetExtraParam()));
					messageValues.push_back(pContent->GetAudioTime());
					messageValues.push_back(XString(pContent->GetLocalPath()));
                    messageValues.push_back(static_cast<int>(pContent->IsPlayed()));
				}
				break;
                case MessageBodyType_File:
                {
                    IYIMMessageBodyFile* pContent = (IYIMMessageBodyFile*)pbodyBase;
                    messageValues.push_back(XString(pContent->GetExtraParam()));
                    messageValues.push_back(XString(pContent->GetLocalPath()));
                    messageValues.push_back(XString(pContent->GetFileName()));
                    messageValues.push_back(XString(pContent->GetFileExtension()));
                    messageValues.push_back(pContent->GetFileSize());
                    messageValues.push_back(pContent->GetFileType());                    
                }
                break;
				default:
					break;
				}
				content->m_values.push_back(messageValues);
			}
		}
		InsertToMessageList(content);
	}

	void OnQueryRoomHistoryMessageFromServer(YIMErrorcode errorcode, const XCHAR* roomID, int remain, std::list<std::shared_ptr<IYIMMessage> >& messageList) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_ROOM_HISTORY_MSG));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(roomID));
		content->m_values.push_back(remain);
		if (errorcode == YIMErrorcode_Success && messageList.size() > 0)
		{
			for (std::list< std::shared_ptr<IYIMMessage> >::iterator itr = messageList.begin(); itr != messageList.end(); ++itr)
			{
				std::vector<youmecommon::CXAny> messageValues;
				if (GetMessage((*itr).get(), messageValues))
				{
					content->m_values.push_back(messageValues);
				}
			}
		}
		InsertToMessageList(content);
	}

	void OnReceiveMessageNotify(YIMChatType chatType, const XCHAR* targetID)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_RECEIVE_MESSAGE_NITIFY));
		content->m_errorcode = YIMErrorcode_Success;
		content->m_values.push_back((int)chatType);
		content->m_values.push_back( XString(targetID));
		InsertToMessageList(content);
	}

	void OnTranslateTextComplete(YIMErrorcode errorcode, unsigned int requestID, const XCHAR* text, LanguageCode srcLangCode, LanguageCode destLangCode)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_TRANSLATE_COMPLETE));
		content->m_errorcode = errorcode;
		content->m_values.push_back(requestID);
		content->m_values.push_back( XString(text) );
		content->m_values.push_back((int)srcLangCode);
		content->m_values.push_back((int)destLangCode);
		InsertToMessageList(content);
	}


    void OnGetRecentContacts(YIMErrorcode errorcode, std::list<std::shared_ptr<IYIMContactsMessageInfo> >& contactList) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_RENCENT_CONTACTS));
		content->m_errorcode = errorcode;
		if (errorcode == YIMErrorcode_Success && contactList.size() > 0)
		{
            for (std::list< std::shared_ptr<IYIMContactsMessageInfo> >::iterator itr = contactList.begin(); itr != contactList.end(); ++itr)
            {
                IYIMContactsMessageInfo* cMessage = (*itr).get();
                std::vector<youmecommon::CXAny> contactsMesValues;
				contactsMesValues.push_back(XString(cMessage->GetContactID()));
				contactsMesValues.push_back((int)cMessage->GetMessageType());
				contactsMesValues.push_back(XString(cMessage->GetMessageContent()));
				contactsMesValues.push_back(cMessage->GetCreateTime());
                contactsMesValues.push_back(cMessage->GetNotReadMsgNum());
                
                contactsMesValues.push_back(XString(cMessage->GetLocalPath()));
                
                content->m_values.push_back(contactsMesValues);
            }

		}
		InsertToMessageList(content);
	}

	void OnQueryUserStatus(YIMErrorcode errorcode, const XCHAR*  userID, YIMUserStatus status)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_QUERY_USER_STATUS));
		content->m_errorcode = errorcode;
		content->m_values.push_back( XString(userID) );
		content->m_values.push_back((int)status);
		InsertToMessageList(content);
	}

	void OnGetUserInfo(YIMErrorcode errorcode, const XCHAR* userID, const XCHAR* userInfo)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_USR_INFO));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(userID));
		content->m_values.push_back(XString(userInfo));
		InsertToMessageList(content);
	}

	void OnPlayCompletion(YIMErrorcode errorcode, const XCHAR* path)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_AUDIO_PLAY_COMPLETE));
		content->m_errorcode = errorcode;
		content->m_values.push_back( XString(path) );
		InsertToMessageList(content);
	}

	void OnGetMicrophoneStatus(AudioDeviceStatus status)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_MICROPHONE_STATUS));
		content->m_errorcode = YIMErrorcode_Success;
		content->m_values.push_back((int)status);
		InsertToMessageList(content);
	}

	void OnUpdateLocation(YIMErrorcode errorcode, std::shared_ptr<GeographyLocation> location)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_DISTRICT));
		content->m_errorcode = errorcode;
		if (location != NULL)
		{
			content->m_values.push_back(location->GetDistrictCode());
			content->m_values.push_back(XString(location->GetCountry()));
			content->m_values.push_back(XString(location->GetProvince()));
			content->m_values.push_back(XString(location->GetCity()));
			content->m_values.push_back(XString(location->GetDistrictCounty()));
			content->m_values.push_back(XString(location->GetStreet()));
			content->m_values.push_back(location->GetLongitude());
			content->m_values.push_back(location->GetLatitude());
		}
		InsertToMessageList(content);
	}

	void OnGetNearbyObjects(YIMErrorcode errorcode, std::list< std::shared_ptr<RelativeLocation> > neighbourList, unsigned int startDistance, unsigned int endDistance)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_PEOPLE_NEARBY));
		content->m_errorcode = errorcode;
		content->m_values.push_back(startDistance);
		content->m_values.push_back(endDistance);
		if (errorcode == YIMErrorcode_Success && neighbourList.size() > 0)
		{
			for (std::list< std::shared_ptr<RelativeLocation>  >::iterator itr = neighbourList.begin(); itr != neighbourList.end(); ++itr)
			{
				RelativeLocation* location = (*itr).get();
				std::vector<youmecommon::CXAny> locationVales;

				locationVales.push_back(XString(location->GetUserID()));
				locationVales.push_back(location->GetDistance());
				locationVales.push_back(location->GetLongitude());
				locationVales.push_back(location->GetLatitude());
				locationVales.push_back(XString(location->GetCountry()));
				locationVales.push_back(XString(location->GetProvince()));
				locationVales.push_back(XString(location->GetCity()));
				locationVales.push_back(XString(location->GetDistrictCounty()));
				locationVales.push_back(XString(location->GetStreet()));

				content->m_values.push_back(locationVales);
			}
		}
		InsertToMessageList(content);
	}

	void OnGetDistance(YIMErrorcode errorcode, const XCHAR* userID, unsigned int distance) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_DISTANCE));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(userID));
		content->m_values.push_back(distance);

		InsertToMessageList(content);
	}

	void OnAccusationResultNotify(AccusationDealResult result, const XCHAR* userID, unsigned int accusationTime)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_TIPOFF_MSG));
		content->m_errorcode = YIMErrorcode_Success;
		content->m_values.push_back((int)result);
		content->m_values.push_back(XString(userID));
		content->m_values.push_back(accusationTime);

		InsertToMessageList(content);
	}


	void OnRecvNotice(YIMNotice* notice)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_RECV_NOTICE));
		content->m_errorcode = YIMErrorcode_Success;
		content->m_values.push_back(notice->GetNoticeID());
		content->m_values.push_back(XString(notice->GetChannelID()));
		content->m_values.push_back(notice->GetNoticeType());
		content->m_values.push_back(XString(notice->GetContent()));
		content->m_values.push_back(XString(notice->GetLinkText()));
		content->m_values.push_back(XString(notice->GetLinkAddr()));
		content->m_values.push_back(notice->GetBeginTime());
		content->m_values.push_back(notice->GetEndTime());

		InsertToMessageList(content);
	}

	void OnCancelNotice(XUINT64 noticeID, const XCHAR* channelID)override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_CANCEL_NOTICE));
		content->m_errorcode = YIMErrorcode_Success;
		content->m_values.push_back(noticeID);
		content->m_values.push_back(XString(channelID));
		InsertToMessageList(content);
	}
    
    void OnGetForbiddenSpeakInfo( YIMErrorcode errorcode, std::vector< std::shared_ptr<IYIMForbidSpeakInfo> > vecForbiddenSpeakInfos ) override {
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_FORBID_RECORD));
        
        content->m_errorcode = errorcode;
        
        if (errorcode == YIMErrorcode_Success && vecForbiddenSpeakInfos.size() > 0)
        {
            for (std::vector< std::shared_ptr<IYIMForbidSpeakInfo>  >::iterator itr = vecForbiddenSpeakInfos.begin(); itr != vecForbiddenSpeakInfos.end(); ++itr)
            {
                IYIMForbidSpeakInfo* info = (*itr).get();
                std::vector<youmecommon::CXAny> forbidInfo;
                
                forbidInfo.push_back(XString(info->GetChannelID()));
                forbidInfo.push_back( info->GetIsForbidRoom() );
                forbidInfo.push_back( info->GetReasonType() );
                forbidInfo.push_back( info->GetEndTime() );
                
                content->m_values.push_back(forbidInfo);
            }
        }
        
        InsertToMessageList(content);
    }

	void OnBlockUser(YIMErrorcode errorcode, const XCHAR* userID, bool block) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_SET_MASK_USER_MSG));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(userID));
		content->m_values.push_back((int)block);
		InsertToMessageList(content);
	}

	void OnUnBlockAllUser(YIMErrorcode errorcode) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_CLEAN_MASK_USER_MSG));
		content->m_errorcode = errorcode;
		InsertToMessageList(content);
	}

	void OnGetBlockUsers(YIMErrorcode errorcode, std::list<XString> userList) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_MASK_USER_MSG));
		content->m_errorcode = errorcode;
		for (std::list<XString>::const_iterator itr = userList.begin(); itr != userList.end(); ++itr)
		{
			content->m_values.push_back(*itr);
		}
		InsertToMessageList(content);
	}

	void OnRecordVolumeChange(float volume) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_RECORD_VOLUME));
		content->m_errorcode = YIMErrorcode_Success;
		content->m_values.push_back(volume);
		InsertToMessageList(content);
	}
    
    void OnStartReconnect() override
    {
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_START_RECONNECT));
        content->m_errorcode = YIMErrorcode_Success;
        InsertToMessageList(content);
    }
    
    void OnRecvReconnectResult(ReconnectResult result) override
    {
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_RECONNECT_RESULT));
        content->m_errorcode = YIMErrorcode_Success;
        content->m_values.push_back((int)result);
        InsertToMessageList(content);
    }


	virtual void OnFindUser(YIMErrorcode errorcode, std::list<std::shared_ptr<IYIMUserBriefInfo> >& users) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_FIND_FRIEND_BY_ID));
		content->m_errorcode = errorcode;
		if (errorcode == YIMErrorcode_Success && users.size() > 0)
		{
			for (std::list<std::shared_ptr<IYIMUserBriefInfo> >::iterator itr = users.begin(); itr != users.end(); ++itr)
			{
				IYIMUserBriefInfo* ubi = (*itr).get();
				std::vector<youmecommon::CXAny> userInfo;
				userInfo.push_back(XString(ubi->GetUserID()));
				userInfo.push_back(XString(ubi->GetNickname()));
				userInfo.push_back((int)ubi->GetUserStatus());
				content->m_values.push_back(userInfo);
			}
		}
		InsertToMessageList(content);
	}

	virtual void OnRequestAddFriend(YIMErrorcode errorcode, const XCHAR* userID)  override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_REQUEST_ADD_FRIEND));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(userID));
		InsertToMessageList(content);
	}

	virtual void OnBeRequestAddFriendNotify(const XCHAR* userID, const XCHAR* comments, XUINT64 reqID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_REQUEST_ADD_FRIEND_NOTIFY));
		content->m_values.push_back(XString(userID));
		content->m_values.push_back(XString(comments));
        content->m_values.push_back(reqID);
		InsertToMessageList(content);
	}

	virtual void OnBeAddFriendNotify(const XCHAR* userID, const XCHAR* comments) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_BE_ADD_FRIENT));
		content->m_values.push_back(XString(userID));
		content->m_values.push_back(XString(comments));
		InsertToMessageList(content);
	}

	virtual void OnDealBeRequestAddFriend(YIMErrorcode errorcode, const XCHAR* userID, const XCHAR* comments, int dealResult) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_DEAL_ADD_FRIEND));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(userID));
		content->m_values.push_back(XString(comments));
		content->m_values.push_back(dealResult);
		InsertToMessageList(content);
	}

	virtual void OnRequestAddFriendResultNotify(const XCHAR* userID, const XCHAR* comments, int dealResult) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_ADD_FRIENT_RESULT_NOTIFY));
		content->m_values.push_back(XString(userID));
		content->m_values.push_back(XString(comments));
		content->m_values.push_back(dealResult);
		InsertToMessageList(content);
	}

	virtual void OnDeleteFriend(YIMErrorcode errorcode, const XCHAR* userID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_DELETE_FRIEND));
		content->m_errorcode = errorcode;
		content->m_values.push_back(XString(userID));
		InsertToMessageList(content);
	}

	virtual void OnBeDeleteFriendNotify(const XCHAR* userID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_BE_DELETE_FRIEND_NOTIFY));
		content->m_values.push_back(XString(userID));
		InsertToMessageList(content);
	}

	virtual void OnBlackFriend(YIMErrorcode errorcode, int type, const XCHAR* userID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_BLACK_FRIEND));
		content->m_values.push_back(type);
		content->m_values.push_back(XString(userID));
		InsertToMessageList(content);
	}

	/*virtual void OnBeBlackNotify(const XString& userID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_BE_BLACK_FRIEND_NOTIFY));
		content->m_values.push_back(userID);
		InsertToMessageList(content);
	}*/

	virtual void OnQueryFriends(YIMErrorcode errorcode, int type, int startIndex, std::list<std::shared_ptr<IYIMUserBriefInfo> >& friends) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_QUERY_FRIEND_LIST));
		content->m_errorcode = errorcode;
		content->m_values.push_back(type);
		content->m_values.push_back(startIndex);
		if (errorcode == YIMErrorcode_Success && friends.size() > 0)
		{
			for (std::list<std::shared_ptr<IYIMUserBriefInfo> >::iterator itr = friends.begin(); itr != friends.end(); ++itr)
			{
				IYIMUserBriefInfo* ubi = (*itr).get();
				std::vector<youmecommon::CXAny> userInfo;
				userInfo.push_back(XString(ubi->GetUserID()));
				userInfo.push_back(XString(ubi->GetNickname()));
				userInfo.push_back((int)ubi->GetUserStatus());
				content->m_values.push_back(userInfo);
			}
		}
		InsertToMessageList(content);
	}

	virtual void OnQueryFriendRequestList(YIMErrorcode errorcode, int startIndex, std::list<std::shared_ptr<IYIMFriendRequestInfo> >& requestList) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_QUERY_FRIEND_REQUEST_LIST));
		content->m_errorcode = errorcode;
		content->m_values.push_back(startIndex);
		if (errorcode == YIMErrorcode_Success && requestList.size() > 0)
		{
			for (std::list<std::shared_ptr<IYIMFriendRequestInfo> >::iterator itr = requestList.begin(); itr != requestList.end(); ++itr)
			{
				IYIMFriendRequestInfo* fri = (*itr).get();
				std::vector<youmecommon::CXAny> userInfo;
				userInfo.push_back(XString(fri->GetAskerID()));
				userInfo.push_back(XString(fri->GetAskerNickname()));
				userInfo.push_back(XString(fri->GetInviteeID()));
				userInfo.push_back(XString(fri->GetInviteeNickname()));
				userInfo.push_back(XString(fri->GetValidateInfo()));
				userInfo.push_back((int)fri->GetStatus());
				userInfo.push_back(fri->GetCreateTime());
                userInfo.push_back(fri->GetReqID());
				content->m_values.push_back(userInfo);
			}
		}
		InsertToMessageList(content);
	}
    
	virtual void OnQueryUserInfo(YIMErrorcode errorcode, const IMUserProfileInfo &userInfo) override
    {
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_GET_USER_PROFILE));
        content->m_errorcode = errorcode;
        content->m_values.push_back(userInfo.userID);
        content->m_values.push_back(userInfo.photoURL);
        content->m_values.push_back((int)userInfo.onlineState);
        content->m_values.push_back((int)userInfo.beAddPermission);
        content->m_values.push_back((int)userInfo.foundPermission);
        content->m_values.push_back(userInfo.settingInfo.nickName);
        content->m_values.push_back((int)userInfo.settingInfo.sex);
        content->m_values.push_back(userInfo.settingInfo.personalSignature);
        content->m_values.push_back(userInfo.settingInfo.country);
        content->m_values.push_back(userInfo.settingInfo.province);
        content->m_values.push_back(userInfo.settingInfo.city);
        content->m_values.push_back(userInfo.settingInfo.extraInfo);
        
        InsertToMessageList(content);
    }
    
	virtual void OnSetUserInfo(YIMErrorcode errorcode) override
    {        
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_SET_USER_PROFILE));
        content->m_errorcode = errorcode;
        InsertToMessageList(content);
    }

    virtual void OnSwitchUserOnlineState(YIMErrorcode errorcode) override
    {        
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_SWITCH_USER_STATE));
        content->m_errorcode = errorcode;
        InsertToMessageList(content);
    }

    virtual void OnSetPhotoUrl(YIMErrorcode errorcode, const XCHAR* photoUrl) override
    {        
        std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_SET_USER_PHOTO));
        content->m_errorcode = errorcode;
        content->m_values.push_back(XString(photoUrl));
        InsertToMessageList(content);
    }

	virtual void OnUserInfoChangeNotify(const XCHAR* userID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YOUMEServiceProtocol::CMD_HXR_USER_INFO_CHANGE_NOTIFY));
		content->m_values.push_back(XString(userID));
		InsertToMessageList(content);
	}
};
//一个全局的回掉
InterCallback* g_InterCallback = NULL;

int IM_Init(const XCHAR* appKey, const XCHAR* appSecurity)
{
	if ((NULL == appKey) || (NULL == appSecurity))
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (s_YouMeInit)
	{
		return YIMErrorcode_Success;
	}
	g_parserList.clear();
    /*if (g_pIMManager != NULL)
    {
        YIMManager::DestroyInstance();
        g_pIMManager = NULL;
    }*/
	delete g_messageSemap;
	g_messageSemap = new youmecommon::CXSemaphore;
	delete g_contentMutex;
	g_contentMutex = new std::mutex;
	g_pIMManager = YIMManager::CreateInstance();
	YIMErrorcode errorcode = g_pIMManager->Init(appKey, appSecurity, __XT(""));
	if (errorcode == YIMErrorcode_Success)
	{
		g_InterCallback = new InterCallback;
		s_YouMeInit = true;
	}

	return errorcode;
}

void IM_Uninit()
{
	if (!s_YouMeInit)
	{
		return;
	}
    g_messageSemap->Increment();
	if (g_pIMManager != NULL)
	{
		//g_pIMManager->UnInit();
        g_pIMManager->SetLoginCallback(NULL);
        g_pIMManager->SetMessageCallback(NULL);
        g_pIMManager->SetChatRoomCallback(NULL);
        g_pIMManager->SetDownloadCallback(NULL);
        g_pIMManager->SetContactCallback(NULL);
		g_pIMManager->SetAudioPlayCallback(NULL);
		g_pIMManager->SetLocationCallback(NULL);
		g_pIMManager->SetNoticeCallback(NULL);
        g_pIMManager->SetReconnectCallback(NULL);
		g_pIMManager->SetUpdateReadStatusCallback(NULL);

        //YIMManager::DestroyInstance();
        //g_pIMManager = NULL;
    }
    if (g_InterCallback != NULL)
    {
		delete g_InterCallback;
		g_InterCallback = NULL;
	}
	g_parserList.clear();
    s_YouMeInit = false;
}

void IM_UpdateReadStatusCallbackFlag(bool flag) {
	g_updateReadStatusFlag = flag;
}

int IM_SendTextMessage(const XCHAR* receiverID, YIMChatType chatType, const XCHAR* text, const XCHAR* attachParam, XUINT64* requestID)
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
        return YIMErrorcode_NotLogin;
    }
	XUINT64 ullSerial = 0;
	YIMErrorcode errorcode = pMessageManger->SendTextMessage(receiverID, chatType, text, attachParam, &ullSerial);
    if (errorcode == YIMErrorcode_Success) {
		if (NULL != requestID)
		{
			*requestID = ullSerial;
		}
    }
    return errorcode;
}
int IM_SendCustomMessage(const XCHAR* receiverID, YIMChatType chatType, const char* content, unsigned int size, XUINT64* requestID)
{
     if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YIMErrorcode_NotLogin;
    }
    XUINT64 ullSerial = 0;
	YIMErrorcode errorcode = pMessageManger->SendCustomMessage(receiverID, chatType, content, size, &ullSerial);
	if (errorcode == YIMErrorcode_Success && NULL != requestID) {
		*requestID = ullSerial;
    }
    return errorcode;
}
int IM_Logout()
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    CleanMessage();
    g_messageSemap->Increment();
	return g_pIMManager->Logout();
}

int IM_Login(const XCHAR* userID, const XCHAR* password, const XCHAR* token)
{
	if (NULL == userID || NULL == password)
	{
		return YIMErrorcode_ParamInvalid;
	}
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    g_pIMManager->SetLoginCallback(g_InterCallback);
    g_pIMManager->SetMessageCallback(g_InterCallback);
    g_pIMManager->SetChatRoomCallback(g_InterCallback);
    g_pIMManager->SetDownloadCallback(g_InterCallback);
    g_pIMManager->SetContactCallback(g_InterCallback);
	g_pIMManager->SetAudioPlayCallback(g_InterCallback);
	g_pIMManager->SetLocationCallback(g_InterCallback);
	g_pIMManager->SetNoticeCallback(g_InterCallback);
    g_pIMManager->SetReconnectCallback(g_InterCallback);
    g_pIMManager->SetUserProfileCallback(g_InterCallback);
    g_pIMManager->SetFriendCallback(g_InterCallback);

	if (g_updateReadStatusFlag) {
		g_pIMManager->SetUpdateReadStatusCallback(g_InterCallback);
	}

	YIMErrorcode errorcode = g_pIMManager->Login(userID, password, token);
    if (errorcode == YIMErrorcode_Success) {
        CleanMessage();
    }
    return errorcode;
}

int IM_JoinChatRoom(const XCHAR* chatRoomID)
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMChatRoomManager* pGroupManger = g_pIMManager->GetChatRoomManager();
    if (NULL == pGroupManger) {
		return YIMErrorcode_NotLogin;
    }
   return pGroupManger->JoinChatRoom(chatRoomID);
}
int IM_LeaveChatRoom(const XCHAR* chatRoomID)
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMChatRoomManager* pGroupManger = g_pIMManager->GetChatRoomManager();
    if (NULL == pGroupManger) {
		return YIMErrorcode_NotLogin;
    }
    return pGroupManger->LeaveChatRoom(chatRoomID);
}

int IM_LeaveAllChatRooms()
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMChatRoomManager* pGroupManger = g_pIMManager->GetChatRoomManager();
    if (NULL == pGroupManger) {
        return YIMErrorcode_NotLogin;
    }
    return pGroupManger->LeaveAllChatRooms();
}

const XCHAR* IM_GetMessage()
{
	if (NULL == g_messageSemap)
	{
		return NULL;
	}
    g_messageSemap->Decrement();
    return HeadMessage();
}

void IM_PopMessage(const XCHAR*pszBuffer)
{
	delete[]pszBuffer;
}

int IM_SendAudioMessage(const XCHAR* receiverID, YIMChatType chatType, XUINT64* requestID)
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YIMErrorcode_NotLogin;
    }
    XUINT64 ullSerial = 0;
	YIMErrorcode errorcode = pMessageManger->SendAudioMessage(receiverID, chatType, &ullSerial);
    if (errorcode == YIMErrorcode_Success) {
		if (NULL != requestID)
		{
			*requestID = ullSerial;
		}
    }
    return errorcode;

}


int IM_SendOnlyAudioMessage(const XCHAR* receiverID, YIMChatType chatType, XUINT64* requestID)
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
        return YIMErrorcode_NotLogin;
    }
    XUINT64 ullSerial = 0;
	YIMErrorcode errorcode = pMessageManger->SendOnlyAudioMessage(receiverID, chatType, &ullSerial);
    if (errorcode == YIMErrorcode_Success) {
		if (NULL != requestID)
		{
			*requestID = ullSerial;
		}
		
    }
    return errorcode;
    
}

YOUMEDLL_API int IM_DownloadAudioFileSync(XUINT64 serial, const XCHAR* savePath)
{
	if (NULL == g_pIMManager) {
		return YIMErrorcode_EngineNotInit;
	}
	YouMeIMMessageManager* pMessageManger = (YouMeIMMessageManager*)g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger) {
		return YIMErrorcode_NotLogin;
	}
	return pMessageManger->DownloadFileSync(serial, savePath);
}

int IM_DownloadFile(XUINT64 serial, const XCHAR* savePath)
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YIMErrorcode_NotLogin;
    }
	return pMessageManger->DownloadFile(serial, savePath);
}

YOUMEDLL_API int IM_DownloadFileByURL(const XCHAR* downloadURL, const XCHAR* savePath, YIMFileType fileType)
{
	if (NULL == g_pIMManager) {
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger) {
		return YIMErrorcode_NotLogin;
	}
	return pMessageManger->DownloadFile(downloadURL, savePath, fileType);
}

int IM_StopAudioMessage(const XCHAR* extraParam)
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YIMErrorcode_NotLogin;
    }
    
	return (int)pMessageManger->StopAudioMessage(extraParam);
}
 int IM_CancleAudioMessage()
{
    if (NULL == g_pIMManager) {
        return YIMErrorcode_EngineNotInit;
    }
    YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YIMErrorcode_NotLogin;
    }
    return (int)pMessageManger->CancleAudioMessage();

}

int IM_GetSDKVer()
{
    return YIMManager::GetSDKVersion();
}
void IM_SetLogLevel(int iLogLevel, int iConsoleLogLevel)
{
    YouMe_LOG_SetLevel((YouMe_LOG_LEVEL)iLogLevel,(YouMe_LOG_LEVEL)iConsoleLogLevel);
}

void IM_SetMode(int mode)
{
	YIMManager::SetMode(mode);
}

void IM_SetAppId(int appId) {
	YIMManager::SetAppId(appId);
}

YOUMEDLL_API XCHAR* IM_GetFilterText(const XCHAR* text, int* level)
{
	XString strResult = YIMManager::FilterKeyword(text, level);
	XCHAR* pResult = new XCHAR[strResult.length()+1];
	XStrCpy_S(pResult, int(strResult.length() + 1),strResult.c_str());
	return pResult;
}

YOUMEDLL_API void IM_DestroyFilterText(XCHAR* text)
{
	delete[] text;
}

YOUMEDLL_API void IM_SetServerZone(ServerZone zone)
{
	YIMManager::SetServerZone(zone);
}

YOUMEDLL_API void IM_SetAudioCacheDir(const XCHAR* audioCacheDir)
{
	YIMManager::SetAudioCacheDir(audioCacheDir);
}

YOUMEDLL_API int IM_StartAudioSpeech(XUINT64* requestID, bool translate)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
	YIMErrorcode errorcode = pMessageManger->StartAudioSpeech(requestID, translate);
	return errorcode;
}
YOUMEDLL_API int IM_ConvertAMRToWav(const XCHAR* amrFilePath, const XCHAR* wavFielPath)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}

	return pMessageManger->ConvertAMRToWav(amrFilePath, wavFielPath);
}

YOUMEDLL_API int IM_QueryRoomHistoryMessageFromServer(const XCHAR* chatRoomID, int count, int direction)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}

	return pMessageManger->QueryRoomHistoryMessageFromServer(chatRoomID, count, direction);
}

YOUMEDLL_API int IM_DeleteHistoryMessage(YIMChatType chatType, XUINT64 time)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}

	return pMessageManger->DeleteHistoryMessage(chatType,time);
}

YOUMEDLL_API int IM_DeleteHistoryMessageByTarget(const XCHAR* targetID, int chatType, XUINT64 startMessageID, unsigned int count)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}

	return pMessageManger->DeleteHistoryMessage(targetID, (YIMChatType)chatType, startMessageID, count);
}

YOUMEDLL_API int IM_DeleteHistoryMessageByID(XUINT64 messageID)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}

	return pMessageManger->DeleteHistoryMessageByID(messageID);
}

//清除指定用户的本地消息历史记录,保留消息ID列表的记录
YOUMEDLL_API int IM_DeleteSpecifiedHistoryMessage(const XCHAR* targetID, YIMChatType chatType, XUINT64* messageID, int num)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetMessageManager())
    {
        return YIMErrorcode_NotLogin;
    }
    std::vector<XUINT64> vecMes;
    for (int i = 0; i < num; i++)
    {
        vecMes.push_back(*messageID);
        messageID++;
    }
    return g_pIMManager->GetMessageManager()->DeleteSpecifiedHistoryMessage(targetID, chatType, vecMes);
}

YOUMEDLL_API int IM_QueryHistoryMessage(const XCHAR* targetID, int chatType, XUINT64 startMessageID, int count, int direction)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}

	return pMessageManger->QueryHistoryMessage(targetID, (YIMChatType)chatType, startMessageID, count, direction);
}

YOUMEDLL_API int IM_StopAudioSpeech()
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
	return pMessageManger->StopAudioSpeech();
}

YOUMEDLL_API int IM_GetRecentContacts()
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetRecentContacts();
}

int IM_SendGift(const XCHAR* anchorID, const XCHAR* channel, int giftId, int giftCount, const char* extraParam, XUINT64* requestID)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
	return pMessageManger->SendGift(anchorID, channel, giftId, giftCount, extraParam, requestID);
}


YOUMEDLL_API const XCHAR* IM_GetMessage2()
{
	return HeadMessage();
}

YOUMEDLL_API int IM_MultiSendTextMessage(const char* receivers, const XCHAR* text)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
    
    std::vector<XString> vecReceivers;
    youmecommon::Value receiverValue;
    youmecommon::Reader jsonReader;
    if (jsonReader.parse(receivers, receiverValue))
    {
        for( int i = 0 ; i < (int)receiverValue.size(); i++ ){
            vecReceivers.push_back( UTF8TOXString( receiverValue[i].asString()));
        }
    }
    
	return pMessageManger->MultiSendTextMessage(vecReceivers, text);
}

YOUMEDLL_API void IM_OnPause(bool pauseReceiveMessage)
{
	if (NULL == g_pIMManager)
	{
		return;
	}
	g_pIMManager->OnPause(pauseReceiveMessage);
}

YOUMEDLL_API void IM_OnResume()
{
	if (NULL == g_pIMManager)
	{
		return;
	}
	g_pIMManager->OnResume();
}

YOUMEDLL_API int IM_SetDownloadAudioMessageSwitch(bool download)
{
    if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
    return g_pIMManager->GetMessageManager()->SetDownloadAudioMessageSwitch(download);
}

YOUMEDLL_API int IM_SetReceiveMessageSwitch(const XCHAR* targets, bool receive)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
    
    std::vector<XString> vecTargets;
    youmecommon::Value targetsValue;
    youmecommon::Reader jsonReader;
    if (jsonReader.parse( XStringToUTF8(XString(targets)) , targetsValue))
    {
        for( int i = 0 ; i < (int)targetsValue.size(); i++ ){
            vecTargets.push_back( UTF8TOXString( targetsValue[i].asString()));
        }
    }

	pMessageManger->SetReceiveMessageSwitch(vecTargets, receive);
	return YIMErrorcode_Success;
}

YOUMEDLL_API int IM_GetNewMessage(const XCHAR* targets)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
    
    std::vector<XString> vecTargets;
    youmecommon::Value targetsValue;
    youmecommon::Reader jsonReader;
    if (jsonReader.parse(XStringToUTF8(XString(targets)), targetsValue))
    {
        for( int i = 0 ; i < (int)targetsValue.size(); i++ ){
            vecTargets.push_back( UTF8TOXString( targetsValue[i].asString()));
        }
    }

	pMessageManger->GetNewMessage(vecTargets);
	return YIMErrorcode_Success;
}

YOUMEDLL_API int IM_SendFile(const XCHAR* receiverID, YIMChatType chatType, const XCHAR* filePath, const XCHAR* extraParam,YIMFileType fileType, XUINT64* requestID)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
	return pMessageManger->SendFile(receiverID, chatType, filePath, requestID, extraParam, fileType);
}

YOUMEDLL_API int IM_SetUserInfo(const XCHAR* userInfo)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->SetUserInfo(userInfo);
}

YOUMEDLL_API int IM_GetUserInfo(const XCHAR* userID)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetUserInfo(userID);
}

XCHAR* IM_GetAudioCachePath()
{
	if (NULL == g_pIMManager)
	{
		return NULL;
	}
	XString path = g_pIMManager->GetAudioCachePath();
	XCHAR* pResult = new XCHAR[path.length() + 1];
	XStrCpy_S(pResult, int(path.length() + 1), path.c_str());
	return pResult;
}

YOUMEDLL_API void IM_DestroyAudioCachePath(XCHAR* path)
{
	delete[] path;
}

bool IM_ClearAudioCachePath()
{
	if (NULL == g_pIMManager)
	{
		return false;
	}

	return g_pIMManager->ClearAudioCachePath();
}

YOUMEDLL_API int IM_QueryUserStatus(const XCHAR* userID)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	g_pIMManager->QueryUserStatus(userID);
	return YIMErrorcode_Success;
}

YOUMEDLL_API int IM_SetRoomHistoryMessageSwitch(const XCHAR* roomIDs, bool save)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
    
    std::vector<XString> vecTargets;
    youmecommon::Value targetsValue;
    youmecommon::Reader jsonReader;
    if (jsonReader.parse(XStringToUTF8(XString(roomIDs)), targetsValue))
    {
        for( int i = 0 ; i < (int)targetsValue.size(); i++ ){
            vecTargets.push_back( UTF8TOXString( targetsValue[i].asString()));
        }
    }

    
	return pMessageManger->SetRoomHistoryMessageSwitch(vecTargets, save);
}


YOUMEDLL_API void IM_SetVolume(float volume)
{
	if (NULL == g_pIMManager)
	{
		return;
	}
	return g_pIMManager->SetVolume(volume);
}

YOUMEDLL_API int IM_StartPlayAudio(const XCHAR* path)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->StartPlayAudio(path);
}

YOUMEDLL_API int IM_StopPlayAudio()
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->StopPlayAudio();
}

YOUMEDLL_API bool IM_IsPlaying()
{
	if (NULL == g_pIMManager)
	{
		return false;
	}
	return g_pIMManager->IsPlaying();
}

YOUMEDLL_API int IM_TranslateText(unsigned int* requestID, const XCHAR* text, LanguageCode destLangCode, LanguageCode srcLangCode)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger)
	{
		return YIMErrorcode_NotLogin;
	}
	return pMessageManger->TranslateText(requestID, text, destLangCode, srcLangCode);
}

YOUMEDLL_API int IM_GetCurrentLocation()
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetLocationManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetLocationManager()->GetCurrentLocation();
}

YOUMEDLL_API int IM_GetNearbyObjects(int count, const XCHAR* serverAreaID, DistrictLevel districtlevel, bool resetStartDistance)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetLocationManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetLocationManager()->GetNearbyObjects(count, serverAreaID, districtlevel, resetStartDistance);
}

YOUMEDLL_API void IM_SetUpdateInterval(unsigned int interval)
{
	if (NULL == g_pIMManager)
	{
		return;
	}
	if (NULL == g_pIMManager->GetLocationManager())
	{
		return;
	}
	g_pIMManager->GetLocationManager()->SetUpdateInterval(interval);
}

YOUMEDLL_API void IM_SetKeepRecordModel(bool keep)
{
	if (NULL == g_pIMManager)
	{
		return;
	}
	return g_pIMManager->SetKeepRecordModel(keep);
}

YOUMEDLL_API void GetMicrophoneStatus()
{
	if (g_pIMManager != NULL)
	{
		g_pIMManager->GetMicrophoneStatus();
	}
}

YOUMEDLL_API int IM_Accusation(const XCHAR* userID, YIMChatType source, int reason, const XCHAR* description, const XCHAR* extraParam)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetMessageManager())
	{
		return YIMErrorcode_NotLogin;
	}
	return g_pIMManager->GetMessageManager()->Accusation(userID, source, reason, description, extraParam);
}

YOUMEDLL_API int IM_QueryNotice()
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->QueryNotice();
}

YOUMEDLL_API int IM_GetForbiddenSpeakInfo(){
    if( NULL == g_pIMManager ){
        return YIMErrorcode_EngineNotInit;
    }
    
    
    return g_pIMManager->GetMessageManager()->GetForbiddenSpeakInfo();
}

YOUMEDLL_API int IM_SetSpeechRecognizeLanguage(int language)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetMessageManager())
	{
		return YIMErrorcode_NotLogin;
	}
	return g_pIMManager->GetMessageManager()->SetSpeechRecognizeLanguage((SpeechLanguage)language);
}

//设置仅识别语音文字，不发送语音消息; false:识别语音文字并发送语音消息，true:仅识别语音文字
YOUMEDLL_API int IM_SetOnlyRecognizeSpeechText(bool recognition)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetMessageManager())
    {
        return YIMErrorcode_NotLogin;
    }
    return g_pIMManager->GetMessageManager()->SetOnlyRecognizeSpeechText(recognition);
}
YOUMEDLL_API int IM_GetMicrophoneStatus()
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	g_pIMManager->GetMicrophoneStatus();
	return YIMErrorcode_Success;
}

YOUMEDLL_API int IM_SetMessageRead(XUINT64 messageID, bool read)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetMessageManager())
	{
		return YIMErrorcode_NotLogin;
	}
	return g_pIMManager->GetMessageManager()->SetMessageRead(messageID, read);
}

YOUMEDLL_API int IM_SendMessageReadStatus(const XCHAR* userID, int chatType, XUINT64 messageID)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetMessageManager())
	{
		return YIMErrorcode_NotLogin;
	}
	return g_pIMManager->GetMessageManager()->SendMessageReadStatus(userID, chatType, messageID);
}

YOUMEDLL_API int IM_SetAllMessageRead(const XCHAR* userID, bool read)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetMessageManager())
    {
        return YIMErrorcode_NotLogin;
    }
    return g_pIMManager->GetMessageManager()->SetAllMessageRead(userID, read);
}

YOUMEDLL_API int IM_SetVoiceMsgPlayed(XUINT64 messageID, bool played)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetMessageManager())
    {
        return YIMErrorcode_NotLogin;
    }
    return g_pIMManager->GetMessageManager()->SetVoiceMsgPlayed(messageID, played);
}

YOUMEDLL_API int IM_BlockUser(const XCHAR* userID, bool block)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetMessageManager())
	{
		return YIMErrorcode_NotLogin;
	}
	return g_pIMManager->GetMessageManager()->BlockUser(userID, block);
}

YOUMEDLL_API int IM_UnBlockAllUser()
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetMessageManager())
	{
		return YIMErrorcode_NotLogin;
	}
	return g_pIMManager->GetMessageManager()->UnBlockAllUser();
}

YOUMEDLL_API int IM_GetBlockUsers()
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetMessageManager())
	{
		return YIMErrorcode_NotLogin;
	}
	return g_pIMManager->GetMessageManager()->GetBlockUsers();
}

YOUMEDLL_API int IM_SwitchMsgTransType(YIMMsgTransType transType)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetMessageManager())
    {
        return YIMErrorcode_NotLogin;
    }
    return g_pIMManager->GetMessageManager()->SwitchMsgTransType(transType);
}

YOUMEDLL_API int IM_GetRoomMemberCount(const XCHAR* chatRoomID)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	YIMChatRoomManager* pGroupManger = g_pIMManager->GetChatRoomManager();
	if (NULL == pGroupManger)
	{
		return YIMErrorcode_NotLogin;
	}
	return pGroupManger->GetRoomMemberCount(chatRoomID);
}

YOUMEDLL_API int IM_SetDownloadDir(const XCHAR* path)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetMessageManager())
	{
		return YIMErrorcode_NotLogin;
	}
	return g_pIMManager->GetMessageManager()->SetDownloadDir(path);
}

YOUMEDLL_API int IM_GetDistance(const XCHAR* userID)
{
	if (NULL == g_pIMManager)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (NULL == g_pIMManager->GetLocationManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetLocationManager()->GetDistance(userID);
}

YOUMEDLL_API int IM_FindUser(int findType, const XCHAR* target)
{
	if (NULL == g_pIMManager || NULL == g_pIMManager->GetFriendManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetFriendManager()->FindUser(findType, target);
}

YOUMEDLL_API int IM_RequestAddFriend(const XCHAR* users, const XCHAR* comments)
{
	if (NULL == g_pIMManager || NULL == g_pIMManager->GetFriendManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	std::vector<XString> vecUser;
	youmecommon::Value usersValue;
	youmecommon::Reader jsonReader;
	std::string strUsers = XStringToUTF8(XString(users));
	if (jsonReader.parse(strUsers, usersValue))
	{
		for (int i = 0; i < (int)usersValue.size(); i++)
		{
			vecUser.push_back(UTF8TOXString(usersValue[i].asString()));
		}
	}
	return g_pIMManager->GetFriendManager()->RequestAddFriend(vecUser, comments);
}

YOUMEDLL_API int IM_DealAddFriend(const XCHAR* userID, int dealResult, XUINT64 reqID)
{
	if (NULL == g_pIMManager || NULL == g_pIMManager->GetFriendManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetFriendManager()->DealBeRequestAddFriend(userID, dealResult, reqID);
}

YOUMEDLL_API int IM_DeleteFriend(const XCHAR* users, int deleteType)
{
	if (NULL == g_pIMManager || NULL == g_pIMManager->GetFriendManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	std::vector<XString> vecUser;
	youmecommon::Value usersValue;
	youmecommon::Reader jsonReader;
	std::string strUsers = XStringToUTF8(XString(users));
	if (jsonReader.parse(strUsers, usersValue))
	{
		for (int i = 0; i < (int)usersValue.size(); i++)
		{
			vecUser.push_back(UTF8TOXString(usersValue[i].asString()));
		}
	}
	return g_pIMManager->GetFriendManager()->DeleteFriend(vecUser, deleteType);
}

YOUMEDLL_API int IM_BlackFriend(int type, const XCHAR* users)
{
	if (NULL == g_pIMManager || NULL == g_pIMManager->GetFriendManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	std::vector<XString> vecUser;
	youmecommon::Value usersValue;
	youmecommon::Reader jsonReader;
	std::string strUsers = XStringToUTF8(XString(users));
	if (jsonReader.parse(strUsers, usersValue))
	{
		for (int i = 0; i < (int)usersValue.size(); i++)
		{
			vecUser.push_back(UTF8TOXString(usersValue[i].asString()));
		}
	}
	return g_pIMManager->GetFriendManager()->BlackFriend(type, vecUser);
}

YOUMEDLL_API int IM_QueryFriends(int type, int startIndex, int count)
{
	if (NULL == g_pIMManager || NULL == g_pIMManager->GetFriendManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetFriendManager()->QueryFriends(type, startIndex, count);
}

YOUMEDLL_API int IM_QueryFriendRequestList(int startIndex, int count)
{
	if (NULL == g_pIMManager || NULL == g_pIMManager->GetFriendManager())
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pIMManager->GetFriendManager()->QueryFriendRequestList(startIndex, count);
}

//设置用户基本资料
YOUMEDLL_API int IM_SetUserProfileInfo(const XCHAR* profileInfo)
{
    youmecommon::Value userInfoValue;
    youmecommon::Value tmpValue;
    youmecommon::Reader jsonReader;
    std::string strProfileInfo = XStringToUTF8(XString(profileInfo));
    IMUserSettingInfo settingInfo;
    if (jsonReader.parse(strProfileInfo, userInfoValue))
    {
        tmpValue = userInfoValue.get("NickName", "");
        settingInfo.nickName = UTF8TOXString(tmpValue.asString());
        tmpValue = userInfoValue.get("Sex", "");
        int tmpSex = CStringUtilT<char>::str_to_sint32(tmpValue.asString());
        settingInfo.sex = (IMUserSex)tmpSex;
        tmpValue = userInfoValue.get("Signature", "");
        settingInfo.personalSignature = UTF8TOXString(tmpValue.asString());
        tmpValue = userInfoValue.get("Country", "");
        settingInfo.country = UTF8TOXString(tmpValue.asString());
        tmpValue = userInfoValue.get("Province", "");
        settingInfo.province = UTF8TOXString(tmpValue.asString());
        tmpValue = userInfoValue.get("City", "");
        settingInfo.city = UTF8TOXString(tmpValue.asString());
        tmpValue = userInfoValue.get("ExtraInfo", "");
        settingInfo.extraInfo = UTF8TOXString(tmpValue.asString());
        
    }else{
        YouMe_LOG_Error(__XT("parse json fail."));
    }
    return g_pIMManager->GetUserProfileManager()->SetUserProfileInfo(settingInfo);
}

//设置用户头像
YOUMEDLL_API int IM_SetUserProfilePhoto(const XCHAR* photoPath)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetUserProfileManager())
    {
        return YIMErrorcode_NotLogin;
    }
    return g_pIMManager->GetUserProfileManager()->SetUserProfilePhoto(photoPath);
}
//获取用户基本资料
YOUMEDLL_API int IM_GetUserProfileInfo(const XCHAR* userID)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetUserProfileManager())
    {
        return YIMErrorcode_NotLogin;
    }
    return g_pIMManager->GetUserProfileManager()->GetUserProfileInfo(userID);
}
//切换用户状态
YOUMEDLL_API int IM_SwitchUserStatus(const XCHAR* userID, YIMUserStatus userStatus)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetUserProfileManager())
    {
        return YIMErrorcode_NotLogin;
    }
    return g_pIMManager->GetUserProfileManager()->SwitchUserStatus(userID,userStatus);
}
//设置好友添加权限
YOUMEDLL_API int IM_SetAddPermission(bool beFound, IMUserBeAddPermission beAddPermission)
{
    if (NULL == g_pIMManager)
    {
        return YIMErrorcode_EngineNotInit;
    }
    if (NULL == g_pIMManager->GetUserProfileManager())
    {
        return YIMErrorcode_NotLogin;
    }
    return g_pIMManager->GetUserProfileManager()->SetAddPermission(beFound, beAddPermission);
}

YOUMEDLL_API void IM_ResizeImage(const XCHAR* srcImagePath, const XCHAR* resizedSavePath)
{
	std::string strImagePath = XStringToUTF8(XString(srcImagePath));
	std::string strSavePath = XStringToUTF8(XString(resizedSavePath));
	//YouMe_ResizeImage(strImagePath, strSavePath);
}

YOUMEDLL_API int IM_SetLoginAddress(const char* ip, unsigned short port)
{
	return YIMManager::SetLoginAddress(ip, port);
}

#endif
