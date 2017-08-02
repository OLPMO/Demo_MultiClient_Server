#ifndef _SERV_MSG_H_
#define _SERV_MSG_H_


#include <memory.h>


#define PACK_DATA_BYTE (80) // ���ݰ��ֽ���

#define PACK_TYPE_LOGIN   (-1) // ������ - ��¼��Ϣ
#define PACK_TYPE_ERROR   (-2) // ������ - ������Ϣ
#define PACK_TYPE_DENY    (-3) // ������ - �ܾ���¼
#define PACK_TYPE_ACCEPT  (-4) // ������ - ��¼�ɹ�
#define PACK_TYPE_OFFLINE (-5) // ������ - �������
#define PACK_TYPE_SYNC    (-6) // ������ - ͬ������

#define PACK_TAR_BOARDCAST (-1) // ��Ŀ�� - �㲥
#define PACK_TAR_SERVER    (-2) // ��Ŀ�� - ����������
#define PACK_TAR_FROM      (-3) // ��Ŀ�� - ת������Դ

#define PACK_FROM_SERVER (-1) // ����Դ - ������


// ���ݰ�
typedef struct DataPacket
{
	int from;
	int bytes; // ��Ч���ݳ���
	char data[PACK_DATA_BYTE];
} DATA_PACKET, *DATA_PACKET_PTR;


// ���ð�����
inline void SetPacketType(DataPacket &packet, int type)
{
	((int*)packet.data)[0] = type;
}


// ���ð�ʱ��
inline void SetPacketTime(DataPacket &packet, long milli)
{
	*((long*)&packet.data[4]) = milli;
}


// ���ð�ID
inline void SetPacketIdentify(DataPacket &packet, int identify)
{
	((int*)packet.data)[3] = identify;
}


// ���ð�ͷ����
inline void SetPacketHeadInfo(DataPacket &packet, int type, long milli, int identify)
{
	SetPacketType(packet, type);
	SetPacketTime(packet, milli);
	SetPacketIdentify(packet, identify);
}


// ��ȡ������
inline int GetPacketType(const DataPacket &packet)
{
	return ((int*)packet.data)[0];
}


// ��ȡ��ʱ��
inline long GetPacketTime(const DataPacket &packet)
{
	return ((long*)&packet.data[4])[0];
}


// ��ȡ��ID
inline int GetPacketIdentify(const DataPacket &packet)
{
	return ((int*)packet.data)[3];
}


// ��ȡ���û���������
inline void ParsePacketLogin(const DataPacket &packet, char *name, char *pwd)
{
	memcpy(name, &packet.data[16], 32);
	memcpy(pwd, &packet.data[48], 32);
}


#endif