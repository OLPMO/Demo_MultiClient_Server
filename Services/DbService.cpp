#include "DbService.h"

bool DbService::UserValidate(const ServString strName, const ServString strPassword, int * id) {
	//����sql���
	ServString strSQL = ServString("select * from game_server.users where name = \'");
	
	strSQL = strSQL + strName + "\'";
	std::vector<ServDbResult> vecDbResult = ServDatabase::Query(strSQL);
	if (vecDbResult.size()!=1){
		std::cout << "dbr size:" << vecDbResult.size() << std::endl;
			
		//����ͬ���ͻ���ͻ���������Ϊ��¼ʧ��
		return false;
	}
	ServDbResult dbrUser = vecDbResult[0];
	if (dbrUser.Get("password")== strPassword) {
		*id = atoi(dbrUser.Get("id").c_str()); ;
		return true;
	}
	return false;
}
