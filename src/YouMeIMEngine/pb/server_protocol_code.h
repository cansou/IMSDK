#ifndef SERVER_PROTOCOL_CODE_H
#define SERVER_PROTOCOL_CODE_H

namespace YOUMEServiceProtocol
{
	enum COMMOND_TYPE
	{
		CMD_UNKNOW = 0,
		CMD_LOGIN = 1,
		CMD_HEARTBEAT = 2,
		CMD_LOGOUT = 3,
		CMD_ENTER_ROOM = 4,
		CMD_LEAVE_ROOM = 5,
		CMD_SND_TEXT_MSG = 6,
		CMD_SND_VOICE_MSG = 7,
		CMD_SND_FILE_MSG = 8,
		CMD_GET_MSG = 9,
		CMD_GET_UPLOAD_TOKEN = 10,
		CMD_KICK_OFF = 11,
		CMD_SND_BIN_MSG = 12,
		CMD_RELOGIN = 13,
		CMD_CHECK_ONLINE = 14,
		CMD_SND_GIFT_MSG = 15,
		CMD_GET_ROOM_HISTORY_MSG = 16,
		CMD_GET_USR_INFO = 17,
		CMD_UPDATE_USER_INFO = 18,
		CMD_SND_TIPOFF_MSG = 19,
		CMD_GET_TIPOFF_MSG = 20,
		CMD_GET_DISTRICT = 21,
		CMD_GET_PEOPLE_NEARBY = 22,
		CMD_QUERY_NOTICE = 23,
		CMD_SET_MASK_USER_MSG = 24,				// 屏蔽用户
		CMD_GET_MASK_USER_MSG = 25,				// 获取屏蔽用户
		CMD_CLEAN_MASK_USER_MSG = 26,			// 解除所有屏蔽用户
		CMD_GET_ROOM_INFO = 27,					// 获取房间信息(人数)
        CMD_LEAVE_ALL_ROOM = 28,				// 离开所有房间
		CMD_GET_WEIXIN_TOKEN = 29,				// 获取微信token（H5语音消息）

        CMD_GET_FORBID_RECORD = 31,
        
        //关系链管理增加
        CMD_REGISTER_USER_PROFILE = 36,
        CMD_QUERY_USER_PROFILE = 37,
        CMD_UPDATE_USER_PROFILE = 38,
        CMD_UPDATE_ONLINE_STATUS = 39,
		CMD_FIND_FRIEND_BY_ID = 40,				// 按ID查找好友
		CMD_FIND_FRIEND_BY_NICKNAME = 41,		// 按昵称查找好友
		CMD_REQUEST_ADD_FRIEND = 42,			// 请求添加好友
		CMD_FRIND_NOTIFY = 44,					// 好友请求通知
		CMD_DELETE_FRIEND = 45,					// 删除好友
		CMD_BLACK_FRIEND = 46,					// 拉黑好友
		CMD_UNBLACK_FRIEND = 47,				// 解除黑名单
		CMD_DEAL_ADD_FRIEND = 48,				// 好友验证
		CMD_QUERY_FRIEND_LIST = 49,				// 获取好友列表
		CMD_QUERY_BLACK_FRIEND_LIST = 50,		// 获取黑名单列表
		CMD_QUERY_FRIEND_REQUEST_LIST = 51,		// 获取好友验证消息列表
		CMD_UPDATE_FRIEND_RQUEST_STATUS = 52,	// 更新好友请求状态
		CMD_RELATION_CHAIN_HEARTBEAT = 53,		// 关系链心跳

		CMD_SND_READ_STATUS = 56,                //设置消息已读状态
		CMD_SYN_READ_STATUS = 57,				 // 同步服务器消息已读状态
		CMD_HXR_USER_INFO_CHANGE_NOTIFY = 74,	// 用户信息变更通知
		CMD_UPLOAD_PROGRESS = 75,				// 文件上传显示进度
		CMD_SET_READ_STATUS = 76,			    // 发送端设置消息已读

		CMD_GET_ALI_TOKEN = 90,

		//服务器通知
		NOTIFY_LOGIN = 10001,
		NOTIFY_PRIVATE_MSG,
		NOTIFY_ROOM_MSG,
		NOTIFY_ROOM_GENERAL,
		NOTIFY_READ_MSG,

