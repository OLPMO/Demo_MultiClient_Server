#ifndef _SERVER_H_
#define _SERVER_H_

#include <map>
#include <mutex>

#include <memory.h>
#include <process.h>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

#include "ServMsg.h"
#include "ServQueue.h"
#include "ServMemory.h"

#pragma comment(lib, "ws2_32.lib")


// ��


#define CLIENT_NUM_MAX (32) // ֧�ֵ���������

#define PACKET_SIZE_BYTE (64) // ���ݰ���С

#define SEND_THREAD_NUM (3) // ������Ϣ�̵߳�����

#define HANDLE_THREAD_NUM (2) // �����������߳�����


// �ͻ���
typedef struct Client
{
	int id;
	SOCKET sock;
	HANDLE thread;
} CLIENT, *CLIENT_PTR;



// ȫ�ֱ���


extern bool exitFlag;


extern SOCKET sockServ;

extern HANDLE hThreadAccept;

extern HANDLE hThreadSend[SEND_THREAD_NUM];

extern HANDLE hThreadHandle[HANDLE_THREAD_NUM];

extern HANDLE hSignalSend;   // �����߳������ź�

extern HANDLE hSignalHandle; // �����߳������ź�


extern ServQueue<DataPacket*, 512> queForward; // ת������ - �������ݰ���Ҫת��

extern ServQueue<DataPacket*, 128> queHandle;  // ������� - �������ݰ���Ҫ����������

extern ServMemory<DataPacket> packetPool; // ���ݰ��ڴ��


extern std::mutex mtxQueForward; // ת�����л�����

extern std::mutex mtxQueHandle;  // ������л�����

extern std::mutex mtxPacketPool; // ���ڴ�ػ�����


extern CLIENT_PTR listClients[2]; // �ͻ�����Ϣ


// ��������


bool Start(unsigned short port, bool forceCoalesce = false); // ����

void Close(void); // �ر�

bool UserValidate(const char *name, const char *pwd, int &id); // ��֤�Ƿ�ɵ�½������ID

bool WaitForLoginPacket(CLIENT_PTR pClientConn); // �ȴ���¼��Ϣ


unsigned int _stdcall func_thread_accept(void *arg); // ���ӽ��ս���

unsigned int _stdcall func_thread_recv(void *arg);   // ���ݽ��ս���

unsigned int _stdcall func_thread_send(void *arg);   // ���ݷ��ͽ���

unsigned int _stdcall func_thread_handle(void *arg); // ���������ݴ������

unsigned int _stdcall func_thread_heartsync(void *arg); // ����ͬ������


// ��������


// ��ȡ��ǰʱ���
// Return : ��ǰ������ʱ���,����
inline unsigned long long GetServTimeStamp(void)
{
	return (unsigned long long)GetTickCount();
}


// ����һ���µ����ݰ�
// Return : ���뵽�������ݰ�ָ��
inline DataPacket* NewDataPacket(void)
{
	mtxPacketPool.lock();
	DataPacket *pack = packetPool.Alloc();
	mtxPacketPool.unlock();

	return pack;
}


// �ͷ�һ�����ݰ��ڴ�
// Parm : �����յ����ݰ�ָ��
inline void ReleaseDataPacket(DataPacket *pack)
{
	mtxPacketPool.lock();
	packetPool.Release(pack);
	mtxPacketPool.unlock();
}


// ��ת��������ȡ��һ�����ݰ�
// Return : ��ת��������ȡ�������ݰ�
inline DataPacket* PopForwardPacket(void)
{
	mtxQueForward.lock();
	DataPacket* pack = queForward.Pop();
	mtxQueForward.unlock();
	return pack;
}


// ��һ��ת�������ݰ����
// Parm : pack �����뵽ת�����е����ݰ�
inline void PushForwardPacket(DataPacket *pack)
{
	mtxQueForward.lock();
	queForward.Push(pack);
	mtxQueForward.unlock();

	SetEvent(hSignalSend);
}


// �Ӵ��������ȡ��һ�����ݰ�
// Return : �Ӷ���ȡ�������ݰ�
inline DataPacket* PopHandlePacket(void)
{
	mtxQueHandle.lock();
	DataPacket *pack = queHandle.Pop();
	mtxQueHandle.unlock();

	return pack;
}


// ��һ���������ݰ����
// Parm : pack �����뵽������е����ݰ�
inline void PushHandlePacket(DataPacket *pack)
{
	mtxQueHandle.lock();
	queHandle.Push(pack);
	mtxQueHandle.unlock();

	SetEvent(hSignalHandle);
}


