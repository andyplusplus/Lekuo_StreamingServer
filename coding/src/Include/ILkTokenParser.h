/*
ILkTokenParser.h
*/
#if !defined(ILkTokenParser_H_INCLUDED)
#define ILkTokenParser_H_INCLUDED

#include "LeKuo.h"


class ILkTokenParser
{
public:

	virtual ~ILkTokenParser(void) {};

	//General Functions
	virtual LK_Error getToken(const char* inKey, char* outToken, int inTokenSize, int*	outTokenSize) = 0;
	virtual int isTokenExist(const char* inKey)=0;
	virtual int getAllTokens(char* outTokenStr, int inTokenSize) = 0;
	
	//Specific Functions
	virtual int isVirtualDevice()=0;
	virtual int isRelayServer()=0;
	virtual void DelServerToken()=0;
};

typedef ILkTokenParser*  LPILkTokenParser;


const int EN_Invalid_Item = -1;
class ILkEnumParser
{
public:
	//Note: the sizeof inDescArr must be LK_KEYWORD_MaxLen
	//return: return EN_Invalid_Item if inDesc is not found in inDescArr
	virtual int getEnumItem(int inEnumMin,int inEnumMax, const char inDescArr[][LK_KEYWORD_MaxLen], const char* inDesc) = 0;

	virtual ~ILkEnumParser(void) {};
};

typedef ILkEnumParser*  LPILkEnumParser;



#endif //(ILkTokenParser_H_INCLUDED)