#ifndef DB_SERVICE_H
#define DB_SERVICE_H
#include "./ServiceLibs/ServDatabase.h"
class DbService{
public:
	static bool UserValidate(const ServString strName, const ServString strPassword, int * id);
private:

};

#endif