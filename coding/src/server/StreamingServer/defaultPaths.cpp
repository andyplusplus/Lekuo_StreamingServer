/*
 *
 * APPLE_LICENSE_HEADER_START@
 *
 * Copyright c 1999-2008 Apple Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 the 'License'. You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */

//By __Andy__

//#include "StrPtrLen.h"
#include "defaultPaths.h"
#ifdef __Win32__

#include "IniReader2.h"
//#include "ResizeableStringFormatter.h"
#include "shlwapi.h"
#include <mbstring.h>

//2008.08.06, the parameter is configed to VideoRelay.ini previously, now hardcoded to the code, since it is rarly changed
//[ServerPathFileConfig]
//DEFAULTPATHS_ETC_DIR=.\\
//DEFAULTPATHS_SSM_DIR=serverModules\\
//DEFAULTPATHS_LOG_DIR=Logs\\
//DEFAULTPATHS_MOVIES_DIR=Movies\\
//DEFAULTPATHS_PID_FILE=
//DEFAULTPATHS_PID_DIR=
//DEFAULTPATHS_XMLFilePathName=streamingserver.xml
//DEFAULTPATHS_ConfigFileName=streamingserver.cfg
//sDefaultGroupsFilePath=qtgroups
//sDefaultUsersFilePath=qtusers


char* DEFAULTPATHS_ETC_DIR = "D:\\RSDLL_22\\"				;  //0,  "c:\Program Files\\lekuo\\StreamingServer\\"
char* DEFAULTPATHS_SSM_DIR = "D:\\RSDLL_22\\Modules\\"		;//1,  "c:\Program Files\\lekuo\\StreamingServer\\Modules\\"
char* DEFAULTPATHS_LOG_DIR = "D:\\RSDLL_22\\Logs\\"			;//2,  "c:\Program Files\\lekuo\\StreamingServer\\Logs\\"
char* DEFAULTPATHS_MOVIES_DIR = "D:\\RSDLL_22\\Movies\\"	;//3,  "c:\Program Files\\lekuo\\StreamingServer\\Movies\\"
char* DEFAULTPATHS_PID_FILE =""								;//4,  ""
char* DEFAULTPATHS_PID_DIR = ""								;//5,  ""
char* DEFAULTPATHS_XMLFilePathName = "D:\\RSDLL_22\\streamingserver.xml"	;//6, c:\\Program Files\\lekuo\\StreamingServer\\streamingserver.xml
char* DEFAULTPATHS_ConfigFileName = "D:\\RSDLL_22\\streamingserver.cfg"		;//7, c:\\Program Files\\lekuo\\StreamingServer\\streamingserver.cfg
extern char* sDefaultUsersFilePath;		// = "D:\\RSDLL_22\\qtusers";//8, DEFAULTPATHS_ETC_DIR "qtusers";
extern char* sDefaultGroupsFilePath;	// = "D:\\RSDLL_22\\qtgroups";//9, DEFAULTPATHS_ETC_DIR "qtgroups";
//extern char* strScheduleRulePath	 ;//10, "c:\\Program Files\\lekuo\\StreamingServer\\ScheduleRule.xml";

StrPtrLenDel   DEFAULTPATHS_ETC_DIR_Del		 ;	//0
StrPtrLenDel   DEFAULTPATHS_SSM_DIR_Del		 ;	//1
StrPtrLenDel   DEFAULTPATHS_LOG_DIR_Del		 ;	//2
StrPtrLenDel   DEFAULTPATHS_MOVIES_DIR_Del   ;	//3
StrPtrLenDel   DEFAULTPATHS_PID_FILE_Del     ;	//4
StrPtrLenDel   DEFAULTPATHS_PID_DIR_Del      ;	//5
StrPtrLenDel   DEFAULTPATHS_XMLFilePathName_Del	;//6
StrPtrLenDel   DEFAULTPATHS_ConfigFileName_Del	;//7
extern StrPtrLenDel sDefaultUsersFilePath_Del	;//8
extern StrPtrLenDel sDefaultGroupsFilePath_Del	;//9
//extern StrPtrLenDel strScheduleRulePath_Del	;//10

//由于以下两项需要引入 
//#include "QTSServerPrefs.h"，//#include "QTSS.h"，所以把这两荐的管理


int const DEFAULT_PATH_KeySize = 64;


struct DefaultPathInfo
{
    char                    fVal_Name[DEFAULT_PATH_KeySize];
	char**					fVal_PTR;
    StrPtrLenDel*			fVal_Del_PTR;
    char					fVal_Section[DEFAULT_PATH_KeySize];
    char					fVal_Key[DEFAULT_PATH_KeySize];
    char					fVal_DefaultValue[DEFAULT_PATH_KeySize];
};
		

