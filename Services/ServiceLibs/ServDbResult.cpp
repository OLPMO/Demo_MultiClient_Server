#include "ServDbResult.h"
void ServDbResult::SetAffectedRows(int nRows){
	nAffectedRows = nRows;
}

void ServDbResult::SetResultRows(int nRows){
//	nResultRows = nRows;
}

void ServDbResult::Set(string strKey, string strVal) {
	mapResult[strKey] = strVal;
}
string ServDbResult::Get(string strKey) {
	return mapResult[strKey];
}