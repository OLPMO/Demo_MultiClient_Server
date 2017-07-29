#ifndef SERV_DATABASE_H
#define SERV_DATABASE_H


#include <cstdio>
#include <string>
#include <mysql.h>
#pragma comment(lib,"libmysql.lib")

#define USERS_FIELD_NUM 3

using namespace std;
class ServDatabase{
public:
	static bool UserValidate(const string name, const string password, int *id);

private:
	static MYSQL mqServ2Db;
	static MYSQL *pServ2DbSock;
	static MYSQL *GetDbInstance();
	static MYSQL *Connect2MySQL(char *pStrDbHost, char *pStrUserName, char *pStrUserPass, char *pStrDbName, int nPort);
	static MYSQL *SetDbInstance(char *pStrDbHost, char *pStrUserName, char *pStrUserPass, char *pStrDbName, int nPort);
};

#endif