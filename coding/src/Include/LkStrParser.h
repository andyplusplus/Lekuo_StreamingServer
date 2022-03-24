/*
LkStrParser.h
*/

#ifndef LkStrParser_H_INCLUDED
#define LkStrParser_H_INCLUDED

#include "ILkTokenParser.h"

#ifdef _I_LkStrParser
#	define LkStrParser_API  __declspec(dllexport) 
#else
#	define LkStrParser_API  __declspec(dllimport)
#endif


LPILkTokenParser	LkStrParser_API CreateTokenParser(const char* inStr);
void				LkStrParser_API DestroyTokenParser(LPILkTokenParser inParser);

LPILkEnumParser		LkStrParser_API CreateEnumParser();
void				LkStrParser_API DestroyEnumParser(LPILkEnumParser inParser);

void				LkStrParser_API getDeviceManagSetupStr(LK_DEVICE_MANAGER_CallerType inCallerType, char* ioBuffer, int inLen);


#endif //LkStrParser_H_INCLUDED