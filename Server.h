#ifndef _SERVER_H_
#define _SERVER_H_

#include <map>

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

#define SEND_THREAD_NUM (5) // ������Ϣ�̵߳�����

#define HANDLE_THREAD_NUM (3) // �����������߳�����


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

extern std::map<int, CLIENT_PTR> mapClients;  // ͼ - �ͻ�����Ϣ

extern ServQueue<DataPacket*, 512> queForward; // ת������ - �������ݰ���Ҫת��

extern ServQueue<DataPacket*, 128> queHandle;  // ������� - �������ݰ���Ҫ����������

extern ServMemory<DataPacket> PacketPool; // ���ݰ��ڴ��



// ��������


bool Start(unsigned short port); // ����

void Close(void); // �ر�

bool UserValidate(const char *name, const char *pwd, int &id); // ��֤�Ƿ�ɵ�½������ID


unsigned int _stdcall func_thread_accept(void *arg); // ���ӽ��ս���

unsigned int _stdcall func_thread_recv(void *arg);   // ���ݽ��ս���

unsigned int _stdcall func_thread_send(void *arg);   // ���ݷ��ͽ���

unsigned int _stdcall func_thread_handle(void *arg); // ���������ݴ������


#endif