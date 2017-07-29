#include "ParsePackage.h"

using namespace std;
bool ParsePackage::ReadPosition(void *pPackage,float* pFloatX,float* pFloatY,float* pFloatZ){
	
	if (ReadPackageType(pPackage) != m_PLAYER_POSITION) {
		return false;
	}
	float fX,fY,fZ;
	int nPackageType;
	float *pFloatPackage = (float *)SkipPackageHeader(pPackage);
	fX = pFloatPackage[0];
	fY = pFloatPackage[1];
	fZ = pFloatPackage[2];
	*pFloatX = fX;
	*pFloatY = fY;
	*pFloatZ  = fZ;
	return true;
}

bool ParsePackage::CheckPackage(void *pPackage,int nType){
	int *pIntPackage = (int *)pPackage;
	if( pIntPackage[0] == nType){
		return true;
	}
	return false;
}

bool ParsePackage::CheckPackage(void *pPackage){
	int *pIntPackage = (int *)pPackage;
	if (pIntPackage[0] < m_MAX_PACKAGE_NUM && pIntPackage[0]>m_MIN_PACKAGE_NUM){
		return true;
	}
	return false;
}

int ParsePackage::ReadPackageType(void *pPackage){
	if(!CheckPackage(pPackage)){
		return -1;
	}
	int *pIntPackage = (int *)pPackage;
	return pIntPackage[0];
}
bool ParsePackage::ReadState(void *pPackage, PlayerState* psState) {
	if ( ReadPackageType(pPackage) != m_PLAYER_STATE ) {
		return false;
	}
	memcpy(psState, SkipPackageHeader(pPackage), sizeof(PlayerState));
	return true;
}

bool ParsePackage::ParsePackageHeader(void *pPackage, int *pPackageType, long *pTime, int *pUserId  ){
	if (!CheckPackage(pPackage)){
		return false;
	}
	//读取包头中的类型信息
	*pPackageType = ReadPackageType(pPackage);
	//
	
	ReadTime(pPackage, pTime);
	
	ReadUserId(pPackage, pUserId);
	return true;
}

void ParsePackage::ReadUserId(void *pPackage, int *pUserId){
	int *pUid = (int *)((char *)pPackage + PACKAGE_TYPE_SIZE+TIME_SIZE);
	*pUserId = *pUid;
}


void* ParsePackage::SkipPackageHeader(void *pPackage) {
	char *pCharPackage = (char *)pPackage;
	return (void*)(pCharPackage + PACKAGE_TYPE_SIZE + TIME_SIZE + USER_ID_SIZE);
}

bool ParsePackage::ReadSkill(void *pPackage, int* pIntSkill) {
	if (ReadPackageType(pPackage) != m_PLAYER_SKILL) {
		return false;
	}
	int *pSkill = (int *)SkipPackageHeader(pPackage);
	*pIntSkill = *pSkill;
	return true;
}

void ParsePackage::ReadTime(void *pPackage, long* pIntTime) {
	int *pTime = (int *)((char *)pPackage + PACKAGE_TYPE_SIZE);
	*pIntTime = *pTime;
}

void ParsePackage::WriteTime(void *pPackage, long nTime) {
	int *pInt = (int * )pPackage;
	//pInt[0] = m_TIME;
	pInt[1] = nTime;
}

void ParsePackage::ReadPlayerId(void *pPackage, int* pPlayerId){
	int *pPid = (int *)((char *)pPackage + PACKAGE_TYPE_SIZE+TIME_SIZE);
	*pPlayerId = *pPid;
}

void ParsePackage::WritePlayerId(void *pPackage, int pPlayerId) {
	int *pInt = (int * )pPackage;
	long *pLong = (long *)((int *)pInt+1);
	//pInt[0] = m_PLAYER_ID;
	pLong[1] = pPlayerId;
}

bool ParsePackage::ReadLoginInfo(void *pPackage, string &refUserName, string &refUserPass){
	if (ReadPackageType(pPackage) != m_LOGIN_INFO) {
		return false;
	}
	char *pStrUserName = (char *)SkipPackageHeader(pPackage);
	int nNameLength = 0;
	char *pTmp = pStrUserName;
	while (*pTmp){
		if ((pTmp - pStrUserName) > m_USER_NAME_MAX_LENGTH) return false;
		pTmp++;
	}
	refUserName.assign(pStrUserName,(pTmp-pStrUserName));
	char *pStrUserPass =(++pTmp);
	while (*pTmp){
		if ((pTmp - pStrUserPass) > m_USER_PASS_MAX_LENGTH) return false;
		cout << "sdsadfafd" << endl;
		pTmp++;
	}
	refUserPass.assign(pStrUserPass,(pTmp-pStrUserPass));
}