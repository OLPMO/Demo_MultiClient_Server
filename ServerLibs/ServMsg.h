#ifndef _SERV_MSG_H_
#define _SERV_MSG_H_


#include <memory.h>


#define PACK_DATA_BYTE (80) // ���ݰ��ֽ���

#define PACK_TYPE_LOGIN (-1) // ������ - ��¼��Ϣ
#define PACK_TYPE_ERROR (-2) // ������ - ������Ϣ
#define PACK_TYPE_DENY  (-3) // ������ - �ܾ���¼

#define PACK_TAR_BOARDCAST (-1) // ��Ŀ�� - �㲥
#define PACK_TAR_SERVER    (-2) // ��Ŀ�� - ����������

#define PACK_FROM_SERVER (-1) // ����Դ - ������


// ���ݰ�
typedef struct DataPacket
{
	int from;
	char data[PACK_DATA_BYTE];
} DATA_PACKET, *DATA_PACKET_PTR;


// ���ð�����
inline void SetPacketType(DataPacket &packet, int type)
{
	int *data = (int*)packet.data;
	data[0] = type;
}


// ���ð�ʱ��
inline void SetPacketTime(DataPacket &packet, long milli)
{
	long *time = (long*)&packet.data[4];
	*time = milli;
}


// ���ð�ID
inline void SetPacketIdentify(DataPacket &packet, int identify)
{
	int *data = (int*)packet.data;
	data[3] = identify;
}


// ��ȡ������
inline int GetPacketType(const DataPacket &packet)
{
	int *data = (int*)packet.data;
	return data[0];
}


// ��ȡ��ʱ��
inline long GetPacketTime(const DataPacket &packet)
{
	long *time = (long*)&packet.data[4];
	return (*time);
}


// ��ȡ��ID
inline int GetPacketIdentify(const DataPacket &packet)
{
	int *data = (int*)packet.data;
	return data[3];
}


// ��ȡ���û���������
inline void ParsePacketLogin(const DataPacket &packet, char *name, char *pwd)
{
	memcpy(name, &packet.data[16], 32);
	memcpy(pwd, &packet.data[48], 32);
}


#endif