#ifndef SERV_DB_RESULT_H
#define SERV_DB_RESULT_H
#include "ServDatabase.h"
#include <map>
#include <string>
using namespace std;
class ServDbResult{
public:
	friend class ServDatabase;
	int nAffectedRows;
	int nResultRows;
	ServDbResult(){
		nAffectedRows = -1;
		nResultRows = -1;
	}
	~ServDbResult(){
	}
	
private:
	map<string, string> mapResult;
	void SetAffectedRows(int nRows);
	void SetResultRows(int nRows);
	void SetFiledName(string strFiledName);
	
};

#endif