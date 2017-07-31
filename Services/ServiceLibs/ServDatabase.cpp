#include "ServDatabase.h"

MYSQL *ServDatabase::pServ2DbSock = NULL;
MYSQL ServDatabase::mqServ2Db;
MYSQL* ServDatabase::Connect2MySQL(char *pStrDbHost,char *pStrUserName, char *pStrUserPass,char *pStrDbName,int nPort){

	mysql_init(&mqServ2Db);
	if (!(pServ2DbSock = mysql_real_connect(&mqServ2Db, pStrDbHost, pStrUserName, pStrUserPass, pStrDbName, nPort, NULL, 0))){
		printf("Cannot connect to database:%s\n", mysql_error(pServ2DbSock));
		return NULL;
	}

	return pServ2DbSock;
}

MYSQL *ServDatabase::SetDbInstance(char *pStrDbHost, char *pStrUserName, char *pStrUserPass, char *pStrDbName, int nPort) {
	if (pServ2DbSock != NULL) {
		mysql_close(pServ2DbSock);
	}
	mysql_init(&mqServ2Db);
	pServ2DbSock = Connect2MySQL(pStrDbHost, pStrUserName, pStrUserPass, pStrDbName, nPort);
	return pServ2DbSock;
}
MYSQL *ServDatabase::GetDbInstance() {
	if (pServ2DbSock == NULL) {
		pServ2DbSock = SetDbInstance("localhost", "root", "123456", "game_server", 3306);
		
	}
	return pServ2DbSock;
}

bool ServDatabase::UserValidate(const string strName, const string strPassword, int *pId) {
	MYSQL *pDbSock = GetDbInstance();

	//根据name来查询用户信息
	string strSQL("SELECT * FROM users where `name` =  \'");
	strSQL = strSQL + strName+"\'";

	//cout << strSQL <<endl ;
	//strSQL.c_str():把string类型转换为char*类型
	if (mysql_query(pDbSock, strSQL.c_str())) {
		printf("Query Failed:%s", mysql_error(pDbSock));
		return false;
	}
	MYSQL_RES *pResult;
	if (!(pResult = mysql_store_result(pDbSock))){
		printf("Failed to store result:%s", mysql_error(pDbSock));
		return false;
	}
	int uLine = (unsigned)mysql_num_rows(pResult);
	if (uLine == 0) {
		return false;
	}
	MYSQL_ROW row;
	/**
	* MYSQL_ROW STDCALL mysql_fetch_row(MYSQL_RES *result);
	* 检索行
	*/
	row = mysql_fetch_row(pResult);
	if (mysql_num_fields(pResult)<USERS_FIELD_NUM) {
		return false;
	}
	//解析所获得的结果
	string strDbUserId(row[0]);
	string strDbUserName(row[1]);
	string strDbUserPass(row[2]);
	if (strPassword == strDbUserPass) {
		*pId = stoi(strDbUserId);
		return true;
	}
	return false;
}

vector<ServDbResult> ServDatabase::Query(string strSQL){
	MYSQL *pDbSock = GetDbInstance();
	vector<ServDbResult> vecSdrResult;

	//strSQL.c_str():把string类型转换为char*类型
	if (mysql_query(pDbSock, strSQL.c_str())) {
		printf("Query Failed:%s", mysql_error(pDbSock));
		return vecSdrResult;
	}
	int nAffectLine = (unsigned)mysql_affected_rows(pDbSock);
	//cout << "in the fun: " << nAffectLine << endl;
	vecSdrResult.clear();
	if (nAffectLine <= 0) {
		MYSQL_RES *pResult;
		if (!(pResult = mysql_store_result(pDbSock))){
			printf("Failed to store result:%s", mysql_error(pDbSock));
			return vecSdrResult;
		}
	
		int nRowsLine = (unsigned)mysql_num_rows(pResult);
		for (int i = 0; i < nRowsLine;i++) {
			ServDbResult sdrResult;
			//sdrResult.SetResultRows(nRowsLine);
			//获得字段数  
			int nFieldNum = mysql_num_fields(pResult);
			MYSQL_FIELD *pField(NULL);
			pField = mysql_fetch_fields(pResult);
			MYSQL_ROW msrRow;
			/**
			* MYSQL_ROW STDCALL mysql_fetch_row(MYSQL_RES *result);
			* 检索行
			*/
			msrRow = mysql_fetch_row(pResult);
			for (int j = 0; j<nFieldNum; j++) {
				//blob    
				MYSQL_FIELD *pMsFieldTemp = pField + j;
				string strFieldName(pMsFieldTemp->name);
				sdrResult.Set(strFieldName,msrRow[j]);
			}
			vecSdrResult.push_back(sdrResult);

			
		}
		
	}else{
		ServDbResult sdrResult;
		sdrResult.SetAffectedRows(nAffectLine);
		vecSdrResult.push_back(sdrResult);
		
	}
	//cout << "in the fun:"<<vecSdrResult.size() << endl;;
	return vecSdrResult;

}
