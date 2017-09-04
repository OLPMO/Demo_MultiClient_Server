#include "Server.h"
#include "ServDB.h"

// ȫ�ֱ���

bool exitFlag = false;


SOCKET sockServ = INVALID_SOCKET;

HANDLE hThreadAccept = 0;

HANDLE hThreadSend[SEND_THREAD_NUM] = {0};

HANDLE hThreadHandle[HANDLE_THREAD_NUM] = {0};

HANDLE hSignalSend = 0;   // �����߳������ź�

HANDLE hSignalHandle = 0; // �����߳������ź�


ServQueue<DataPacket*, 512> queForward; // ת������ - �������ݰ���Ҫת��

ServQueue<DataPacket*, 128> queHandle;  // ������� - �������ݰ���Ҫ����������

ServMemory<DataPacket> packetPool; // ���ݰ��ڴ��


std::mutex mtxQueForward; // ת�����л�����

std::mutex mtxQueHandle;  // ������л�����

std::mutex mtxPacketPool; // ���ڴ�ػ�����


CLIENT_PTR listClients[2] = { 0 }; // �ͻ�����Ϣ


// ����ʵ��

// ����������
// Parm : port �˿ں�
// Parm : forceCoalesce �Ƿ��������㷨 - ���������Լ�����Ϸ�з�����Ӧ�ٶ�
// Return : �Ƿ������ɹ�
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

	// �������ݰ������㷨
	// ���������������Ϊ�������ӿ����ݰ������ٶ�
	// ����Ҫ���ʹ���С���ݰ�ʱʹ��
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


// ��֤�Ƿ�ɵ�½������ID
// Parm : name �û���
// Parm : pwd  ����֤������
// Parm : id   ���ط�����û��ı�ʶ
// Return : ��֤�Ƿ�ɹ�
bool UserValidate(const char *name, const char *pwd, int &id)
{
	return DB_Validate_User(name, pwd, id);
}


// �������ӽ���
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

		// �������ݽ����߳�
		clientConn.sock = sockClient;
		clientConn.thread = (HANDLE)_beginthreadex(NULL, 0, func_thread_recv, (void*)&clientConn, 0, &id);
		if (clientConn.thread == NULL)
		{
			// ��������������
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


// �ȴ���¼���ݰ�����֤
// �����øú������߳�����ֱ����֤��¼�ɹ���ͻ��˶Ͽ�����
// Parm   : pClientConn �ͻ�����Ϣ
// Return : �Ƿ��¼�ɹ�
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
			// ���յ���¼��Ϣ����֤
			char name[32] = { 0 }, pwd[32] = { 0 };
			ParsePacketLogin(*packetLogin, name, pwd);
			if (UserValidate(name, pwd, pClientConn->id) && FindClientByID(pClientConn->id) == nullptr)
			{
				loginSucceed = true;
				LoginRequestAccept(pClientConn); // ���ܵ�¼
				printf("User Login ACCEPTED : %s\n", name); // DEBUG
			}
			else
			{
				LoginRequestDeny(pClientConn); // �ܾ���¼
				printf("User Login DENIED : %s\n", name); // DEBUG
			}

			break;
		}
		else if (type == PACK_TYPE_OFFLINE)
		{
			// �ͻ������� - ������¼�ȴ�
			break;
		}

	} // End while exitFlag

	ReleaseDataPacket(packetLogin);

	return loginSucceed;
}


// �������ݽ���
// Parm : parm ָ���̷߳����û���Client�ṹ
// Return : ������
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

	// �����ȴ���¼��֤
	bool loginSucceed = WaitForLoginPacket(&clientConn);

	// ��¼�ɹ� - ѭ���������ݰ�������
	// �����¼ʧ�� - ��ִ��if�����
	if (loginSucceed)
	{
		// �������ݰ�������
		DataPacket *packRecv = NewDataPacket();
		while (exitFlag == false && clientConn.sock != INVALID_SOCKET)
		{
			packRecv->bytes = recv(clientConn.sock, packRecv->data, PACK_TOTL_BYTE, 0);
			if (packRecv->bytes <= 0) break;
			packRecv->from = clientConn.id;

			int tar = GetPacketIdentify(*packRecv);
			if (tar == PACK_TAR_SERVER)
			{
				// �Ƿ�Ϊ������Ϣ - ���������߹㲥
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

	// �ӿͻ��˼�¼����ɾ����ǰ�ͻ��˼�¼�������¼��Ϣ
	clearUserCache(clientConn.id);
	closesocket(clientConn.sock);
	clientConn.sock = INVALID_SOCKET;

	printf("The client id is %d\n", clientConn.id);

	listClients[clientConn.id] = nullptr;

	_endthreadex(0);
	return 0;
}


// �������ݽ���
// Parm : parm δʹ�õ�
// Return : ������
unsigned int _stdcall func_thread_send(void * parm)
{
	while (exitFlag == false)
	{
		// �������ȴ��źŻ���
		WaitForSingleObject(hSignalSend, INFINITE);

		// �Ӷ�����ȡ��һ�����ݰ�
		DataPacket *packForward = PopForwardPacket();
		if (packForward == nullptr)
		{
			ResetEvent(hSignalSend);
			continue;
		}

		// �����ݽ���ת��
		int tar = GetPacketIdentify(*packForward);
		if (tar == PACK_TAR_BOARDCAST)
		{
			// �㲥 - ת��������Դ֮������пͻ��� 
			SetPacketIdentify(*packForward, packForward->from);

			if(packForward->from != 0 && listClients[0])
				SendPacket(listClients[0]->sock, packForward);

			if(packForward->from != 1 && listClients[1])
				SendPacket(listClients[1]->sock, packForward);
		}
		else if(tar == PACK_TAR_FROM)
		{
			// ���� - ת������Դ
			CLIENT_PTR pClient = FindClientByID(packForward->from);
			if(pClient)
			{
				SetPacketIdentify(*packForward, packForward->from);
				SendPacket(pClient->sock, packForward);
			}
		}
		else
		{
			// ���� - ת����ָ���ͻ���
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


// ���������ݴ������
// Parm : arg δʹ�õ�
// Return : ������
unsigned int _stdcall func_thread_handle(void *arg)
{
	while (exitFlag == false)
	{
		// �������ȴ��źŻ���
		WaitForSingleObject(hSignalHandle, INFINITE);

		// �Ӷ�����ȡ��һ������������ݰ�
		DataPacket *packHandling = PopHandlePacket();
		if (packHandling == nullptr)
		{
			ResetEvent(hSignalHandle);
			continue;
		}

		// �������ݰ���������ݰ����д���
		int type = GetPacketType(*packHandling);
		switch (type)
		{
		case PACK_TYPE_SYNC: // ʱ��ͬ������
			TimeSyncRequestFeedback(packHandling->from);
			break;

		case PACK_TYPE_RECALC_TIMESTAMP: // ����ʱ���������
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