		NOTIFY_UPDATE_CONFIG = 10008,

		NOTIFY_PRIVATE_MSG_V2 = 10012,
		NOTIFY_ROOM_MSG_V2 = 10013,

		NOTIFY_MAX_ID = 19999,	// 服务端通知最大ID


		//客户端(C接口使用)
		CMD_DOWNLOAD = 20001,
		CMD_SEND_MESSAGE_STATUS = 20002,
		CMD_RECV_MESSAGE = 20003,
		CMD_STOP_AUDIOSPEECH = 20004,
		CMD_QUERY_HISTORY_MESSAGE = 20005,
		CMD_GET_RENCENT_CONTACTS = 20006,
		CMD_RECEIVE_MESSAGE_NITIFY = 20007,
		CMD_QUERY_USER_STATUS = 20008,
		CMD_AUDIO_PLAY_COMPLETE = 20009,
		CMD_STOP_SEND_AUDIO = 20010,
		CMD_TRANSLATE_COMPLETE = 20011,
		CMD_DOWNLOAD_URL = 20012,
		CMD_GET_MICROPHONE_STATUS = 20013,
		CMD_USER_ENTER_ROOM = 20014,
		CMD_USER_LEAVE_ROOM = 20015,
		CMD_RECV_NOTICE = 20016,
		CMD_CANCEL_NOTICE = 20017,
        
		CMD_GET_SPEECH_TEXT = 20018,			// 仅需要语音的文字识别内容
		CMD_GET_RECONNECT_RESULT = 20019,		// 重连结果
		CMD_START_RECONNECT = 20020,			// 开始重连
		CMD_RECORD_VOLUME = 20021,				// 音量
		CMD_GET_DISTANCE = 20022,				// 获取双方地理位置距离
		CMD_REQUEST_ADD_FRIEND_NOTIFY = 20023,	// 请求添加好友通知
		CMD_ADD_FRIENT_RESULT_NOTIFY = 20024,	// 添加好友结果通知
		CMD_BE_ADD_FRIENT = 20025,				// 被好友添加通知
		CMD_BE_DELETE_FRIEND_NOTIFY = 20026,	// 被删除好友通知
		CMD_BE_BLACK_FRIEND_NOTIFY = 20027,		// 被拉黑好友通知
        CMD_GET_USER_PROFILE = 20028,			//关系链-查询用户信息
        CMD_SET_USER_PROFILE = 20029,			//关系链-设置用户信息
        CMD_SET_USER_PHOTO = 20030,				//关系链-设置头像
        CMD_SWITCH_USER_STATE = 20031			//关系链-切换用户在线状态
	};


	enum LoginErrorCode
	{
		EC_GET_USERINFO_ERR = 1,				// 获取用户信息失败，服务端检查用户所在 mysql 服务器是否正常
		EC_ADD_USERINFO_ERR = 2,				// 添加新用户失败，服务端检查用户所在 mysql 服务器是否正常
		EC_PSWD_ERR = 3,						// 用户密码不正确
		EC_INVLIAD_USER = 4,					// 用户状态为无效用户
		EC_UPDATE_USERINFO_ERR = 5,				// 更新用户信息失败，服务端检查用户所在 mysql 服务器是否正常
		EC_ENCRYPT_KEY_ERR = 6,					// 计算加密 key 失败，服务端异常
		EC_USER_NAME_TOO_LONG = 7,
		EC_INVALID_SERVICE_ID = 8,
		EC_AUTH_ERR = 9,
	};

