/*
#include "RsTokenParser.h"
*/

#pragma once

#include "ILkTokenParser.h"
#include "StdRsType.h"


class RsTokenParser : public ILkTokenParser
{
public:
	RsTokenParser(const char* inStr, bool inNeedValidate = TRUE);
	virtual ~RsTokenParser(void);

	//
	//General Functions
	virtual LK_Error getToken(const char* inKey, char* outToken, int inTokenSize, int* outTokenSize);
	virtual int		isTokenExist(const char* inKey);
	virtual int		getAllTokens(char* outTokenStr, int inTokenSize);
	//
	//Specific Functions
	virtual int isVirtualDevice();
	virtual int isRelayServer();
	virtual void DelServerToken();

private:
	bool addTokenStr(rsString inStr);
	bool addTokenPair(rsString inKey, rsString inToken);
	bool m_NeedValidate;
	rsString	m_Command;
	String_map	m_Map;
};

