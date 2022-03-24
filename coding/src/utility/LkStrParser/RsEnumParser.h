/*
#include "RsTokenParser.h"
*/

#pragma once

#include "ILkTokenParser.h"
#include "RsBoostType.h"

class RsEnumParser : public ILkEnumParser
{
public:
	virtual ~RsEnumParser(void) {};
	virtual int getEnumItem(int inEnumMin,int inEnumMax, const char inDescArr[][LK_KEYWORD_MaxLen], const char* inDesc);
};

