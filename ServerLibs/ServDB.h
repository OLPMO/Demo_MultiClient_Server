#ifndef _SERV_DB_H_
#define _SERV_DB_H_

#include "../Services/DbService.h"

/*

	���������ݿ����Ӳ�������ݴ���

*/

/*
// �����ݿ�ƥ���û���������
inline bool DB_Validate_User(ServString name, ServString pwd, int &id)
{
	return DbService::UserValidate(name, pwd, &id);
}
*/


// 2017 - 07 - 18 �޸�
// ��Ϸ��ֻ��������� ������Ҫ���ص�IDֻ�� 0 �� 1 ����ֵ
// ������ֱ�Ӵ����ݿ��л�ȡ


#define USER_FLAG_NONE (0) // ������ҵ�¼
#define USER_FLAG_0    (1) // 0 ��λ���ѵ�¼
#define USER_FLAG_1    (2) // 1 ��λ���ѵ�¼
#define USER_FLAG_FULL (3) // ����λ�þ���ռ��


extern int userFlag; // �û���־ ��һλ�͵ڶ�λ�ֱ��ʶ 0 �ź� 1 �� userName����ʹ��

extern int userIdOfDatabase[2]; // �ѵ�¼���û������ݿ��е�ID


// �����ݿ�ƥ���û���������
bool DB_Validate_User(ServString name, ServString pwd, int &id);

void clearUserCache(int id); // ���ĳ���û��ĵ�¼��¼


#endif