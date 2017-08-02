#include "DbService.h"

bool DbService::UserValidate(const ServString strName, const ServString strPassword, int * id) {
	//构造sql语句
	ServString strSQL = ServString("select * from game_server.users where name = \'");
	
	strSQL = strSQL + strName + "\'";
	std::vector<ServDbResult> vecDbResult = ServDatabase::Query(strSQL);
	if (vecDbResult.size()!=1){
		std::cout << "dbr size:" << vecDbResult.size() << std::endl;
			
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
