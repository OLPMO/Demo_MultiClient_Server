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


// 宏


#define CLIENT_NUM_MAX (32) // 支持的最大玩家数

#define PACKET_SIZE_BYTE (64) // 数据包大小

#define SEND_THREAD_NUM (3) // 发送消息线程的数量

#define HANDLE_THREAD_NUM (2) // 服务器处理线程数量


// 客户端
typedef struct Client
{
	int id;
	SOCKET sock;
	HANDLE thread;
} CLIENT, *CLIENT_PTR;



// 全局变量


extern bool exitFlag;


extern SOCKET sockServ;

extern HANDLE hThreadAccept;

extern HANDLE hThreadSend[SEND_THREAD_NUM];

extern HANDLE hThreadHandle[HANDLE_THREAD_NUM];

extern HANDLE hSignalSend;   // 发送线程启动信号

extern HANDLE hSignalHandle; // 处理线程启动信号


extern ServQueue<DataPacket*, 512> queForward; // 转发队列 - 其中数据包需要转发

extern ServQueue<DataPacket*, 128> queHandle;  // 处理队列 - 其中数据包需要服务器处理

extern ServMemory<DataPacket> packetPool; // 数据包内存池


extern std::map<int, CLIENT_PTR> mapClients;  // 图 - 客户端信息

extern std::mutex mtxQueForward; // 转发队列互斥量

extern std::mutex mtxQueHandle;  // 处理队列互斥量

extern std::mutex mtxPacketPool; // 包内存池互斥量


// 函数声明


bool Start(unsigned short port, bool forceCoalesce = false); // 启动

void Close(void); // 关闭

bool UserValidate(const char *name, const char *pwd, int &id); // 验证是否可登陆并返回ID

bool WaitForLoginPacket(CLIENT_PTR pClientConn); // 等待登录信息


unsigned int _stdcall func_thread_accept(void *arg); // 连接接收进程

unsigned int _stdcall func_thread_recv(void *arg);   // 数据接收进程

unsigned int _stdcall func_thread_send(void *arg);   // 数据发送进程

unsigned int _stdcall func_thread_handle(void *arg); // 服务器数据处理进程

unsigned int _stdcall func_thread_heartsync(void *arg); // 心跳同步进程


// 内联函数


// 获取当前时间戳
inline unsigned long GetServTimeStamp(void)
{
	return (unsigned long)GetTickCount();
}


// 申请一个新的数据包
inline DataPacket* NewDataPacket(void)
{
	mtxPacketPool.lock();
	DataPacket *pack = packetPool.Alloc();
	mtxPacketPool.unlock();

	return pack;
}


// 释放一个数据包内存
inline void ReleaseDataPacket(DataPacket *pack)
{
	mtxPacketPool.lock();
	packetPool.Release(pack);
	mtxPacketPool.unlock();
}


// 从转发队列中取出一个数据包
inline DataPacket* PopForwardPacket(void)
{
	mtxQueForward.lock();
	DataPacket* pack = queForward.Pop();
	mtxQueForward.unlock();
	return pack;
}


// 将一个转发的数据包入队
inline void PushForwardPacket(DataPacket *pack)
{
	mtxQueForward.lock();
	queForward.Push(pack);
	mtxQueForward.unlock();

	SetEvent(hSignalSend);
}


// 从处理队列中取出一个数据包
inline DataPacket* PopHandlePacket(void)
{
	mtxQueHandle.lock();
	DataPacket *pack = queHandle.Pop();
	mtxQueHandle.unlock();

	return pack;
}


// 将一个处理数据包入队
inline void PushHandlePacket(DataPacket *pack)
{
	mtxQueHandle.lock();
	queHandle.Push(pack);
	mtxQueHandle.unlock();

	SetEvent(hSignalHandle);
}


// 获取客户端信息 - 客户端ID
inline CLIENT_PTR FindClientByID(int id)
{
	std::map<int, CLIENT_PTR>::iterator itor = mapClients.find(id);
	if (itor != mapClients.end()) return (itor->second);
	return nullptr;
}


// 发送一个数据包
inline void SendPacket(SOCKET sock, DataPacket *pack)
{
	send(sock, pack->data, (pack->bytes > 0 ? pack->bytes : sizeof(pack->data)), 0);
}


// 登录请求接受
// 添加当前用户到用户表并反馈登录成功信息
inline void LoginRequestAccept(CLIENT_PTR pClient)
{
	mapClients.insert(std::pair<int, CLIENT_PTR>(pClient->id, pClient));

	// 向登录成功的客户端发送登录成功反馈
	DataPacket *packAccept = NewDataPacket();
	packAccept->bytes = PACK_HEAD_BYTE;
	packAccept->from = pClient->id;
	SetPacketHeadInfo(*packAccept, PACK_TYPE_ACCEPT, GetServTimeStamp(), pClient->id);
	PushForwardPacket(packAccept);
}


// 登录请求拒绝
// 向客户端发送登录拒绝的反馈信息
inline void LoginRequestDeny(CLIENT_PTR pClient)
{
	// 向客户端发送服务器拒绝登录的反馈
	DataPacket *packDeny = NewDataPacket();
	packDeny->bytes = PACK_HEAD_BYTE;
	SetPacketHeadInfo(*packDeny, PACK_TYPE_DENY, GetServTimeStamp(), PACK_FROM_SERVER);
	SendPacket(pClient->sock, packDeny);
}


// 时间同步回馈消息
// 向客户端发送带有服务器时间的数据包
inline void TimeSyncRequestFeedback(int idFrom)
{
	DataPacket *packSync = NewDataPacket();
	packSync->bytes = PACK_HEAD_BYTE;
	packSync->from = PACK_FROM_SERVER;
	SetPacketHeadInfo(*packSync, PACK_TYPE_SYNC, GetServTimeStamp(), idFrom);
	PushForwardPacket(packSync);

	printf("Send a time sync : %ld\n", GetPacketTime(*packSync));
}


// 客户端下线广播
// 收到客户端下线信息后向所有客户端发送广播消息
inline void ClientOfflineBoardcast(int clientID)
{
	DataPacket *packOffline = NewDataPacket();
	packOffline->bytes = PACK_HEAD_BYTE;
	packOffline->from = clientID;
	SetPacketHeadInfo(*packOffline, PACK_TYPE_OFFLINE, GetServTimeStamp(), PACK_TAR_BOARDCAST);
	PushForwardPacket(packOffline);
}


#endif