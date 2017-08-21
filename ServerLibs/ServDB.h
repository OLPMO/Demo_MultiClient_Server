#ifndef _SERV_DB_H_
#define _SERV_DB_H_

#include "../Services/DbService.h"

/*

	用于与数据库连接层进行数据传递

*/

/*
// 从数据库匹配用户名和密码
inline bool DB_Validate_User(ServString name, ServString pwd, int &id)
{
	return DbService::UserValidate(name, pwd, &id);
}
*/


// 2017 - 07 - 18 修改
// 游戏中只有两个人物， 所以需要返回的ID只有 0 和 1 两个值
// 而不能直接从数据库中获取


#define USER_FLAG_NONE (0) // 尚无玩家登录
#define USER_FLAG_0    (1) // 0 号位置已登录
#define USER_FLAG_1    (2) // 1 好位置已登录
#define USER_FLAG_FULL (3) // 所有位置均已占满


extern int userFlag; // 用户标志 第一位和第二位分别标识 0 号和 1 号 userName有无使用

extern int userIdOfDatabase[2]; // 已登录的用户在数据库中的ID


// 从数据库匹配用户名和密码
bool DB_Validate_User(ServString name, ServString pwd, int &id);

void clearUserCache(int id); // 清除某个用户的登录记录


#endif