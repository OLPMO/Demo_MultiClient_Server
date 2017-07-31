#ifndef DB_SERVICE_H
#define DB_SERVICE_H
#include "./ServiceLibs/ServDatabase.h"
class DbService{
public:
	static bool UserValidate(const string strName, const string strPassword, int * id);
private:

};

#endif