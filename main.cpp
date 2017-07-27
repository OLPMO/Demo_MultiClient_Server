#include <queue>
#include <iostream>
using namespace std;

#include <time.h>

#include "Server.h"

#define SERVER_PORT (8000) // ¶Ë¿ÚºÅ

int main(void)
{
	
	if (Start(SERVER_PORT))
	{
		// TODO 
		cout << "Start success" << endl;
	}

	Close();
	

	return 0;
}