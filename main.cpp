#include <queue>
#include <iostream>
using namespace std;

#include <time.h>

#include "ServerLibs\Server.h"

#define SERVER_PORT (8000) // 端口号

int main(void)
{
	
	if (Start(SERVER_PORT))
	{
		cout << "[SERVER] : Has Been Started" << endl;

		// 维持服务器
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