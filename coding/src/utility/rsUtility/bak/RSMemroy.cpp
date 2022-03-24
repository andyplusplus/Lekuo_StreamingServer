// RSMemroy.cpp: implementation of the RSMemroy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LeKuo.h"
#include "RSMemroy.h"
//#include "StrPtrLen.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
;
RSMemroy::RSMemroy()
{

}

RSMemroy::~RSMemroy()
{

}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


RSCharMemroy::RSCharMemroy()
{

}

RSCharMemroy::~RSCharMemroy()
{

}

char* RSCharMemroy::newStrMemory()
{
	char* pStr = new char[MAX_CHAR_SIZE];
	memset(pStr, 0, MAX_CHAR_SIZE * sizeof(char));
	return pStr;
}

char* RSCharMemroy::newStrMemory_SetValue(const char* inStr)
{
	char* pStr = new char[MAX_CHAR_SIZE];
	memset(pStr, 0, MAX_CHAR_SIZE * sizeof(char));
	strcpy(pStr, inStr);
	return pStr;
}

void RSCharMemroy::zeroStrMemory(char* pStr)
{
	memset(pStr, 0, MAX_CHAR_SIZE);
}


void RSCharMemroy::releaseMemroy(char** pStr)
{
	if(*pStr)
	{
		delete [](*pStr);
		*pStr = NULL;
	}
};
