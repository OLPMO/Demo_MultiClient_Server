#include "ParsePackage.h"


bool ParsePackage::ReadPosition(void *pPackage,float* pFloatX,float* pFloatY,float* pFloatZ){
	
	if (ReadPackageType(pPackage) != m_PLAYER_POSITION) {
		return false;
	}
	float fX,fY,fZ;
	int nPackageType;
	float *pFloatPackage = (float *)SkipPackageType(pPackage);
	fX = pFloatPackage[0];
	fY = pFloatPackage[1];
	fZ = pFloatPackage[2];
	*pFloatX = fX;
	*pFloatY = fY;
	*pFloatZ  = fZ;
	return true;
}

bool ParsePackage::CheckPackage(void *pPackage){
	int *pIntPackage = (int *)pPackage;
	if( pIntPackage[0]>m_MIN_PACKAGE_NUM && pIntPackage[0]<m_MAX_PACKAGE_NUM ){
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
	memcpy(psState, SkipPackageType(pPackage),sizeof(PlayerState));
	return true;
}

void* ParsePackage::SkipPackageType(void *pPackage) {
	int *pIntPackage = (int *)pPackage;
	return (void* )(pIntPackage + 1);
}

bool ParsePackage::ReadSkill(void *pPackage, int* pIntSkill) {
	if (ReadPackageType(pPackage) != m_PLAYER_SKILL) {
		return false;
	}
	int *pSkill = (int *)SkipPackageType(pPackage);
	*pIntSkill = *pSkill;
	return true;
}

bool ParsePackage::ReadTime(void *pPackage, int* pIntTime) {
	if (ReadPackageType(pPackage) != m_TIME) {
		return false;
	}
	int *pTime = (int *)SkipPackageType(pPackage);
	*pIntTime = *pTime;
	return true;
}

void ParsePackage::WriteTime(void *pPackage, int nTime) {
	int *pInt = (int * )pPackage;
	pInt[0] = m_Time;
	pInt[1] = nTime;
}

bool ParsePackage::ReadPlayerId(void *pPackage, int* pPlayerId){
	if (ReadPackageType(pPackage) != m_PLAYER_ID) {
		return false;
	}
	int *pInt = (int *)SkipPackageType(pPackage);
	*pPlayerId = *pInt;
	return true;
}

void ParsePackage::WritePlayerId(void *pPackage, int pPlayerId) {
	int *pInt = (int * )pPackage;
	pInt[0] = m_PLAYER_ID;
	pInt[1] = pPlayerId;
}