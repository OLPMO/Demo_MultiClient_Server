#ifndef SERV_DATABASE_H
#define SERV_DATABASE_H

#include <cstdio>
#include <string>
#include <iostream>
#include <vector>

#include <Windows.h>
#include <mysql.h>

#include "ServDbResult.h"
#pragma comment(lib,"libmysql.lib")

#define USERS_FIELD_NUM 3

class ServDatabase{
public:
	static std::vector<ServDbResult> Query(ServString strSQL);
	static MYSQL *SetDbInstance(char *pStrDbHost, char *pStrUserName, char *pStrUserPass, char *pStrDbName, int nPort);
private:
	static MYSQL mqServ2Db;
	static MYSQL *pServ2DbSock;
	static MYSQL *GetDbInstance();
	static MYSQL *Connect2MySQL(char *pStrDbHost, char *pStrUserName, char *pStrUserPass, char *pStrDbName, int nPort);

};

#endif