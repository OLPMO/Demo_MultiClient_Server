#include <queue>
#include <iostream>
using namespace std;

#include <time.h>

#include "ServerLibs\Server.h"

#define SERVER_PORT (8000) // 端口号

// 字符串转换为小写
void strConvertToLowWord(char *str)
{
	for(char *ptr = str; *ptr != '\0'; ptr++)
	{
		if(*ptr >= 'A' && *ptr <= 'Z')
			(*ptr) += 32;
	}
}

int main(void)
{
	// DEBUG 
	//FLogger::addLog("debug", "./debug.txt");
	/////////////////////

	if (Start(SERVER_PORT))
	{
		cout << "[SERVER] : Version - 梁宇轩" << endl;
		cout << "[SERVER] : Has Been Started" << endl;

		// 循环等待 - 维持服务器
		char cmd[128] = { 0 };
		while (cin >> cmd)
		{
			strConvertToLowWord(cmd); //  转换为小写串 - 忽略大小写

			if (strcmp(cmd, "clear") == 0) // 清屏
			{
				system("cls");
				cout << "[SERVER] : Version - 梁宇轩" << endl;
				cout << "[SERVER] : Has Been Started" << endl;
			}
			else if(strcmp(cmd, "exit") == 0) // 退出
			{
				break;
			}

		}
	}

	Close();

	cout << "[SERVER] : Has Been Closed" << endl;
	

	return 0;
}