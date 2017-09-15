#include <queue>
#include <iostream>
using namespace std;

#include <time.h>

#include "ServerLibs\Server.h"

#define SERVER_PORT (8000) // �˿ں�

// �ַ���ת��ΪСд
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
		cout << "[SERVER] : Version - ������" << endl;
		cout << "[SERVER] : Has Been Started" << endl;

		// ѭ���ȴ� - ά�ַ�����
		char cmd[128] = { 0 };
		while (cin >> cmd)
		{
			strConvertToLowWord(cmd); //  ת��ΪСд�� - ���Դ�Сд

			if (strcmp(cmd, "clear") == 0) // ����
			{
				system("cls");
				cout << "[SERVER] : Version - ������" << endl;
				cout << "[SERVER] : Has Been Started" << endl;
			}
			else if(strcmp(cmd, "exit") == 0) // �˳�
			{
				break;
			}

		}
	}

	Close();

	cout << "[SERVER] : Has Been Closed" << endl;
	

	return 0;
}