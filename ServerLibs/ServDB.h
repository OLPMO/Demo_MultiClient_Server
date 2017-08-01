#ifndef _SERV_DB_H_
#define _SERV_DB_H_

#include "../Services/DbService.h"

/*

	用于与数据库连接层进行数据传递

*/


// 从数据库匹配用户名和密码
inline bool DB_Validate_User(ServString name, ServString pwd, int &id)
{
	return DbService::UserValidate(name, pwd, &id);
}



#endif