	enum SendMessageErrorCode
	{
		EC_MSG_TOO_LONG = 1,				// 消息太长
		EC_GEN_MSGID_ERR = 2,				// 生成消息 ID 错误，服务器异常
		EC_RECVID_TOO_LONG = 3,				// 接收方 ID 过长，检查房间名是否过长
		EC_SAVE_MSG_ERR = 4,				// 保存消息错误，服务器检查消息所在的 redis 、mysql 是否正常，服务器致命错误
		EC_INVALID_CHAT_TYPE = 5,			// 无效的聊天类型，目前支持 1：私聊，2：房间
		EC_INVALID_RECV_ID = 6,				// 无效的用户ID，私聊时传入的接收方 ID 无法转换为非 0 的 64 位整数
		EC_EVENT_QUEUE_BUSY = 7,			// 服务端该进程的事件队列忙，检查事件 agent 和通知中心是否工作正常
		EC_GET_ROOMMEMBER_CNT = 8,			// 获取房间成员失败
		EC_SNDMSG_TO_EMPTY_ROOM = 9,		// 
		EC_GET_RECVER_LIST = 10,			// 解析receiver list 失败
		EC_RECVER_LIST_TOO_BIG = 11,		// receiver list太长
		EC_RECVID_EMPTY = 12,
		EC_USER_GAGED = 13,					// 被禁言
		EC_ILLEGAL_MESSAGE = 14,			// 含非法消息
		EC_ADVERTISEMENT_MESSAGE = 15,		// 涉嫌广告
		EC_MASK_MSG = 16					// 消息被屏蔽
	};

	enum ChatRoomErrorCode
	{
		EC_ROOM_NAME_TOO_LONG = 1,				// 房间名太长
		EC_SERVICE_EXCEPTION = 2,				// 服务器异常
		EC_ROOM_OVERFLOW = 3					// 房间人数超过上限
	};

	enum GetMSGErrorCode
	{
		EC_GET_MSGID_LIST_ERR = 1		// 获取用户收件箱列表失败，服务端检查该用户所在的 redis 服务是否正常
	};


	enum NearbyPeopleCode
	{
		EC_INVLIAD_PROTOCOL = 1,	// proto解析异常
		EC_NO_APP_USER = 2,			// 游戏下无用户记录
		EC_INVALID_QUERY_CONDITION = 3 	// 查询条件无效
	};

	// 查找好友
	enum FindFriendError
	{
		FINDFRIEND_DB_ERROR = 3,		// 获取mysql client错误
		FINDFRIEND_ERROR_ID = 4,		// 从DB中按ID查找好友错误
		FINDFRIEND_ERROR_NICKNAME = 5	// 从DB中按昵称查找好友错误
	};

	// 添加好友
	enum AddFriendError
	{
		ADDFRIEND_DB_ERROR = 3,					// 获取mysql client错误
		ADDFRIEND_QUERY_USERLIST_ERROR = 4,		// 从DB中查询用户列表错误
		ADDFRIEND_ALREADY_FRIEND = 5,			// 已经是好友
		ADDFRIEND_QUERY_BRIEF_INFO_ERROR = 6,	// 从DB中获取简略信息错误
		ADDFRIEND_GENERATE_ID_ERROR_1 = 7,		// 为请求主动方生成好友请求id错误
		ADDFRIEND_DB_ERROR_2 = 8,				// 插入请求主动方好友请求到DB错误
		ADDFRIEND_DB_ERROR_3 = 9,				// 插入请求被动方好友请求到DB错误
		ADDFRIEND_GENERATE_ID_ERROR_2 = 10,		// 为请求被动方生成好友请求id错误
		ADDFRIEND_ADD_TO_LIST_ERROR_1 = 11,		// 添加好友到请求主动方好友列表错误
		ADDFRIEND_ADD_TO_LIST_ERROR_2 = 12,		// 添加好友到请求被动方好友列表错误
		ADDFRIEND_NOT_FOUND_NOTIFY = 13			// 未找到通知服务
	};

	// 删除好友
	enum DeleteFriendError
	{
		DELETEFRIEND_DB_ERROR = 3,					// 获取mysql client错误
		DELETEFRIEND_QUERY_USERLIST_ERROR = 4,		// 从DB中查询用户列表错误
		DELETEFRIEND_NOT_FRIEND = 5,				// 当前还不是还有关系
		DELETEFRIEND_QUERY_BRIEF_INFO_ERROR = 6,	// 从DB中获取简略信息错误
		DELETEFRIEND_GENERATE_ID_ERROR_1 = 7,		// 为请求主动方生成好友请求id错误
		DELETEFRIEND_DB_ERROR_2 = 8,				// 插入请求主动方好友请求到DB错误
		DELETEFRIEND_DB_ERROR_3 = 9,				// 插入请求被动方好友请求到DB错误
		DELETEFRIEND_GENERATE_ID_ERROR_2 = 10,		// 为请求被动方生成好友请求id错误
		DELETEFRIEND_DELETE_ERROR_1 = 11,			// 从请求主动方好友列表删除好友错误
		DELETEFRIEND_DELETE_ERROR_2 = 12,			// 从请求被动方好友列表删除好友错误
		DELETEFRIEND_NOT_FOUND_NOTIFY = 13			// 未找到通知服务
	};

