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


extern std::map<int, CLIENT_PTR> mapClients;  // ͼ - �ͻ�����Ϣ

extern std::mutex mtxQueForward; // ת�����л�����

extern std::mutex mtxQueHandle;  // ������л�����

extern std::mutex mtxPacketPool; // ���ڴ�ػ�����


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
inline unsigned long GetServTimeStamp(void)
{
	return (unsigned long)GetTickCount();
}


// ����һ���µ����ݰ�
inline DataPacket* NewDataPacket(void)
{
	mtxPacketPool.lock();
	DataPacket *pack = packetPool.Alloc();
	mtxPacketPool.unlock();

	return pack;
}


// �ͷ�һ�����ݰ��ڴ�
inline void ReleaseDataPacket(DataPacket *pack)
{
	mtxPacketPool.lock();
	packetPool.Release(pack);
	mtxPacketPool.unlock();
}


// ��ת��������ȡ��һ�����ݰ�
inline DataPacket* PopForwardPacket(void)
{
	mtxQueForward.lock();
	DataPacket* pack = queForward.Pop();
	mtxQueForward.unlock();
	return pack;
}


// ��һ��ת�������ݰ����
inline void PushForwardPacket(DataPacket *pack)
{
	mtxQueForward.lock();
	queForward.Push(pack);
	mtxQueForward.unlock();

	SetEvent(hSignalSend);
}


// �Ӵ��������ȡ��һ�����ݰ�
inline DataPacket* PopHandlePacket(void)
{
	mtxQueHandle.lock();
	DataPacket *pack = queHandle.Pop();
	mtxQueHandle.unlock();

	return pack;
}


// ��һ���������ݰ����
inline void PushHandlePacket(DataPacket *pack)
{
	mtxQueHandle.lock();
	queHandle.Push(pack);
	mtxQueHandle.unlock();

	SetEvent(hSignalHandle);
}


// ��ȡ�ͻ�����Ϣ - �ͻ���ID
inline CLIENT_PTR FindClientByID(int id)
{
	std::map<int, CLIENT_PTR>::iterator itor = mapClients.find(id);
	if (itor != mapClients.end()) return (itor->second);
	return nullptr;
}


// ����һ�����ݰ�
inline void SendPacket(SOCKET sock, DataPacket *pack)
{
	send(sock, pack->data, (pack->bytes > 0 ? pack->bytes : sizeof(pack->data)), 0);
}


// ��¼�������
// ��ӵ�ǰ�û����û���������¼�ɹ���Ϣ
inline void LoginRequestAccept(CLIENT_PTR pClient)
{
	mapClients.insert(std::pair<int, CLIENT_PTR>(pClient->id, pClient));

	// ���¼�ɹ��Ŀͻ��˷��͵�¼�ɹ�����
	DataPacket *packAccept = NewDataPacket();
	packAccept->bytes = PACK_HEAD_BYTE;
	packAccept->from = pClient->id;
	SetPacketHeadInfo(*packAccept, PACK_TYPE_ACCEPT, GetServTimeStamp(), pClient->id);
	PushForwardPacket(packAccept);
}


// ��¼����ܾ�
// ��ͻ��˷��͵�¼�ܾ��ķ�����Ϣ
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
inline void TimeSyncRequestFeedback(int idFrom)
{
	DataPacket *packSync = NewDataPacket();
	packSync->bytes = PACK_HEAD_BYTE;
	packSync->from = PACK_FROM_SERVER;
	SetPacketHeadInfo(*packSync, PACK_TYPE_SYNC, GetServTimeStamp(), idFrom);
	PushForwardPacket(packSync);

	printf("Send a time sync : %ld\n", GetPacketTime(*packSync));
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