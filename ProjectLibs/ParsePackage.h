#ifndef PARSE_PACKAGE_H
#define PARSE_PACKAGE_H

#include <iostream>
#include <string>
#include "PackageType.h"
#include "GameInfo.h"

#define TIME_SIZE sizeof(long)
#define USER_ID_SIZE sizeof(int)
#define PACKAGE_TYPE_SIZE sizeof(int)
using namespace std;

class ParsePackage {
public:
	static int ReadPackageType(void *pPackage);
	static bool ReadPosition(void *pPackage, float* pFloatX, float* pFloatY, float* pFloatZ);
	static bool ReadState(void *pPackage, PlayerState* psState);
	static bool ReadSkill(void *pPackage, int* pIntSkill);
	static void ReadTime(void *pPackage, long* pIntTime);
	static void WriteTime(void *pPackage, long nTime);
	static void ReadPlayerId(void *pPackage, int* pPlayerId);
	static void WritePlayerId(void *pPackage, int pPlayerId);
	static bool ReadLoginInfo(void *pPackage, string &refUserName, string &refUserPass);
	static bool ParsePackageHeader(void *pPackage, int *pPackageType, long *pTime, int *pUserId);
	static void ReadUserId(void *pPackage, int *pUserId);
private:
	static bool CheckPackage(void *pPackage);
	static bool CheckPackage(void *pPackage,int nType);
	static void* SkipPackageHeader(void *pPackage);
};

#endif