static DefaultPathInfo sDefaultPathInfos[] = 
{   /*fields: fVal_Name,						fVal_PTR,	fVal_Del_PTR,	fVal_Section,					fVal_DefaultValue*/
    /* 0 */ { "DEFAULTPATHS_ETC_DIR"		,	NULL,	NULL,	"ServerPathFileConfig",	"DEFAULTPATHS_ETC_DIR"			,		".\\"},
    /* 1 */ { "DEFAULTPATHS_SSM_DIR"		,	NULL,	NULL,	"ServerPathFileConfig",	"DEFAULTPATHS_SSM_DIR"			,		"serverModules\\"},
    /* 2 */ { "DEFAULTPATHS_LOG_DIR"		,	NULL,	NULL,	"ServerPathFileConfig",	"DEFAULTPATHS_LOG_DIR"			,		"Logs\\"},
    /* 3 */ { "DEFAULTPATHS_MOVIES_DIR"		,	NULL,	NULL,	"ServerPathFileConfig",	"DEFAULTPATHS_MOVIES_DIR"		,		"Movies\\"},
    /* 4 */ { "DEFAULTPATHS_PID_FILE"		,	NULL,	NULL,	"ServerPathFileConfig",	"DEFAULTPATHS_PID_FILE"			,		""},
    /* 5 */ { "DEFAULTPATHS_PID_DIR"		,	NULL,	NULL,	"ServerPathFileConfig",	"DEFAULTPATHS_PID_DIR"			,		""},
    /* 6 */ { "DEFAULTPATHS_XMLFilePathName",	NULL,	NULL,	"ServerPathFileConfig",	"DEFAULTPATHS_XMLFilePathName"	,		"streamingserver.xml"},
    /* 7 */ { "DEFAULTPATHS_ConfigFileName"	,	NULL,	NULL,	"ServerPathFileConfig",	"DEFAULTPATHS_ConfigFileName"	,		"streamingserver.cfg"},
    /* 8 */ { "sDefaultUsersFilePath"		,	NULL,	NULL,	"ServerPathFileConfig",	"sDefaultUsersFilePath"			,		"qtgroups"},
    /* 9 */ { "sDefaultGroupsFilePath"		,	NULL,	NULL,	"ServerPathFileConfig",	"sDefaultGroupsFilePath"		,		"qtusers"},
    /* 10 */{ ""							,	NULL,	NULL,	"ServerPathFileConfig",	""								,		""},
};

const int sDefaultPathInfoNumbers = 10;


void DEFAULTPATHS_SetChar(char** psDest, char* psSrc);
void DEFAULTPATHS_AddChar(char** psDest, char* psSrc);
void DEFAULTPATHS_InitArray();
int  DEFAULTPATHS_IsAbsolutePathValue(char* psDest);

