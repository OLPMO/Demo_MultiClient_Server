#include <queue>
#include <iostream>
using namespace std;

#include <time.h>

#include "ServerLibs\Server.h"

#define SERVER_PORT (8000) // �˿ں�

int main(void)
{
	
	if (Start(SERVER_PORT))
	{
		cout << "[SERVER] : Has Been Started" << endl;

		// ά�ַ�����
		char cmd[128] = { 0 };
		while (true)
		{
			cin >> cmd;
			if (strcmp(cmd, "exit") == 0)
				break;
		}
	}

	Close();

	cout << "[SERVER] : Has been closed" << endl;
	

	return 0;
}