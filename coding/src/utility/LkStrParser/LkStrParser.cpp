// LkStrParser.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "StdRsType.h"

#include "LkStrParser.h"

#include "RsTokenParser.h"
#include "RsEnumParser.h"

LPILkTokenParser LkStrParser_API CreateTokenParser(const char* inStr/*, bool inNeedValidate*/)
{
	LPILkTokenParser theParser=new RsTokenParser(inStr, true);
	return theParser;
} 

void LkStrParser_API DestroyTokenParser(LPILkTokenParser inParser)
{
	delete inParser;
}


LPILkEnumParser LkStrParser_API CreateEnumParser()
{
	LPILkEnumParser theParser=new RsEnumParser();
	return theParser;
}

void LkStrParser_API DestroyEnumParser(LPILkEnumParser inParser)
{
	delete inParser;
}

void LkStrParser_API getDeviceManagSetupStr(LK_DEVICE_MANAGER_CallerType inCallerType, char* ioBuffer, int inLen)
{
	sprintf_s(ioBuffer, inLen, "%s=%s", 
		LK_DEVICE_MANAGER_CallerType_STR[LK_DEVICE_MANAGER_CallerType_TypeName], 
		LK_DEVICE_MANAGER_CallerType_STR[inCallerType]			);	
}