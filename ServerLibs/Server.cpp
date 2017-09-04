#include "Server.h"
#include "ServDB.h"

// 全局变量

bool exitFlag = false;


SOCKET sockServ = INVALID_SOCKET;

HANDLE hThreadAccept = 0;

HANDLE hThreadSend[SEND_THREAD_NUM] = {0};

HANDLE hThreadHandle[HANDLE_THREAD_NUM] = {0};

HANDLE hSignalSend = 0;   // 发送线程启动信号

HANDLE hSignalHandle = 0; // 处理线程启动信号


ServQueue<DataPacket*, 512> queForward; // 转发队列 - 其中数据包需要转发

ServQueue<DataPacket*, 128> queHandle;  // 处理队列 - 其中数据包需要服务器处理

ServMemory<DataPacket> packetPool; // 数据包内存池


std::mutex mtxQueForward; // 转发队列互斥量

std::mutex mtxQueHandle;  // 处理队列互斥量

std::mutex mtxPacketPool; // 包内存池互斥量


CLIENT_PTR listClients[2] = { 0 }; // 客户端信息


// 函数实现

// 启动服务器
// Parm : port 端口号
// Parm : forceCoalesce 是否开启分组算法 - 不开启可以加速游戏中发包响应速度
// Return : 是否启动成功
bool Start(unsigned short port, bool forceCoalesce /* = false */)
{
	if (sockServ != INVALID_SOCKET) Close();
	exitFlag = false;
	memset(hThreadSend, 0, sizeof(hThreadSend));
	memset(hThreadHandle, 0, sizeof(hThreadHandle));

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sockServ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockServ == INVALID_SOCKET) return false;

	// 禁用数据包分组算法
	// 会以牺牲网络带宽为代价来加快数据包发送速度
	// 在需要发送大量小数据包时使用
	if (forceCoalesce == false)
	{
		int optval = 1;
		setsockopt(sockServ, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	}

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
	hSignalSend = CreateEvent(NULL, true, true, TEXT("signal_send"));
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
	hSignalHandle = CreateEvent(NULL, true, true, TEXT("signal_send"));
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
	if (sockServ == INVALID_SOCKET) return;

	exitFlag = true;

	closesocket(sockServ);
	sockServ = INVALID_SOCKET;

	queHandle.Clear();
	queForward.Clear();

	for (int i = 0; i < SEND_THREAD_NUM; ++i) SetEvent(hSignalSend);
	for (int i = 0; i < HANDLE_THREAD_NUM; ++i) SetEvent(hSignalHandle);

	hThreadAccept = 0;
	memset(hThreadSend, 0, sizeof(hThreadSend));
	memset(hThreadHandle, 0, sizeof(hThreadHandle));
}


// 验证是否可登陆并返回ID
// Parm : name 用户名
// Parm : pwd  待验证的密码
// Parm : id   返回分配给用户的标识
// Return : 验证是否成功
bool UserValidate(const char *name, const char *pwd, int &id)
{
	return DB_Validate_User(name, pwd, id);
}


// 接收连接进程
unsigned int _stdcall func_thread_accept(void * parm)
{
	Client clientConn;
	unsigned int id = 0;
	int lenOfAddr = sizeof(sockaddr_in);
	while (exitFlag == false)
	{
		sockaddr_in addr = { 0 };
		SOCKET sockClient = accept(sockServ, (sockaddr*)&addr, &lenOfAddr);
		if (sockClient == INVALID_SOCKET) continue;

		char ipBuffer[64] = { 0 };
		inet_ntop(AF_INET, &addr.sin_addr, ipBuffer, sizeof(ipBuffer));

		// 创建数据接收线程
		clientConn.sock = sockClient;
		clientConn.thread = (HANDLE)_beginthreadex(NULL, 0, func_thread_recv, (void*)&clientConn, 0, &id);
		if (clientConn.thread == NULL)
		{
			// 反馈服务器错误
			DataPacket *pack = NewDataPacket();
			SetPacketHeadInfo(*pack, PACK_TYPE_ERROR, GetServTimeStamp(), PACK_FROM_SERVER);
			SendPacket(clientConn.sock, pack);
			ReleaseDataPacket(pack);

			closesocket(clientConn.sock);
		}

	}

	_endthreadex(0);
	return 0;
}


// 等待登录数据包并验证
// 将调用该函数的线程阻塞直到验证登录成功或客户端断开连接
// Parm   : pClientConn 客户端信息
// Return : 是否登录成功
bool WaitForLoginPacket(CLIENT_PTR pClientConn)
{
	bool loginSucceed = false;
	DataPacket *packetLogin = NewDataPacket();
	while (exitFlag == false && pClientConn->sock != INVALID_SOCKET)
	{
		int lenOfData = recv(pClientConn->sock, packetLogin->data, sizeof(packetLogin->data), 0);
		if (lenOfData <= 0) break;

		int type = GetPacketType(*packetLogin);
		if (type == PACK_TYPE_LOGIN)
		{
			// 接收到登录信息并验证
			char name[32] = { 0 }, pwd[32] = { 0 };
			ParsePacketLogin(*packetLogin, name, pwd);
			if (UserValidate(name, pwd, pClientConn->id) && FindClientByID(pClientConn->id) == nullptr)
			{
				loginSucceed = true;
				LoginRequestAccept(pClientConn); // 接受登录
				printf("User Login ACCEPTED : %s\n", name); // DEBUG
			}
			else
			{
				LoginRequestDeny(pClientConn); // 拒绝登录
				printf("User Login DENIED : %s\n", name); // DEBUG
			}

			break;
		}
		else if (type == PACK_TYPE_OFFLINE)
		{
			// 客户端下线 - 结束登录等待
			break;
		}

	} // End while exitFlag

	ReleaseDataPacket(packetLogin);

	return loginSucceed;
}