// ��ȡ�ͻ�����Ϣ
// Parm : id �ͻ��˱�ʶ
inline CLIENT_PTR FindClientByID(int id)
{
	if((unsigned)id <= 1)
		return listClients[id];

	return nullptr;
}


// ����һ�����ݰ�
// Parm : sock �ͻ����׽���
// Parm : pack �����͵����ݰ�
inline void SendPacket(SOCKET sock, DataPacket *pack)
{
	send(sock, pack->data, (pack->bytes > 0 ? pack->bytes : sizeof(pack->data)), 0);
}


// ��¼�������,��ӵ�ǰ�û����û���������¼�ɹ���Ϣ
// Parm : pClient ��¼�ɹ����û���Ϣ
inline void LoginRequestAccept(CLIENT_PTR pClient)
{
	listClients[pClient->id] = pClient;

	// ���¼�ɹ��Ŀͻ��˷��͵�¼�ɹ�����
	DataPacket *packAccept = NewDataPacket();
	packAccept->bytes = PACK_HEAD_BYTE;
	packAccept->from = pClient->id;
	SetPacketHeadInfo(*packAccept, PACK_TYPE_ACCEPT, GetServTimeStamp(), pClient->id);
	PushForwardPacket(packAccept);

	// �ж��Ƿ񶼵�¼�ɹ�
	// �����ͻ��˵�½�ɹ����Ϳ�ʼ��Ϸ�����ݰ�
	if(listClients[0] && listClients[1])
	{
		DataPacket *packGameStart = NewDataPacket();
		packGameStart->bytes = PACK_HEAD_BYTE;
		packGameStart->from = PACK_FROM_SERVER;
		SetPacketHeadInfo(*packGameStart, PACK_TYPE_GAMEON, GetServTimeStamp() + 1000, PACK_TAR_BOARDCAST);
		PushForwardPacket(packGameStart);
	}
}


// ��¼����ܾ�
// ��ͻ��˷��͵�¼�ܾ��ķ�����Ϣ
// Parm : pClient �ܾ���¼���û���Ϣ
inline void LoginRequestDeny(CLIENT_PTR pClient)
{
	// ��ͻ��˷��ͷ������ܾ���¼�ķ���
	DataPacket *packDeny = NewDataPacket();
	packDeny->bytes = PACK_HEAD_BYTE;
	SetPacketHeadInfo(*packDeny, PACK_TYPE_DENY, GetServTimeStamp(), PACK_FROM_SERVER);
	SendPacket(pClient->sock, packDeny);
}


// ʱ��ͬ��������Ϣ
// ��ͻ��˷��ʹ��з�����ʱ������ݰ�
// Parm : idFrom ��Դ�ͻ��˱�ʶ
inline void TimeSyncRequestFeedback(int idFrom)
{
	DataPacket *packSync = NewDataPacket();
	packSync->bytes = PACK_HEAD_BYTE;
	packSync->from = PACK_FROM_SERVER;
	SetPacketHeadInfo(*packSync, PACK_TYPE_SET_TIMESTAMP, GetServTimeStamp(), idFrom);
	PushForwardPacket(packSync);
}


// ʱ�����¼����������
// ��ͻ��˷��ͼ�������ʱ��
// Parm : idFrom ����ԴID
// Parm : timeStampInPack ��Դ��Я����ʱ���
inline void TimeSyncRecalcFeedback(int idFrom, unsigned long long timeStampInPack)
{
	unsigned long long recalcTimeStamp = (GetServTimeStamp() + timeStampInPack) / 2;

	DataPacket *packFeedback = NewDataPacket();
	packFeedback->bytes = PACK_HEAD_BYTE;
	packFeedback->from = PACK_FROM_SERVER;
	SetPacketHeadInfo(*packFeedback, PACK_TYPE_RECALC_RESULT, recalcTimeStamp, idFrom);
	PushForwardPacket(packFeedback);
}


// �ͻ������߹㲥
// �յ��ͻ���������Ϣ�������пͻ��˷��͹㲥��Ϣ
inline void ClientOfflineBoardcast(int clientID)
{
	DataPacket *packOffline = NewDataPacket();
	packOffline->bytes = PACK_HEAD_BYTE;
	packOffline->from = clientID;
	SetPacketHeadInfo(*packOffline, PACK_TYPE_OFFLINE, GetServTimeStamp(), PACK_TAR_BOARDCAST);
	PushForwardPacket(packOffline);
}


#endif