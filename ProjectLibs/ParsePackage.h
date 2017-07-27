#ifndef PARSE_PACKAGE_H
#define PARSE_PACKAGE_H

#include <iostream>
#include <cstring>
#include "PackageType.h"
#include "GameInfo.h"

class ParsePackage {
public:
	static int ReadPackageType(void *pPackage);
	static bool ReadPosition(void *,float* ,float* ,float* );
	static bool ReadState(void *pPackage, PlayerState* psState);
	static bool ReadSkill(void *pPackage, int* pIntSkill);
private:
	static bool CheckPackage(void *pPackage);
	static void* SkipPackageType(void *pPackage);
};

#endif