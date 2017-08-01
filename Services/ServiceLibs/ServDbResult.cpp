#include "ServDbResult.h"
void ServDbResult::SetAffectedRows(int nRows){
	nAffectedRows = nRows;
}

void ServDbResult::SetResultRows(int nRows){
//	nResultRows = nRows;
}

void ServDbResult::Set(ServString strKey, ServString strVal) {
	mapResult[strKey] = strVal;
}
ServString ServDbResult::Get(ServString strKey) {
	return mapResult[strKey];
}