	// 拉黑好友
	enum BlackFriendError
	{
		BLACKFRIEND_DB_ERROR = 3,				// 获取mysql client错误
		BLACKFRIEND_QUERY_USERLIST_ERROR = 4,	// 从DB中查询用户列表错误
		BLACKFRIEND_NOT_FRIEND = 5,				// 当前还不是好友关系
		BLACKFRIEND_QUERY_BRIEF_INFO_ERROR = 6,	// 从DB中获取简略信息错误
		BLACKFRIEND_GENERATE_ID_ERROR_1 = 7,	// 为请求主动方生成好友请求id错误
		BLACKFRIEND_DB_ERROR_2 = 8,				// 插入请求主动方好友请求到DB错误
		BLACKFRIEND_DB_ERROR_3 = 9,				// 插入请求被动方好友请求到DB错误
		BLACKFRIEND_GENERATE_ID_ERROR_2 = 10,	// 为请求被动方生成好友请求id错误
		BLACKFRIEND_BLACK_ERROR = 11,			// 从请求主动方好友列表拉黑好友错误
		BLACKFRIEND_UNBLACK_ERROR = 12,			// 从请求主动方好友列表恢复好友错误
		BLACKFRIEND_NOT_FOUND_NOTIFY = 13,		// 未找到通知服务
		BLACKFRIEND_NOT_BLACK = 14				// 对方不在我的黑名单内
	};

	// 验证好友
	enum VerifyFriendError
	{
		VERIFYFRIEND_DB_ERROR = 3,					// 获取mysql client错误
		VERIFYFRIEND_QUERY_USERLIST_ERROR = 4,		// 从DB中查询用户列表错误
		VERIFYFRIEND_ALREADY_FRIEND = 5,			// 已经是好友
		VERIFYFRIEND_QUERY_BRIEF_INFO_ERROR = 6,	// 从DB中获取简略信息错误
		VERIFYFRIEND_GENERATE_ID_ERROR_1 = 7,		// 为请求主动方生成好友请求id错误
		VERIFYFRIEND_DB_ERROR_2 = 8,				// 更新请求主动方好友请求到DB错误
		VERIFYFRIEND_DB_ERROR_3 = 9,				// 更新请求被动方好友请求到DB错误
		VERIFYFRIEND_GENERATE_ID_ERROR_2 = 10,		// 为请求被动方生成好友请求id错误
		VERIFYFRIEND_ADD_LIST_ERROR_1 = 11,			// 添加好友到请求主动方好友列表错误
		VERIFYFRIEND_ADD_LIST_ERROR_2 = 12,			// 添加好友到请求被动方好友列表错误
		VERIFYFRIEND_NOT_FOUND_NOTIFY = 13,			// 未找到通知服务
	};

	// 查询好友列表
	enum QueryFriendListError
	{
		FRIENDLISTERROR_DB_ERROR_1 = 3,	// 获取mysql client错误
		FRIENDLISTERROR_DB_ERROR_2 = 4,	// 从DB获取好友列表错误
		FRIENDLISTERROR_DB_ERROR_3 = 5,	// 从DB获取黑名单列表错误
		FRIENDLISTERROR_DB_ERROR_4 = 6	// 从DB获取用户简略信息错误
	};

	// 获取好友请求列表
	enum FriendRequestListError
	{
		REQUESTLISTERROR_1 = 3,	// 获取mysql client错误
		REQUESTLISTERROR_2 = 4	// 从DB获取好友请求列表错误

	};

	// 关系链心跳
	enum RelationHeartbeatError
	{
		RELATIONHEARTERROR_1 = 3,	// 获取mysql client错误
		RELATIONHEARTERROR_2 = 4,	// 获取redis client错误
		RELATIONHEARTERROR_3 = 5	// 执行心跳错误
	};

    enum GetUserProfileInfoCode
    {
        EC_USER_HAS_NOT_REGISTER = 6  //未注册用户信息
    };
};

#endif