//
//
//用于初始化路径变量
//
//
void DEFAULTPATHS_Initialize()
{
	DEFAULTPATHS_InitArray();
																						//CIniReader2 theReader("VideoRelay.ini");
																						//for (int i = 0; i<sDefaultPathInfoNumbers; i++){
																						//	*(sDefaultPathInfos[i].fVal_PTR) = 
																						//		theReader.ReadString(	sDefaultPathInfos[i].fVal_Section,
																						//															sDefaultPathInfos[i].fVal_Key,
																						//															sDefaultPathInfos[i].fVal_DefaultValue);
																						//}
	for (int i = 0; i<sDefaultPathInfoNumbers; i++)
	{
		char* szResult = new char[INIREADER2_MAX_PATH];
		memset(szResult, 0x00, INIREADER2_MAX_PATH);
		strcpy_s(szResult, INIREADER2_MAX_PATH,sDefaultPathInfos[i].fVal_DefaultValue);

		*(sDefaultPathInfos[i].fVal_PTR) = szResult;
	}
	//
	//Set the Default Value
	//
	if ( NULL == *(sDefaultPathInfos[0].fVal_PTR)
		|| '.' == (*(sDefaultPathInfos[0].fVal_PTR))[0]
		)
	{
		if (*(sDefaultPathInfos[0].fVal_PTR))
		{
			delete *(sDefaultPathInfos[0].fVal_PTR);		
		}
		*(sDefaultPathInfos[0].fVal_PTR) = NULL;
		*(sDefaultPathInfos[0].fVal_PTR) = new char[INIREADER2_MAX_PATH];
		memset(*(sDefaultPathInfos[0].fVal_PTR), 0, INIREADER2_MAX_PATH);
		//
        ::GetModuleFileName(NULL, *(sDefaultPathInfos[0].fVal_PTR), INIREADER2_MAX_PATH);
        PathRemoveFileSpec(   *(sDefaultPathInfos[0].fVal_PTR)   );
		//
		int theSize = strlen( *(sDefaultPathInfos[0].fVal_PTR)  );
		//
		(*sDefaultPathInfos[0].fVal_PTR)[theSize] = '\\';
		(*sDefaultPathInfos[0].fVal_PTR)[theSize + 1] = 0;
		//(*sDefaultPathInfos[0].fVal_PTR)[theSize+1] = '\\';
		//(*sDefaultPathInfos[0].fVal_PTR)[theSize+2] = 0;

		//
		//把值Copy到*sDefaultPathInfos[1].fVal_PTR
		//

		if (*(sDefaultPathInfos[1].fVal_PTR))
		{
			delete *(sDefaultPathInfos[1].fVal_PTR);		
		}

		*(sDefaultPathInfos[1].fVal_PTR) = new char[INIREADER2_MAX_PATH];
		memcpy(
				(*sDefaultPathInfos[1].fVal_PTR), 
				(*sDefaultPathInfos[0].fVal_PTR),
				INIREADER2_MAX_PATH
			);
	}
	else
	{
		
	}
	//
	//Confirm the Value
	//
	for (int j = 2; j<sDefaultPathInfoNumbers; j++)
	{
		char** thePathValue= sDefaultPathInfos[j].fVal_PTR;
		if (*thePathValue)
		{
			if (!DEFAULTPATHS_IsAbsolutePathValue(  *thePathValue  )  )
			{
				char* theAbsolutePathValue = new char[INIREADER2_MAX_PATH];
				memcpy(theAbsolutePathValue, *(sDefaultPathInfos[0].fVal_PTR), INIREADER2_MAX_PATH);
				strcat(theAbsolutePathValue, *thePathValue);
				delete *thePathValue;
				*thePathValue = theAbsolutePathValue;
			}
		}
	}

	//
	//Set the deleter
	//
	for (int k = 0; k<sDefaultPathInfoNumbers; k++)
	{
		if (sDefaultPathInfos[k].fVal_Del_PTR && (*sDefaultPathInfos[k].fVal_PTR) )
		{
			sDefaultPathInfos[k].fVal_Del_PTR->Set(*sDefaultPathInfos[k].fVal_PTR);		
		}
	}

		//
		//
		//	char** strPrefsPidFile = QTSServerPrefs::GetPrefInfoDefaultValueAddr(qtssPrefsPidFile);
		//	DEFAULTPATHS_SetChar(strPrefsPidFile, DEFAULTPATHS_PID_DIR);
		//	DEFAULTPATHS_AddChar(strPrefsPidFile, PLATFORM_SERVER_BIN_NAME);
		//	DEFAULTPATHS_AddChar(strPrefsPidFile, ".pid");
}

//
//
//是否绝对路径
//
//
int isPathShouldChanged(const char* psPathName)
{
	if (
			strcmp( psPathName, "movie_folder") ==0				||
			strcmp( psPathName, "module_folder") ==0				||			
			strcmp( psPathName, "error_logfile_dir") ==0			||			
			strcmp( psPathName, "relay_prefs_file") ==0			||			
			strcmp( psPathName, "request_logfile_dir") ==0		||
			strcmp( psPathName, "modAccess_usersfilepath") ==0	||
			strcmp( psPathName, "modAccess_groupsfilepath") ==0
		)

		return 1;
	else
		return 0;

}

//
//
//按psPathName的不同，把*psPath的值改成绝对路径
//
//
int DEFAULTPATHS_GetAbsoluteDir(const char* psPathName, char** psPath)
{
	int bGetAbsoluteDir = 0;
	if (isPathShouldChanged(psPathName))
	{			
			if ( (*psPath)[0] == '.' &&
				 (*psPath)[1] == '\\' 
				 )
			{
				int len_psPath = strlen(*psPath);
				int len_defualt = strlen(DEFAULTPATHS_ETC_DIR);
				char *PNewPath = new char[ len_psPath + len_defualt];
				memset(PNewPath, 0, len_psPath + len_defualt);
				memcpy(PNewPath, DEFAULTPATHS_ETC_DIR, len_defualt);
				memcpy(PNewPath+len_defualt, ((*psPath)+2), len_psPath - 2 );

				delete [](*psPath);
				(*psPath) = PNewPath;

				bGetAbsoluteDir = 1;

			}else
			{
				;
			}		
	}

	return bGetAbsoluteDir;
}

