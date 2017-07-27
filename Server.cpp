#include <mutex>

#include <memory.h>
#include <process.h>

#include "Server.h"


// 全局变量

bool exitFlag;

SOCKET sockServ;

HANDLE hThreadAccept;

HANDLE hThreadSend[SEND_THREAD_NUM];

HANDLE hThreadHandle[HANDLE_THREAD_NUM];

std::map<int, CLIENT_PTR> mapClients;  // 图 - 客户端信息

ServQueue<DataPacket*, 512> queForward; // 转发队列 - 其中数据包需要转发

ServQueue<DataPacket*, 128> queHandle;  // 处理队列 - 其中数据包需要服务器处理

ServMemory<DataPacket> PacketPool; // 数据包内存池


// 函数实现

// 启动
bool Start(unsigned short port)
{
	if (sockServ) Close();
	exitFlag = false;
	memset(hThreadSend, 0, sizeof(hThreadSend));
	memset(hThreadHandle, 0, sizeof(hThreadHandle));

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sockServ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockServ == INVALID_SOCKET) return false;

	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (bind(sockServ, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		Close();
		return false;
	}

	if (listen(sockServ, 5) == SOCKET_ERROR)
	{
		Close();
		return false;
	}

	// 创建请求接收进程
	unsigned int id = 0;
	hThreadAccept = (HANDLE)_beginthreadex(NULL, 0, func_thread_accept, NULL, 0, &id);
	if (hThreadAccept == NULL)
	{
		Close();
		return false;
	}

	// 创建数据发送进程
	for (int i = 0; i < SEND_THREAD_NUM; i++)
	{
		unsigned int id = 0;
		hThreadSend[i] = (HANDLE)_beginthreadex(NULL, 0, func_thread_send, NULL, 0, &id);
		if (hThreadSend[i] == NULL)
		{
			Close();
			return false;
		}
	}

	// 创建服务器数据处理进程
	for (int i = 0; i < HANDLE_THREAD_NUM; i++)
	{
		unsigned int id = 0;
		hThreadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, func_thread_handle, NULL, 0, &id);
		if (hThreadHandle[i] == NULL)
		{
			Close();
			return false;
		}
	}

	return true;
}


// 关闭
void Close(void)
{
	if (sockServ == 0) return;

	exitFlag = true;

	closesocket(sockServ);
	sockServ = 0;

	queHandle.Clear();
	queForward.Clear();

	hThreadAccept = 0;
	memset(hThreadSend, 0, sizeof(hThreadSend));
	memset(hThreadHandle, 0, sizeof(hThreadHandle));
}


// 验证是否可登陆并返回ID
bool UserValidate(const char *name, const char *pwd, int &id)
{
	return true;
}


// 接收连接进程
unsigned int _stdcall func_thread_accept(void * parm)
{
	unsigned int id = 0;
	int lenOfAddr = sizeof(sockaddr_in);
	while (exitFlag == false)
	{
		sockaddr_in addr = { 0 };
		SOCKET sockClient = accept(sockServ, (sockaddr*)&addr, &lenOfAddr);
		if (sockClient == INVALID_SOCKET) continue;

		// 创建数据接收线程
		CLIENT_PTR clientConn = new Client();
		clientConn->sock = sockClient;
		clientConn->thread = (HANDLE)_beginthreadex(NULL, 0, func_thread_recv, (void*)clientConn, 0, &id);
		if (clientConn->thread == NULL)
		{
			// TODO - 回馈给客户端连接失败的消息 //////////

			closesocket(clientConn->sock);
			delete clientConn;
		}
	}

	_endthreadex(0);
	return 0;
}


// 接收数据进程
unsigned int _stdcall func_thread_recv(void * parm)
{
	if (parm == nullptr)
	{
		_endthreadex(0);
		return 0;
	}

	CLIENT_PTR clientConn = (CLIENT_PTR)parm;

	// 等待登录信息
	while (exitFlag == false)
	{
		DataPacket *packet = PacketPool.Alloc();
		if (packet == nullptr) continue;
		int lenOfData = recv(clientConn->sock, packet->data, sizeof(packet->data), 0);
		if (lenOfData < 0)
		{
			PacketPool.Release(packet);
			break;
		}
		else if (lenOfData > 0)
		{
			int type = GetPacketType(*packet);
			if (type == PACK_TYPE_LOGIN)
			{
				// TODO - 登录操作

				PacketPool.Release(packet);
				break;
			}
		}

		PacketPool.Release(packet);
		
	}
	
	// 接收数据包并分配
	while (exitFlag == false)
	{
		DataPacket *packet = PacketPool.Alloc();
		if (packet == nullptr) continue;

		int lenOfData = recv(clientConn->sock, packet->data, sizeof(packet->data), 0);
		if (lenOfData < 0)
		{
			PacketPool.Release(packet);
			break;
		}
		else if (lenOfData > 0)
		{
			int tar = GetPacketIdentify(*packet);
			if (tar == PACK_TAR_SERVER)
			{
				// TODO 插入到处理队列
			}
			else
			{
				// TODO 插入到转发队列
			}

		}

		PacketPool.Release(packet);

	}

	closesocket(clientConn->sock);
	delete clientConn;

	_endthreadex(0);
	return 0;
}


// 发送数据进程
unsigned int _stdcall func_thread_send(void * parm)
{
	while (exitFlag == false)
	{
		if (queForward.Empty()) continue;
		DataPacket *pack = queForward.Pop();
		int tar = GetPacketIdentify(*pack);
		if (tar >= 0)
		{
			// TODO 发送到指定客户端
		}
		else if (tar == PACK_TAR_BOARDCAST)
		{
			// TODO 广播发送
		}

		PacketPool.Release(pack);
	}

	_endthreadex(0);
	return 0;
}


// 服务器数据处理进程
unsigned int _stdcall func_thread_handle(void *arg)
{
	while (exitFlag == false)
	{
		if (queHandle.Empty()) continue;
		DataPacket *pack = queHandle.Pop();
		if (pack == nullptr) continue;

		int type = GetPacketType(*pack);

		// TODO 处理消息

	}

	_endthreadex(0);
	return 0;
}