#ifndef _SERV_DB_H_
#define _SERV_DB_H_

#include "../Services/DbService.h"

/*

	���������ݿ����Ӳ�������ݴ���

*/


// �����ݿ�ƥ���û���������
inline bool DB_Validate_User(ServString name, ServString pwd, int &id)
{
	return DbService::UserValidate(name, pwd, &id);
}



#endif