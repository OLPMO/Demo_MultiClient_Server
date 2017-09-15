#ifndef _SERV_MSG_H_
#define _SERV_MSG_H_


#include <memory.h>

#define PACK_HEAD_BYTE (16) // 包头字节数
#define PACK_DATA_BYTE (128) // 数据包有效数据段字节数
#define PACK_TOTL_BYTE (PACK_HEAD_BYTE + PACK_DATA_BYTE) // 数据包总字节数


#define PACK_TYPE_LOGIN     (-1) // 包类型 - 登录信息
#define PACK_TYPE_ERROR     (-2) // 包类型 - 错误信息
#define PACK_TYPE_DENY      (-3) // 包类型 - 拒绝登录
#define PACK_TYPE_ACCEPT    (-4) // 包类型 - 登录成功
#define PACK_TYPE_OFFLINE   (-5) // 包类型 - 玩家下线
#define PACK_TYPE_SYNC      (-6) // 包类型 - 同步请求

#define PACK_TYPE_SET_TIMESTAMP    (-7) // 包类型 - 设置客户端时间戳
#define PACK_TYPE_RECALC_TIMESTAMP (-8) // 包类型 - 重新计算时间戳请求
#define PACK_TYPE_RECALC_RESULT    (-9) // 包类型 - 时间戳重新计算结果

#define PACK_TYPE_GAMEON    (-10) // 包类型 - 游戏开始


#define PACK_TAR_BOARDCAST (-1) // 包目标 - 广播
#define PACK_TAR_SERVER    (-2) // 包目标 - 服务器处理
#define PACK_TAR_FROM      (-3) // 包目标 - 转发到来源


#define PACK_FROM_SERVER (-1) // 包来源 - 服务器


// 数据包
typedef struct DataPacket
{
	int from;
	int bytes; // 有效数据长度
	char data[PACK_TOTL_BYTE];
} DATA_PACKET, *DATA_PACKET_PTR;


// 设置包类型
inline void SetPacketType(DataPacket &packet, int type)
{
	((int*)packet.data)[0] = type;
}


// 设置包时间
inline void SetPacketTime(DataPacket &packet, unsigned long long milli)
{
	*((unsigned long long*)&packet.data[4]) = milli;
}


// 设置包ID
inline void SetPacketIdentify(DataPacket &packet, int identify)
{
	((int*)packet.data)[3] = identify;
}


// 设置包头内容
inline void SetPacketHeadInfo(DataPacket &packet, int type, unsigned long long milli, int identify)
{
	SetPacketType(packet, type);
	SetPacketTime(packet, milli);
	SetPacketIdentify(packet, identify);
}


// 获取包类型
inline int GetPacketType(const DataPacket &packet)
{
	return ((int*)packet.data)[0];
}


// 获取包时间
inline unsigned long long GetPacketTime(const DataPacket &packet)
{
	return ((unsigned long long*)&packet.data[4])[0];
}


// 获取包ID
inline int GetPacketIdentify(const DataPacket &packet)
{
	return ((int*)packet.data)[3];
}


// 获取包用户名和密码
inline void ParsePacketLogin(const DataPacket &packet, char *name, char *pwd)
{
	memcpy(name, &packet.data[16], 32);
	memcpy(pwd, &packet.data[48], 32);
}


#endif