// 接收数据进程
// Parm : parm 指向线程服务用户的Client结构
// Return : 结束码
unsigned int _stdcall func_thread_recv(void * parm)
{
	if (parm == nullptr)
	{
		_endthreadex(0);
		return 0;
	}

	Client clientConn;
	clientConn.id = -1;
	clientConn.sock = static_cast<CLIENT_PTR>(parm)->sock;
	clientConn.thread = static_cast<CLIENT_PTR>(parm)->thread;

	printf("Connection request\n"); // DEBUG

	// 阻塞等待登录验证
	bool loginSucceed = WaitForLoginPacket(&clientConn);

	// 登录成功 - 循环接收数据包并处理
	// 如果登录失败 - 则不执行if内语句
	if (loginSucceed)
	{
		// 接收数据包并分配
		DataPacket *packRecv = NewDataPacket();
		while (exitFlag == false && clientConn.sock != INVALID_SOCKET)
		{
			packRecv->bytes = recv(clientConn.sock, packRecv->data, PACK_TOTL_BYTE, 0);
			if (packRecv->bytes <= 0) break;
			packRecv->from = clientConn.id;

			int tar = GetPacketIdentify(*packRecv);
			if (tar == PACK_TAR_SERVER)
			{
				// 是否为下线信息 - 是则发送下线广播
				if (GetPacketType(*packRecv) == PACK_TYPE_OFFLINE)
				{
					ClientOfflineBoardcast(clientConn.id);
					break;
				}

				PushHandlePacket(packRecv);
				packRecv = NewDataPacket();
			}
			else
			{
				PushForwardPacket(packRecv);
				packRecv = NewDataPacket();
			}

		} // End while - exitFlag

		ReleaseDataPacket(packRecv);

	} // End if - Login Succ

	// 从客户端记录表中删除当前客户端记录并清除登录信息
	clearUserCache(clientConn.id);
	closesocket(clientConn.sock);
	clientConn.sock = INVALID_SOCKET;

	printf("The client id is %d\n", clientConn.id);

	listClients[clientConn.id] = nullptr;

	_endthreadex(0);
	return 0;
}


// 发送数据进程
// Parm : parm 未使用的
// Return : 返回码
unsigned int _stdcall func_thread_send(void * parm)
{
	while (exitFlag == false)
	{
		// 阻塞并等待信号唤醒
		WaitForSingleObject(hSignalSend, INFINITE);

		// 从队列中取出一个数据包
		DataPacket *packForward = PopForwardPacket();
		if (packForward == nullptr)
		{
			ResetEvent(hSignalSend);
			continue;
		}

		// 对数据进行转发
		int tar = GetPacketIdentify(*packForward);
		if (tar == PACK_TAR_BOARDCAST)
		{
			// 广播 - 转发给除来源之外的所有客户端 
			SetPacketIdentify(*packForward, packForward->from);

			if(packForward->from != 0 && listClients[0])
				SendPacket(listClients[0]->sock, packForward);

			if(packForward->from != 1 && listClients[1])
				SendPacket(listClients[1]->sock, packForward);
		}
		else if(tar == PACK_TAR_FROM)
		{
			// 测试 - 转发至来源
			CLIENT_PTR pClient = FindClientByID(packForward->from);
			if(pClient)
			{
				SetPacketIdentify(*packForward, packForward->from);
				SendPacket(pClient->sock, packForward);
			}
		}
		else
		{
			// 定点 - 转发给指定客户端
			CLIENT_PTR pClient = FindClientByID(tar);
			if(pClient)
			{
				SetPacketIdentify(*packForward, packForward->from);
				SendPacket(pClient->sock, packForward);
			}
		}

		ReleaseDataPacket(packForward);
	}

	_endthreadex(0);
	return 0;
}


// 服务器数据处理进程
// Parm : arg 未使用的
// Return : 结束码
unsigned int _stdcall func_thread_handle(void *arg)
{
	while (exitFlag == false)
	{
		// 阻塞并等待信号唤醒
		WaitForSingleObject(hSignalHandle, INFINITE);

		// 从队列中取出一个待处理的数据包
		DataPacket *packHandling = PopHandlePacket();
		if (packHandling == nullptr)
		{
			ResetEvent(hSignalHandle);
			continue;
		}

		// 根据数据包种类对数据包进行处理
		int type = GetPacketType(*packHandling);
		switch (type)
		{
		case PACK_TYPE_SYNC: // 时间同步请求
			TimeSyncRequestFeedback(packHandling->from);
			break;

		case PACK_TYPE_RECALC_TIMESTAMP: // 重新时间计算请求
			TimeSyncRecalcFeedback(packHandling->from, GetPacketTime(*packHandling));
			break;

		default:
			break;
		}

		ReleaseDataPacket(packHandling);

	}

	_endthreadex(0);
	return 0;
}