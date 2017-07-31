#include "DbService.h"
using namespace std;
bool DbService::UserValidate(const string strName, const string strPassword, int * id) {
	//构造sql语句
	string strSQL = string("select * from game_server.users where name = \'");
	strSQL = strSQL + strName + "\'";
	cout << strSQL << endl;
	vector<ServDbResult> vecDbResult = ServDatabase::Query(strSQL);
	if (vecDbResult.size()!=1){
		cout << "dbr size:" << vecDbResult.size() << endl;
			
		//存在同名客户或客户不存在视为登录失败
		return false;
	}
	ServDbResult dbrUser = vecDbResult[0];
	if (dbrUser.Get("password")== strPassword) {
		*id = atoi(dbrUser.Get("id").c_str()); ;
		return true;
	}
	return false;
}
