#include "ServDB.h"


int userFlag = 0;

int userIdOfDatabase[2];


bool DB_Validate_User(ServString name, ServString pwd, int &id)
{
	id = -1;

	// ��ѯ���ݿ��û��������Ƿ�ƥ��
	int tmpUserID = -1;
	if(DbService::UserValidate(name, pwd, &tmpUserID))
	{
		// �ж����޿����û�λ��
		switch(userFlag)
		{
		case USER_FLAG_NONE: // û����Ϣ
			{
				userIdOfDatabase[0] = tmpUserID;
				userFlag |= USER_FLAG_0;
				id = 0;
			}
			break;

		case USER_FLAG_0: // 0 ��λ������Ϣ
			{
				if(userIdOfDatabase[0] != tmpUserID)
				{
					userIdOfDatabase[1] = tmpUserID;
					userFlag |= USER_FLAG_1;
					id = 1;
				}
				else 
					id = 0;
			}
			break;

		case USER_FLAG_1: // 1 ��λ������Ϣ
			{
				if(userIdOfDatabase[1] != tmpUserID)
				{
					userIdOfDatabase[0] = tmpUserID;
					userFlag |= USER_FLAG_0;
					id = 0;
				}
				else 
					id = 1;
			}
			break;

		case USER_FLAG_FULL: // ����λ�þ�����Ϣ
			{
				if(userIdOfDatabase[0] == tmpUserID)
				{
					id = 0;
				}
				else if(userIdOfDatabase[1] == tmpUserID)
				{
					id = 1;
				}
			}
			break;
			
		default:
			break;
		
		} // end switch

	} // end if

	return (id == 0 || id == 1);
}

// ���ĳ���û��ĵ�¼��¼
void clearUserCache(int id)
{
	int USER_FLAGS[2] = { USER_FLAG_0, USER_FLAG_1 };
	if(id == 0 || id == 1)
		userFlag &= ~USER_FLAGS[id];
}