#include "Server.h"


// ȫ�ֱ���

bool exitFlag;

SOCKET sockServ;

HANDLE hThreadAccept;

HANDLE hThreadSend[SEND_THREAD_NUM];

HANDLE hThreadHandle[HANDLE_THREAD_NUM];

std::map<int, CLIENT_PTR> mapClients;  // ͼ - �ͻ�����Ϣ

ServQueue<DataPacket*, 512> queForward; // ת������ - �������ݰ���Ҫת��

ServQueue<DataPacket*, 128> queHandle;  // ������� - �������ݰ���Ҫ����������

ServMemory<DataPacket> packetPool; // ���ݰ��ڴ��

std::mutex mtxQueForward; // ת�����л�����

std::mutex mtxQueHandle;  // ������л�����

std::mutex mtxPacketPool; // ���ڴ�ػ�����


// ����ʵ��

// ����
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

	// ����������ս���
	unsigned int id = 0;
	hThreadAccept = (HANDLE)_beginthreadex(NULL, 0, func_thread_accept, NULL, 0, &id);
	if (hThreadAccept == NULL)
	{
		Close();
		return false;
	}

	// �������ݷ��ͽ���
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

	// �������������ݴ������
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


// �ر�
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


// ��֤�Ƿ�ɵ�½������ID
bool UserValidate(const char *name, const char *pwd, int &id)
{
	// TODO ��д������� - ��ǰ����ֻΪ����

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


// �������ӽ���
unsigned int _stdcall func_thread_accept(void * parm)
{
	printf("I am waiting\n"); //////////////////////////////////////////

	unsigned int id = 0;
	int lenOfAddr = sizeof(sockaddr_in);
	while (exitFlag == false)
	{
		sockaddr_in addr = { 0 };
		SOCKET sockClient = accept(sockServ, (sockaddr*)&addr, &lenOfAddr);
		if (sockClient == INVALID_SOCKET) continue;

		printf("Connection ... \n"); //////////////////////////////////////////

		// �������ݽ����߳�
		CLIENT_PTR clientConn = new Client();
		clientConn->sock = sockClient;
		clientConn->thread = (HANDLE)_beginthreadex(NULL, 0, func_thread_recv, (void*)clientConn, 0, &id);
		if (clientConn->thread == NULL)
		{
			mtxPacketPool.lock(); // lock
			DataPacket *pack = packetPool.Alloc();
			mtxPacketPool.unlock(); // Unlock
			
			SetPacketType(*pack, PACK_TYPE_ERROR);
			SetPacketTime(*pack, (long)time(NULL));
			SetPacketIdentify(*pack, PACK_FROM_SERVER);
			send(clientConn->sock, pack->data, sizeof(pack->data), 0);

			mtxPacketPool.lock(); // Lock
			packetPool.Release(pack);
			mtxPacketPool.unlock(); // Unlock

			closesocket(clientConn->sock);
			delete clientConn;
		}
	}

	_endthreadex(0);
	return 0;
}


// �������ݽ���
unsigned int _stdcall func_thread_recv(void * parm)
{
	if (parm == nullptr)
	{
		_endthreadex(0);
		return 0;
	}

	printf("New Thread recv\n"); //////////////////////////////////////////

	CLIENT_PTR clientConn = (CLIENT_PTR)parm;
	clientConn->id = -1;

	// �ȴ���¼��Ϣ
	mtxPacketPool.lock(); // Lock
	DataPacket *packet = packetPool.Alloc();
	mtxPacketPool.unlock(); // Unlock

	bool loginSucc = false;
	while (exitFlag == false)
	{
		int lenOfData = recv(clientConn->sock, packet->data, sizeof(packet->data), 0);
		if (lenOfData <= 0) break;

		int type = GetPacketType(*packet);
		if (type == PACK_TYPE_LOGIN)
		{
			// TODO - ��¼����
			char name[32] = { 0 }, pwd[32] = { 0 };
			ParsePacketLogin(*packet, name, pwd);
			printf("User name : %s.\n", name);
			printf("Password  : %s.\n", pwd);
			if (UserValidate(name, pwd, clientConn->id) && mapClients.find(clientConn->id) == mapClients.end())
			{
				mapClients.insert(std::pair<int, CLIENT_PTR>(clientConn->id, clientConn));
				loginSucc = true;
			}
			else
			{
				printf("Login has been denied\n"); //////////////////////////////////////

				SetPacketType(*packet, PACK_TYPE_DENY);
				SetPacketTime(*packet, (long)time(NULL));
				SetPacketIdentify(*packet, PACK_FROM_SERVER);
				send(clientConn->sock, packet->data, sizeof(packet->data), 0);
			}

			break;

		} // End if type

	} // End while exitFlag


	if (loginSucc)
	{

		printf("Login success \n"); //////////////////////////////////////////

		// �������ݰ�������
		while (exitFlag == false)
		{
			int lenOfData = recv(clientConn->sock, packet->data, sizeof(packet->data), 0);
			if (lenOfData <= 0) 
				break;
			else
			{
				packet->from = clientConn->id;

				int tar = GetPacketIdentify(*packet);
				if (tar == PACK_TAR_SERVER)
				{
					mtxQueHandle.lock(); // Lock
					queHandle.Push(packet);
					mtxQueHandle.unlock(); // Unlock
				}
				else if (tar >= 0 || tar == PACK_TAR_BOARDCAST)
				{
					mtxQueForward.lock(); // Lock
					queForward.Push(packet);
					mtxQueForward.unlock(); // Unlock
				}

				mtxPacketPool.lock(); // Lock
				packet = packetPool.Alloc();
				mtxPacketPool.unlock(); // Unlock

			}

		} // End while exitFlag

	} // Login Succ

	mtxPacketPool.lock(); // Lock
	packetPool.Release(packet);
	mtxPacketPool.unlock(); // Unlock

	std::map<int, CLIENT_PTR>::iterator itor = mapClients.find(clientConn->id);
	if (clientConn->id != -1 && itor != mapClients.end()) mapClients.erase(itor);
	closesocket(clientConn->sock);
	delete clientConn;

	_endthreadex(0);
	return 0;
}


// �������ݽ���
unsigned int _stdcall func_thread_send(void * parm)
{
	while (exitFlag == false)
	{
		if (queForward.Empty()) continue;

		mtxQueForward.lock(); // Lock
		DataPacket *pack = queForward.Pop();
		mtxQueForward.unlock(); // Unlock
		if (pack == nullptr) continue;

		int tar = GetPacketIdentify(*pack);
		if (tar >= 0)
		{
			std::map<int, CLIENT_PTR>::iterator itor = mapClients.find(tar);
			if (itor != mapClients.end())
			{
				SetPacketIdentify(*pack, pack->from);
				std::map<int, CLIENT_PTR>::iterator itor = mapClients.find(tar);
				if (itor != mapClients.end())
					send(itor->second->sock, pack->data, sizeof(pack->data), 0);
			}

		}
		else if (tar == PACK_TAR_BOARDCAST)
		{
			SetPacketIdentify(*pack, PACK_FROM_SERVER);
			std::map<int, CLIENT_PTR>::iterator itor;
			for (itor = mapClients.begin(); itor != mapClients.end(); itor++)
			{
				if (itor->second->id == pack->from) continue;
				send(itor->second->sock, pack->data, sizeof(pack->data), 0);
			}

		}

		mtxPacketPool.lock(); // Lock
		packetPool.Release(pack);
		mtxPacketPool.unlock(); // Unlock
	}

	_endthreadex(0);
	return 0;
}


// ���������ݴ������
unsigned int _stdcall func_thread_handle(void *arg)
{
	while (exitFlag == false)
	{
		if (queHandle.Empty()) continue;

		mtxQueHandle.lock(); // Lock
		DataPacket *pack = queHandle.Pop();
		mtxQueHandle.unlock(); // Unlock
		if (pack == nullptr) continue;

		int type = GetPacketType(*pack);

		// TODO ������Ϣ

		mtxPacketPool.lock(); // Lock
		packetPool.Release(pack);
		mtxPacketPool.unlock(); // Unlock

	}

	_endthreadex(0);
	return 0;
}