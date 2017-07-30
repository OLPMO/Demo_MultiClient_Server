#include "ServDbResult.h"
void ServDbResult::SetAffectedRows(int nRows){
	nAffectedRows = nRows;
}

void ServDbResult::SetResultRows(int nRows){
	nResultRows = nRows;
}

void ServDbResult::SetFiledName(string strFiledName){
	mapResult[strFiledName] = string("");
}