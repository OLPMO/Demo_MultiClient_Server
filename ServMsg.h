#ifndef _SERV_MSG_H_
#define _SERV_MSG_H_

#define PACK_TYPE_LOGIN (-1) // 包类型 - 登录信息

#define PACK_TAR_BOARDCAST (-1) // 包目标 - 广播
#define PACK_TAR_SERVER    (-2) // 包目标 - 服务器处理

#define PACK_FROM_SERVER (-1) // 包来源 - 服务器


// 数据包
typedef struct DataPacket
{
	char data[128];
} DATA_PACKET, *DATA_PACKET_PTR;


// 设置包类型
inline void SetPacketType(DataPacket &packet, int type)
{
	int *data = (int*)packet.data;
	data[0] = type;
}


// 设置包时间
inline void SetPacketTime(DataPacket &packet, long milli)
{
	long *time = (long*)&packet.data[4];
	*time = milli;
}


// 设置包ID
inline void SetPacketIdentify(DataPacket &packet, int identify)
{
	int *data = (int*)packet.data;
	data[3] = identify;
}


// 获取包类型
inline int GetPacketType(const DataPacket &packet)
{
	int *data = (int*)packet.data;
	return data[0];
}


// 获取包时间
inline long GetPacketTime(const DataPacket &packet)
{
	long *time = (long*)&packet.data[4];
	return (*time);
}



// 获取包ID
inline int GetPacketIdentify(const DataPacket &packet)
{
	int *data = (int*)packet.data;
	return data[3];
}


#endif