//
//
//Set the psPathDest according to psPathSrc
//
//
int DEFAULTPATHS_GetRelativeDir(char* psPathName, char* psPathSrc, char* psPathDest)
{
	int bGetRelativeDir = 0;

	if (psPathSrc)
	{
		if (strlen(psPathSrc) > 2 )
		{
		
			if ( 0 == _mbsnbcmp((unsigned char*)psPathSrc, (unsigned char*)DEFAULTPATHS_ETC_DIR, strlen(DEFAULTPATHS_ETC_DIR)) )
			{
				memset(psPathDest, 0, INIREADER2_MAX_PATH);
				psPathDest[0] = '.';
				psPathDest[1] = '\\';
				memcpy(psPathDest + 2, psPathSrc+strlen(DEFAULTPATHS_ETC_DIR), strlen(psPathSrc) - strlen(DEFAULTPATHS_ETC_DIR));
				//
				bGetRelativeDir = 1;
			}
			else
			{

				bGetRelativeDir = 0;

			}
		
		}
	
	}
	return bGetRelativeDir;
}

//
//
//
//
//
void DEFAULTPATHS_SetChar(char** psDest, char* psSrc)
{
	int srcLen;
	if (*psDest == NULL)
	{
		*psDest = new char[INIREADER2_MAX_PATH];
		memset(*psDest, 0, INIREADER2_MAX_PATH-1);
	}else
	{
		
	}
	srcLen = strlen(psSrc);
	if (srcLen > 0 )
	{
		memcpy( *psDest, psSrc, strlen(psSrc));
	}
	(*psDest)[srcLen] = 0;

	return;
}

void DEFAULTPATHS_AddChar(char** psDest, char* psSrc)
{
	memcpy( (*psDest + strlen(*psDest) ), psSrc,strlen(psSrc));
	(*psDest)[strlen(*psDest)] = 0;
	return;
}

void DEFAULTPATHS_InitArray()
{
	sDefaultPathInfos[ 0  ].fVal_PTR		=	&  DEFAULTPATHS_ETC_DIR			;
	sDefaultPathInfos[ 0  ].fVal_Del_PTR	=	&  DEFAULTPATHS_ETC_DIR_Del		;
	//
	sDefaultPathInfos[ 1  ].fVal_PTR		=	&  DEFAULTPATHS_SSM_DIR			;
	sDefaultPathInfos[ 1  ].fVal_Del_PTR	=	&  DEFAULTPATHS_SSM_DIR_Del		;
	//
	sDefaultPathInfos[ 2  ].fVal_PTR		=	&  DEFAULTPATHS_LOG_DIR			;
	sDefaultPathInfos[ 2  ].fVal_Del_PTR	=	&  DEFAULTPATHS_LOG_DIR_Del		;
	//
	sDefaultPathInfos[ 3  ].fVal_PTR		=	&  DEFAULTPATHS_MOVIES_DIR		;
	sDefaultPathInfos[ 3  ].fVal_Del_PTR	=	&  DEFAULTPATHS_MOVIES_DIR_Del	;
	//
	sDefaultPathInfos[ 4  ].fVal_PTR		=	&  DEFAULTPATHS_PID_FILE		;
	sDefaultPathInfos[ 4  ].fVal_Del_PTR	=	&  DEFAULTPATHS_PID_FILE_Del	;
	//
	sDefaultPathInfos[ 5  ].fVal_PTR		=	&  DEFAULTPATHS_PID_DIR			;
	sDefaultPathInfos[ 5  ].fVal_Del_PTR	=	&  DEFAULTPATHS_PID_DIR_Del		;
	//
	sDefaultPathInfos[ 6  ].fVal_PTR		=	&  DEFAULTPATHS_XMLFilePathName	;
	sDefaultPathInfos[ 6  ].fVal_Del_PTR	=	&  DEFAULTPATHS_XMLFilePathName_Del	;
	//
	sDefaultPathInfos[ 7  ].fVal_PTR		=	&  DEFAULTPATHS_ConfigFileName	;
	sDefaultPathInfos[ 7  ].fVal_Del_PTR	=	&  DEFAULTPATHS_ConfigFileName_Del	;
	//
	sDefaultPathInfos[ 8  ].fVal_PTR		=	&  sDefaultUsersFilePath		;
	sDefaultPathInfos[ 8  ].fVal_Del_PTR	=	&  sDefaultUsersFilePath_Del	;
	//
	sDefaultPathInfos[ 9  ].fVal_PTR		=	&  sDefaultGroupsFilePath			;
	sDefaultPathInfos[ 9  ].fVal_Del_PTR	=	&  sDefaultGroupsFilePath_Del		;
//	//
//	sDefaultPathInfos[ 10 ].fVal_PTR		=	&  strScheduleRulePath			;
//	sDefaultPathInfos[ 10 ].fVal_Del_PTR	=	&  strScheduleRulePath_Del		;
	//

}

int DEFAULTPATHS_IsAbsolutePathValue(char* psDest)
{
	if (strchr(psDest, ':'))
		return 1;

	return 0;
}

#elif __MacOSX__

#else

#endif
