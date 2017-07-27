#ifndef PARSE_PACKAGE_H
#define PARSE_PACKAGE_H

#include <iostream>
#include <cstring>
#include "PackageType.h"
#include "GameInfo.h"

class ParsePackage {
public:
	static int ReadPackageType(void *pPackage);
	static bool ReadPosition(void *pPackage,float* pFloatX,float* pFloatY,float* pFloatZ)
	static bool ReadState(void *pPackage, PlayerState* psState);
	static bool ReadSkill(void *pPackage, int* pIntSkill);
	static bool ReadTime(void *pPackage, int* pIntTime);
	static void WriteTime(void *pPackage, int nTime);
	static bool PReadPlayerId(void *pPackage, int* pPlayerId);
	static void WritePlayerId(void *pPackage, int pPlayerId);
private:
	static bool CheckPackage(void *pPackage);
	static void* SkipPackageType(void *pPackage);
};

#endif