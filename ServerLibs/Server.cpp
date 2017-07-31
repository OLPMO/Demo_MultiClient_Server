#include "Server.h"


// 全局变量

bool exitFlag;

SOCKET sockServ;

HANDLE hThreadAccept;

HANDLE hThreadSend[SEND_THREAD_NUM];

HANDLE hThreadHandle[HANDLE_THREAD_NUM];

HANDLE hSignalSend;   // 发送线程启动信号

HANDLE hSignalHandle; // 处理线程启动信号


ServQueue<DataPacket*, 512> queForward; // 转发队列 - 其中数据包需要转发

ServQueue<DataPacket*, 128> queHandle;  // 处理队列 - 其中数据包需要服务器处理

ServMemory<DataPacket> packetPool; // 数据包内存池


std::map<int, CLIENT_PTR> mapClients;  // 图 - 客户端信息

std::mutex mtxQueForward; // 转发队列互斥量

std::mutex mtxQueHandle;  // 处理队列互斥量

std::mutex mtxPacketPool; // 包内存池互斥量


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
	if (sockServ == 0) return;

	exitFlag = true;

	closesocket(sockServ);
	sockServ = 0;

	queHandle.Clear();
	queForward.Clear();

	for (int i = 0; i < SEND_THREAD_NUM; i++) SetEvent(hSignalSend);
	for (int i = 0; i < HANDLE_THREAD_NUM; i++) SetEvent(hSignalHandle);

	hThreadAccept = 0;
	memset(hThreadSend, 0, sizeof(hThreadSend));
	memset(hThreadHandle, 0, sizeof(hThreadHandle));
}


// 验证是否可登陆并返回ID
bool UserValidate(const char *name, const char *pwd, int &id)
{
	// TODO 重写这个函数 - 当前函数只为测试

	if (strcmp(name, "1031193940") == 0)
	{
		if (strcmp(pwd, "123456")) return false;
		id = 0;
	}
	else if (strcmp(name, "17691071306") == 0)
	{
		if (strcmp(pwd, "456789")) return false;
		id = 1;
	}
	else if (strcmp(name, "15829075532") == 0)
	{
		if (strcmp(pwd, "789456123")) return false;
		id = 2;
	}
	else
		return false;

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

		char ipBuffer[64] = { 0 };
		inet_ntop(AF_INET, &addr.sin_addr, ipBuffer, sizeof(ipBuffer));

		// 创建数据接收线程
		CLIENT_PTR clientConn = new Client();
		clientConn->sock = sockClient;
		clientConn->thread = (HANDLE)_beginthreadex(NULL, 0, func_thread_recv, (void*)clientConn, 0, &id);
		if (clientConn->thread == NULL)
		{
			// 反馈服务器错误
			DataPacket *pack = NewDataPacket();
			SetPacketHeadInfo(*pack, PACK_TYPE_ERROR, (long)time(NULL), PACK_FROM_SERVER);
			SendPacket(clientConn->sock, pack);
			ReleaseDataPacket(pack);

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
	clientConn->id = -1;

	// 等待登录信息
	bool loginSucc = false;
	DataPacket *packet = NewDataPacket();
	while (exitFlag == false && clientConn->sock > 0)
	{
		int lenOfData = recv(clientConn->sock, packet->data, sizeof(packet->data), 0);
		if (lenOfData <= 0) break;

		int type = GetPacketType(*packet);
		if (type == PACK_TYPE_LOGIN)
		{
			// 接收到登录信息并验证
			// 验证成功 - 发送登录被接受通知到请求客户端 / 广播玩家上线信息
			// 验证失败 - 发送登录被拒绝通知到请求客户端
			char name[32] = { 0 }, pwd[32] = { 0 };
			ParsePacketLogin(*packet, name, pwd);
			if (UserValidate(name, pwd, clientConn->id) && FindClientByID(clientConn->id) == nullptr)
			{
				loginSucc = true;
				mapClients.insert(std::pair<int, CLIENT_PTR>(clientConn->id, clientConn));
				
				DataPacket *packAccept = NewDataPacket(); // 登录成功反馈
				packAccept->from = PACK_FROM_SERVER;
				SetPacketHeadInfo(*packAccept, PACK_TYPE_ACCEPT, (long)time(NULL), clientConn->id);
				PushForwardPacket(packAccept);

				break;

			}
			else
			{
				SetPacketHeadInfo(*packet, PACK_TYPE_DENY, (long)time(NULL), PACK_FROM_SERVER);
				SendPacket(clientConn->sock, packet);
			}

		} // End if type

	} // End while exitFlag


	if (loginSucc)
	{

		// 接收数据包并分配
		while (exitFlag == false && clientConn->sock > 0)
		{
			int lenOfData = recv(clientConn->sock, packet->data, sizeof(packet->data), 0);
			if (lenOfData < 0) break;
			if (lenOfData == 0) continue;
			packet->from = clientConn->id;

			int tar = GetPacketIdentify(*packet);
			if (tar == PACK_TAR_SERVER)
			{
				// 是否为下线信息 - 是则发送下线广播
				if (GetPacketType(*packet) == PACK_TYPE_OFFLINE)
				{
					DataPacket *packOffline = NewDataPacket();
					SetPacketHeadInfo(*packOffline, PACK_TYPE_OFFLINE, (long)time(NULL), clientConn->id);
					SendPacket(clientConn->sock, packOffline);

					break;

				}

				PushHandlePacket(packet);
				packet = NewDataPacket();
			}
			else if (tar >= 0 || tar == PACK_TAR_BOARDCAST)
			{
				PushForwardPacket(packet);
				packet = NewDataPacket();

				SetEvent(hSignalSend);
			}

		} // End while - exitFlag

	} // End if - Login Succ

	ReleaseDataPacket(packet);

	std::map<int, CLIENT_PTR>::iterator itor = mapClients.find(clientConn->id);
	if (clientConn->id != -1 && itor != mapClients.end()) mapClients.erase(itor);
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
		// 阻塞并等待信号唤醒
		WaitForSingleObject(hSignalSend, INFINITE);

		DataPacket *pack = PopForwardPacket();
		if (pack == nullptr)
		{
			ResetEvent(hSignalSend);
			continue;
		}

		int tar = GetPacketIdentify(*pack);
		if (tar >= 0)
		{
			CLIENT_PTR ptrClient = FindClientByID(tar);
			if (ptrClient != nullptr)
			{
				SetPacketIdentify(*pack, pack->from);
				SendPacket(ptrClient->sock, pack);
			}
		}
		else if (tar == PACK_TAR_BOARDCAST)
		{
			SetPacketIdentify(*pack, PACK_FROM_SERVER);
			std::map<int, CLIENT_PTR>::iterator itor;
			for (itor = mapClients.begin(); itor != mapClients.end(); itor++)
			{
				if (itor->second->id != pack->from) 
					SendPacket(itor->second->sock, pack);
			}

		}

		ReleaseDataPacket(pack);
	}

	_endthreadex(0);
	return 0;
}


// 服务器数据处理进程
unsigned int _stdcall func_thread_handle(void *arg)
{
	while (exitFlag == false)
	{
		// 阻塞并等待信号唤醒
		WaitForSingleObject(hSignalHandle, INFINITE);

		DataPacket *pack = PopHandlePacket();
		if (pack == nullptr)
		{
			ResetEvent(hSignalHandle);
			continue;
		}

		int type = GetPacketType(*pack);

		// TODO 处理消息

		ReleaseDataPacket(pack);

	}

	_endthreadex(0);
	return 0;
}