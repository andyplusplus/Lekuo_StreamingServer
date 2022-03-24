#include "stdafx.h"
#include "StdRsType.h"
#include "RsBoostType.h"
#include "RsEnumParser.h"

//template <typename TENUM>

int RsEnumParser::getEnumItem(int inEnumMin,int inEnumMax, const char inDescArr[][LK_KEYWORD_MaxLen], const char* inDesc)
{
	rsString theStr1 = inDesc;
	boost::to_upper(theStr1);
	boost::trim(theStr1);
	for(int i = inEnumMin; i<inEnumMax; i++)
	{
		rsString theStr2=inDescArr[i];
		boost::to_upper(theStr2);
		boost::trim(theStr2);

		if(theStr1 == theStr2)
		{
			return i;
		}

	}
	return EN_Invalid_Item;
}
