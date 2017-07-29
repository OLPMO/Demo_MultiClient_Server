#ifndef _SERVER_H_
#define _SERVER_H_

#include <map>
#include <mutex>

#include <time.h>
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

#define SEND_THREAD_NUM (8) // ������Ϣ�̵߳�����

#define HANDLE_THREAD_NUM (5) // �����������߳�����


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


bool Start(unsigned short port); // ����

void Close(void); // �ر�

bool UserValidate(const char *name, const char *pwd, int &id); // ��֤�Ƿ�ɵ�½������ID


unsigned int _stdcall func_thread_accept(void *arg); // ���ӽ��ս���

unsigned int _stdcall func_thread_recv(void *arg);   // ���ݽ��ս���

unsigned int _stdcall func_thread_send(void *arg);   // ���ݷ��ͽ���

unsigned int _stdcall func_thread_handle(void *arg); // ���������ݴ������


// ��������


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


#endif