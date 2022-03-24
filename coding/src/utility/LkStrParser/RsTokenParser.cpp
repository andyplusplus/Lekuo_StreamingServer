#include "stdafx.h"
#include "LeKuo.h"
#include "RsTokenParser.h"
#include "RsBoostType.h"
#include "boost/algorithm/string/trim.hpp"

#pragma region General Functions ======================================================

	RsTokenParser::RsTokenParser(const char* inStr, bool inNeedValidate) : m_Command(inStr), m_NeedValidate(inNeedValidate)
	{
		rsCharSeparator theSeparator(TOKEN_Sep);
		rsTokenizer theToken(m_Command, theSeparator);

		for(rsTokenizer_iter tok_iter=theToken.begin(); tok_iter!=theToken.end(); ++tok_iter)
		{
			rsString theStr = *tok_iter;
			boost::algorithm::trim(theStr);
			addTokenStr(theStr);
		}
	}

	RsTokenParser::~RsTokenParser(void)
	{
		//m_Map.clear();
	}


	LK_Error RsTokenParser::getToken(const char* inKey, char* outToken, int inTokenSize, int* outTokenSize)
	{
		LK_Error theError = LK_NoErr;

		rsString theKey = inKey;

		String_map_iter pos = m_Map.find(theKey);
		if (pos != m_Map.end())
		{
			strcpy_s(outToken, inTokenSize, pos->second.c_str());
			if(outTokenSize) *outTokenSize = strlen(outToken);
		}else
		{
			theError = LK_Token_not_exist;
			if(outTokenSize) *outTokenSize = 0;
		}

		return theError;
	}


	int RsTokenParser::isTokenExist(const char* inKey)
	{
		rsString theKey = inKey;

		String_map_iter pos = m_Map.find(theKey);

		return (pos != m_Map.end());
	}


	int RsTokenParser::getAllTokens(char* outTokenStr, int inTokenSize)
	{
		rsString theStr;
		for(String_map_iter pos = m_Map.begin(); pos != m_Map.end(); ++pos)
		{
			theStr=theStr+pos->first + "=" + pos->second + ";";
		}
		
		int theStrSize=theStr.size(); 
		assert(theStrSize<inTokenSize);

		strcpy_s(outTokenStr, inTokenSize, theStr.c_str());

		return theStrSize;
	}




	bool RsTokenParser::addTokenStr(rsString inStr)
	{
		rsCharSeparator theSeparator("=");
		rsTokenizer theToken(inStr, theSeparator);

		rsString theKeyToken[2];
		int i = 0;
		for(rsTokenizer_iter tok_iter=theToken.begin(); tok_iter!=theToken.end(); ++tok_iter, ++i)
		{
			if(i<2)
			{
				theKeyToken[i] = *tok_iter;
			}
		};
		addTokenPair(theKeyToken[0], theKeyToken[1]);
		return true;
	}

	bool RsTokenParser::addTokenPair(rsString inKey, rsString inToken)
	{
		m_Map.insert(make_pair(inKey, inToken));
		return true;
	}


#pragma endregion General Functions ======================================================


int RsTokenParser::isVirtualDevice()
{
	BOOL theResult;
	char theToken[NORM_CHAR_SIZE];
	LK_Error theError = getToken(LK_KEYWORD_STR[LK_KEYWORD_Device_isVirtual], theToken, NORM_CHAR_SIZE, &theResult);

	if ( (LK_NoErr == theError) && theResult)
		theResult = TRUE;	//isVirtual = 0, or !=0
	else 
		theResult = FALSE;

	return theResult;
}

int RsTokenParser::isRelayServer()
{
	BOOL theResult;
	char theToken[NORM_CHAR_SIZE];
	LK_Error theError = getToken(LK_KEYWORD_STR[LK_KEYWORD_RelayServer_ServerIP], theToken, NORM_CHAR_SIZE, &theResult);

	if ( (LK_NoErr == theError) && theResult)
		theResult = TRUE;	//the len of IP > 0
	else 
		theResult = FALSE;

	return theResult;
}


void RsTokenParser::DelServerToken()
{
	m_Map.erase(LK_KEYWORD_STR[LK_KEYWORD_RelayServer_ServerIP]);
}