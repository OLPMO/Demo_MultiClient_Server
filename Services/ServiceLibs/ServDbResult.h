#ifndef SERV_DB_RESULT_H
#define SERV_DB_RESULT_H
#include <map>
#include <string>
using namespace std;
class ServDbResult{
public:
	friend class ServDatabase;
	int nAffectedRows;
	
	ServDbResult(){
		nAffectedRows = -1;
		
	}
	ServDbResult(const ServDbResult & c){
// 将对象c中的数据成员值复制过来
		nAffectedRows = c.nAffectedRows;
		mapResult = c.mapResult;
	}
	~ServDbResult() {}
	const ServDbResult& operator=(const ServDbResult& _tmp)
	{
		nAffectedRows = _tmp.nAffectedRows;
		mapResult = _tmp.mapResult;
		return *this;
	}
	string Get(string strKey);
private:
	map<string, string> mapResult;
	void Set(string strKey, string strVal);
	
	void SetAffectedRows(int nRows);
	void SetResultRows(int nRows);
	//void SetFiledName(string strFiledName);
	
};

#endif