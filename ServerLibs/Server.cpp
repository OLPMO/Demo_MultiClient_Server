#include "Server.h"


// ȫ�ֱ���

bool exitFlag;

SOCKET sockServ;

HANDLE hThreadAccept;

HANDLE hThreadSend[SEND_THREAD_NUM];

HANDLE hThreadHandle[HANDLE_THREAD_NUM];

HANDLE hSignalSend;   // �����߳������ź�

HANDLE hSignalHandle; // �����߳������ź�


ServQueue<DataPacket*, 512> queForward; // ת������ - �������ݰ���Ҫת��

ServQueue<DataPacket*, 128> queHandle;  // ������� - �������ݰ���Ҫ����������

ServMemory<DataPacket> packetPool; // ���ݰ��ڴ��


std::map<int, CLIENT_PTR> mapClients;  // ͼ - �ͻ�����Ϣ

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

	// �������������ݴ������
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


// �ر�
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
			DataPacket *pack = NewDataPacket();
			SetPacketHeadInfo(*pack, PACK_TYPE_ERROR, (long)time(NULL), PACK_FROM_SERVER);
			send(clientConn->sock, pack->data, sizeof(pack->data), 0);
			ReleaseDataPacket(pack);

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
	bool loginSucc = false;
	DataPacket *packet = NewDataPacket();
	while (exitFlag == false && clientConn->sock > 0)
	{
		int lenOfData = recv(clientConn->sock, packet->data, sizeof(packet->data), 0);
		if (lenOfData <= 0) break;

		int type = GetPacketType(*packet);
		if (type == PACK_TYPE_LOGIN)
		{
			// ���յ���¼��Ϣ����֤
			// ��֤�ɹ� - ���͵�¼������֪ͨ������ͻ��� / �㲥���������Ϣ
			// ��֤ʧ�� - ���͵�¼���ܾ�֪ͨ������ͻ���
			char name[32] = { 0 }, pwd[32] = { 0 };
			ParsePacketLogin(*packet, name, pwd);
			printf("User name : %s.\n", name); ////////////////////////////////////////////////////////
			printf("Password  : %s.\n", pwd);  ////////////////////////////////////////////////////////
			if (UserValidate(name, pwd, clientConn->id) && mapClients.find(clientConn->id) == mapClients.end())
			{
				loginSucc = true;
				mapClients.insert(std::pair<int, CLIENT_PTR>(clientConn->id, clientConn));
				
				DataPacket *packAccept = NewDataPacket(); // ��¼�ɹ�����
				packAccept->from = PACK_FROM_SERVER;
				SetPacketHeadInfo(*packAccept, PACK_TYPE_ACCEPT, (long)time(NULL), clientConn->id);
				PushForwardPacket(packAccept);

				DataPacket *packOnline = NewDataPacket(); // ������߹㲥
				packOnline->from = clientConn->id;
				SetPacketHeadInfo(*packOnline, PACK_TYPE_NEWONE, (long)time(NULL), PACK_TAR_BOARDCAST);
				PushForwardPacket(packOnline);

				printf("[ %s ] Login Accepted\n", name); //////////////////////////////////////

				break;

			}
			else
			{
				SetPacketHeadInfo(*packet, PACK_TYPE_DENY, (long)time(NULL), PACK_FROM_SERVER);
				send(clientConn->sock, packet->data, sizeof(packet->data), 0);

				printf("Login has been denied\n"); //////////////////////////////////////
			}

		} // End if type

	} // End while exitFlag


	if (loginSucc)
	{

		printf("Login success \n"); //////////////////////////////////////////

		// �������ݰ�������
		while (exitFlag == false && clientConn->sock > 0)
		{
			int lenOfData = recv(clientConn->sock, packet->data, sizeof(packet->data), 0);
			if (lenOfData <= 0) break;
			packet->from = clientConn->id;

			int tar = GetPacketIdentify(*packet);
			if (tar == PACK_TAR_SERVER)
			{
				// �Ƿ�Ϊ������Ϣ - ���������߹㲥
				if (GetPacketType(*packet) == PACK_TYPE_OFFLINE)
				{
					DataPacket *packOffline = NewDataPacket();
					SetPacketHeadInfo(*packOffline, PACK_TYPE_OFFLINE, (long)time(NULL), clientConn->id);
					send(clientConn->sock, packOffline->data, sizeof(packOffline->data), 0);

					break;

				}

				PushHandlePacket(packet);

			}
			else if (tar >= 0 || tar == PACK_TAR_BOARDCAST)
			{
				PushForwardPacket(packet);
			}

			packet = NewDataPacket();

		} // End while exitFlag

	} // Login Succ

	ReleaseDataPacket(packet);

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
		// �������ȴ��źŻ���
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
			std::map<int, CLIENT_PTR>::iterator itor = mapClients.find(tar);
			if (itor != mapClients.end())
			{
				SetPacketIdentify(*pack, pack->from);
				std::map<int, CLIENT_PTR>::iterator itor = mapClients.find(tar);
				if (itor != mapClients.end())
					send(itor->second->sock, pack->data, sizeof(pack->data), 0);
			}

			printf("A Message\n"); //////////////////////////////////////
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

			printf("A Board cast\n"); //////////////////////////////////////

		}

		ReleaseDataPacket(pack);
	}

	_endthreadex(0);
	return 0;
}


// ���������ݴ������
unsigned int _stdcall func_thread_handle(void *arg)
{
	while (exitFlag == false)
	{
		// �������ȴ��źŻ���
		WaitForSingleObject(hSignalHandle, INFINITE);

		DataPacket *pack = PopHandlePacket();
		if (pack == nullptr)
		{
			ResetEvent(hSignalHandle);
			continue;
		}

		int type = GetPacketType(*pack);

		// TODO ������Ϣ

		ReleaseDataPacket(pack);

	}

	_endthreadex(0);
	return 0;
}