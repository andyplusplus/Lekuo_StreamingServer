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


/*
    File:       QTSSAdminModule.cpp

    Contains:   Implements Admin module

*/


#ifndef __Win32__
    #include <unistd.h>     /* for getopt() et al */
#endif

#include <time.h>
#include <stdio.h>      /* for qtss_printf */
#include <stdlib.h>     /* for getloadavg & other useful stuff */
#include "QTSSAdminModule.h"
#include "OSArrayObjectDeleter.h"
#include "StringParser.h"
#include "StrPtrLen.h"
#include "QTSSModuleUtils.h"
#include "OSHashTable.h"
#include "OSMutex.h"
#include "StrPtrLen.h"
#include "OSRef.h" 
#include "AdminElementNode.h"
#include "base64.h"
#include "OSMemory.h"
#include "md5.h"
#include "md5digest.h"

#if __MacOSX__
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#endif

#if __solaris__ || __linux__ || __sgi__ || __hpux__
	#include <crypt.h>
#endif

#define DEBUG_ADMIN_MODULE 0


//**************************************************
#define kAuthNameAndPasswordBuffSize 512
#define kPasswordBuffSize kAuthNameAndPasswordBuffSize/2

// STATIC DATA
//**************************************************
#if DEBUG_ADMIN_MODULE
static UInt32                   sRequestCount= 0;
#endif

static QTSS_Initialize_Params sQTSSparams;

//static char* sResponseHeader = "HTTP/1.0 200 OK\r\nServer: QTSS\r\nConnection: Close\r\nContent-Type: text/plain\r\n\r\n";
static char* sResponseHeader = "HTTP/1.0 200 OK";
static char* sUnauthorizedResponseHeader = "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"QTSS/modules/admin\"\r\nServer: QTSS\r\nConnection: Close\r\nContent-Type: text/plain\r\n\r\n";
static char* sPermissionDeniedHeader = "HTTP/1.1 403 Forbidden\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n";
static char* sHTMLBody =  "<HTML><BODY>\n<P><b>Your request was denied by the server.</b></P>\n</BODY></HTML>\r\n\r\n";
    

static char* sVersionHeader = NULL;
static char* sConnectionHeader = "Connection: Close";
static char* kDefaultHeader = "Server: QTSS";
static char* sContentType = "Content-Type: text/plain";
static char* sEOL = "\r\n";
static char* sEOM = "\r\n\r\n";
static char* sAuthRealm = "QTSS/modules/admin";
static char* sAuthResourceLocalPath = "/modules/admin/";

static QTSS_ServerObject        sServer = NULL;
static QTSS_ModuleObject        sModule = NULL;
static QTSS_ModulePrefsObject   sModulePrefs = NULL;
static QTSS_PrefsObject         sServerPrefs = NULL;
static AdminClass               *sAdminPtr = NULL;
static QueryURI                 *sQueryPtr = NULL;
static OSMutex*                 sAdminMutex = NULL;//admin module isn't reentrant
static UInt32                   sVersion=20030306;
static char *sDesc="Implements HTTP based Admin Protocol for accessing server attributes";
static char decodedLine[kAuthNameAndPasswordBuffSize] = { 0 };
static char codedLine[kAuthNameAndPasswordBuffSize] = { 0 }; 
static QTSS_TimeVal             sLastRequestTime = 0;
static UInt32                   sSessID = 0;

static StrPtrLen            sAuthRef("AuthRef");
#if __MacOSX__

static char*                sSecurityServerAuthKey = "com.apple.server.admin.streaming";
static AuthorizationItem    sRight = { sSecurityServerAuthKey, 0, NULL, 0 };
static AuthorizationRights  sRightSet = { 1, &sRight };
#endif


// ATTRIBUTES
//**************************************************
enum { kMaxRequestTimeIntervalMilli = 1000, kDefaultRequestTimeIntervalMilli = 50 };
static UInt32 sDefaultRequestTimeIntervalMilli = kDefaultRequestTimeIntervalMilli;
static UInt32 sRequestTimeIntervalMilli = kDefaultRequestTimeIntervalMilli;

static Bool16 sAuthenticationEnabled = true;
static Bool16 sDefaultAuthenticationEnabled = true;

static Bool16 sLocalLoopBackOnlyEnabled = true;
static Bool16 sDefaultLocalLoopBackOnlyEnabled = true;

static Bool16 sEnableRemoteAdmin = true;
static Bool16 sDefaultEnableRemoteAdmin = true;


static QTSS_AttributeID sIPAccessListID = qtssIllegalAttrID;
static char*            sIPAccessList = NULL;
static char*            sLocalLoopBackAddress = "127.0.0.*";

static char*            sAdministratorGroup = NULL;
static char*            sDefaultAdministratorGroup = "admin";

static Bool16           sFlushing = false;
static QTSS_AttributeID sFlushingID = qtssIllegalAttrID;
static char*            sFlushingName = "QTSSAdminModuleFlushingState";
static UInt32           sFlushingLen = sizeof(sFlushing);

static QTSS_AttributeID sAuthenticatedID = qtssIllegalAttrID;
static char*            sAuthenticatedName = "QTSSAdminModuleAuthenticatedState";

//**************************************************

static QTSS_Error QTSSAdminModuleDispatch(QTSS_Role inRole, QTSS_RoleParamPtr inParams);
static QTSS_Error Register(QTSS_Register_Params* inParams);
static QTSS_Error Initialize(QTSS_Initialize_Params* inParams);
static QTSS_Error FilterRequest(QTSS_Filter_Params* inParams);
static QTSS_Error RereadPrefs();
static QTSS_Error AuthorizeAdminRequest(QTSS_StandardRTSP_Params* inParams);
static Bool16 AcceptSession(QTSS_RTSPSessionObject inRTSPSession);


#if !DEBUG_ADMIN_MODULE
    #define APITests_DEBUG() 
    #define ShowQuery_DEBUG()
#else
void ShowQuery_DEBUG()
{
    qtss_printf("======REQUEST #%lu======\n",++sRequestCount);
    StrPtrLen*  aStr;
    aStr = sQueryPtr->GetURL();
    qtss_printf("URL="); PRINT_STR(aStr); 

    aStr = sQueryPtr->GetQuery();
    qtss_printf("Query="); PRINT_STR(aStr); 

    aStr = sQueryPtr->GetParameters();
    qtss_printf("Parameters="); PRINT_STR(aStr); 

    aStr = sQueryPtr->GetCommand();
    qtss_printf("Command="); PRINT_STR(aStr); 
    qtss_printf("CommandID=%ld \n",sQueryPtr->GetCommandID());
    aStr = sQueryPtr->GetValue();
    qtss_printf("Value="); PRINT_STR(aStr); 
    aStr = sQueryPtr->GetType();
    qtss_printf("Type="); PRINT_STR(aStr); 
    aStr = sQueryPtr->GetAccess();
    qtss_printf("Access="); PRINT_STR(aStr); 
}       

void APITests_DEBUG()
{
    if (0)
    {   qtss_printf("QTSSAdminModule start tests \n");
    
        if (0)
        {
            qtss_printf("admin called locked \n");
            const int ksleeptime = 15;
            qtss_printf("sleeping for %d seconds \n",ksleeptime);
            sleep(ksleeptime);
            qtss_printf("done sleeping \n");
            qtss_printf("QTSS_GlobalUnLock \n");
            (void) QTSS_GlobalUnLock();
            qtss_printf("again sleeping for %d seconds \n",ksleeptime);
            sleep(ksleeptime);
        }
    
        if (0)
        {
            qtss_printf(" GET VALUE PTR TEST \n");

            QTSS_Object *sessionsPtr = NULL;
            UInt32      paramLen = sizeof(sessionsPtr);
            UInt32      numValues = 0;
            QTSS_Error  err = 0;
            
            err = QTSS_GetNumValues (sServer, qtssSvrClientSessions, &numValues);//qtssSvrClientSessions ????????????????????????????QTSS_ClientSessionObject??????????????????????
            err = QTSS_GetValuePtr(sServer, qtssSvrClientSessions, 0, (void**)&sessionsPtr, &paramLen);//qtssSvrClientSessions ????????????????????????????QTSS_ClientSessionObject??????????????????????
            qtss_printf("Admin Module Num Sessions = %lu sessions[0] = %ld err = %ld paramLen =%lu\n", numValues, (SInt32) *sessionsPtr,err,paramLen);
    
            UInt32      numAttr = 0;
            if (sessionsPtr)
            {   err = QTSS_GetNumAttributes (*sessionsPtr, &numAttr);
                qtss_printf("Admin Module Num attributes = %lu sessions[0] = %ld  err = %ld\n", numAttr, (SInt32) *sessionsPtr,err);
        
                QTSS_Object theAttributeInfo;
                char nameBuff[128];
                UInt32 len = 127;
                for (UInt32 i = 0; i < numAttr; i++)
                {   err = QTSS_GetAttrInfoByIndex(*sessionsPtr, i, &theAttributeInfo);
                    nameBuff[0] = 0;len = 127;
                    err = QTSS_GetValue (theAttributeInfo, qtssAttrName,0, nameBuff,&len);
                    nameBuff[len] = 0;
                    qtss_printf("found %s \n",nameBuff);
                }
            }
        }
        
        if (0)
        {
            qtss_printf(" GET VALUE TEST \n");

            QTSS_Object sessions = NULL;
            UInt32      paramLen = sizeof(sessions);
            UInt32      numValues = 0;
            QTSS_Error  err = 0;
            
            err = QTSS_GetNumValues (sServer, qtssSvrClientSessions, &numValues);			//qtssSvrClientSessions ????????????????????????????QTSS_ClientSessionObject??????????????????????
            err = QTSS_GetValue(sServer, qtssSvrClientSessions, 0, (void*)&sessions, &paramLen);		//qtssSvrClientSessions ????????????????????????????QTSS_ClientSessionObject??????????????????????
            qtss_printf("Admin Module Num Sessions = %lu sessions[0] = %ld err = %ld paramLen = %lu\n", numValues, (SInt32) sessions,err, paramLen);
            
            if (sessions)
            {
                UInt32      numAttr = 0;
                err = QTSS_GetNumAttributes (sessions, &numAttr);
                qtss_printf("Admin Module Num attributes = %lu sessions[0] = %ld  err = %ld\n", numAttr,(SInt32) sessions,err);
                
                QTSS_Object theAttributeInfo;
                char nameBuff[128];
                UInt32 len = 127;
                for (UInt32 i = 0; i < numAttr; i++)
                {   err = QTSS_GetAttrInfoByIndex(sessions, i, &theAttributeInfo);
                    nameBuff[0] = 0;len = 127;
                    err = QTSS_GetValue (theAttributeInfo, qtssAttrName,0, nameBuff,&len);
                    nameBuff[len] = 0;
                    qtss_printf("found %s \n",nameBuff);
                }
            }
        }
        

        if (0)
        {
            qtss_printf("----------------- Start test ----------------- \n");
            qtss_printf(" GET indexed pref TEST \n");

            QTSS_Error  err = 0;
            
            UInt32      numAttr = 1;
            err = QTSS_GetNumAttributes (sModulePrefs, &numAttr);
            qtss_printf("Admin Module Num preference attributes = %lu err = %ld\n", numAttr, err);
                
            QTSS_Object theAttributeInfo;
            char valueBuff[512];
            char nameBuff[128];
            QTSS_AttributeID theID;
            UInt32 len = 127;
            UInt32 i = 0;
            qtss_printf("first pass over preferences\n");
            for ( i = 0; i < numAttr; i++)
            {   err = QTSS_GetAttrInfoByIndex(sModulePrefs, i, &theAttributeInfo);
                nameBuff[0] = 0;len = 127;
                err = QTSS_GetValue (theAttributeInfo, qtssAttrName,0, nameBuff,&len);
                nameBuff[len]=0;

                theID = qtssIllegalAttrID; len = sizeof(theID);
                err = QTSS_GetValue (theAttributeInfo, qtssAttrID,0, &theID,&len);
                qtss_printf("found preference=%s \n",nameBuff);
            }
            valueBuff[0] = 0;len = 512;
            err = QTSS_GetValue (sModulePrefs, theID,0, valueBuff,&len);valueBuff[len] = 0;
            qtss_printf("Admin Module QTSS_GetValue name = %s id = %ld value=%s err = %ld\n", nameBuff,theID, valueBuff, err);
            err = QTSS_SetValue (sModulePrefs,theID,0, valueBuff,len);
            qtss_printf("Admin Module QTSS_SetValue name = %s id = %ld value=%s err = %ld\n", nameBuff,theID, valueBuff, err);
            
            {   QTSS_ServiceID id;
                (void) QTSS_IDForService(QTSS_REREAD_PREFS_SERVICE, &id);           
                (void) QTSS_DoService(id, NULL);
            }

            valueBuff[0] = 0;len = 512;
            err = QTSS_GetValue (sModulePrefs, theID,0, valueBuff,&len);valueBuff[len] = 0;
            qtss_printf("Admin Module QTSS_GetValue name = %s id = %ld value=%s err = %ld\n", nameBuff,theID, valueBuff, err);
            err = QTSS_SetValue (sModulePrefs,theID,0, valueBuff,len);
            qtss_printf("Admin Module QTSS_SetValue name = %s id = %ld value=%s err = %ld\n", nameBuff,theID, valueBuff, err);
                
            qtss_printf("second pass over preferences\n");
            for ( i = 0; i < numAttr; i++)
            {   err = QTSS_GetAttrInfoByIndex(sModulePrefs, i, &theAttributeInfo);
                nameBuff[0] = 0;len = 127;
                err = QTSS_GetValue (theAttributeInfo, qtssAttrName,0, nameBuff,&len);
                nameBuff[len]=0;

                theID = qtssIllegalAttrID; len = sizeof(theID);
                err = QTSS_GetValue (theAttributeInfo, qtssAttrID,0, &theID,&len);
                qtss_printf("found preference=%s \n",nameBuff);
            }
            qtss_printf("----------------- Done test ----------------- \n");
        }
            
    }
}

#endif

inline void KeepSession(QTSS_RTSPRequestObject theRequest,Bool16 keep)
{
    (void)QTSS_SetValue(theRequest, qtssRTSPReqRespKeepAlive, 0, &keep, sizeof(keep));
}

// FUNCTION IMPLEMENTATIONS

QTSS_Error QTSSAdminModule_Main(void* inPrivateArgs)
{
    return _stublibrary_main(inPrivateArgs, QTSSAdminModuleDispatch);
}


QTSS_Error  QTSSAdminModuleDispatch(QTSS_Role inRole, QTSS_RoleParamPtr inParams)
{
    switch (inRole)
    {
        case QTSS_Register_Role:
            return Register(&inParams->regParams);
        case QTSS_Initialize_Role:
            return Initialize(&inParams->initParams);
        case QTSS_RTSPFilter_Role:
        {   if (!sEnableRemoteAdmin) 
                break;
            return FilterRequest(&inParams->rtspFilterParams);
        }
        case QTSS_RTSPAuthorize_Role:
                return AuthorizeAdminRequest(&inParams->rtspRequestParams);
        case QTSS_RereadPrefs_Role:
            return RereadPrefs();
    }
    return QTSS_NoErr;
}


QTSS_Error Register(QTSS_Register_Params* inParams)
{
    // Do role & attribute setup
    (void)QTSS_AddRole(QTSS_Initialize_Role);
    (void)QTSS_AddRole(QTSS_RTSPFilter_Role);
    (void)QTSS_AddRole(QTSS_RereadPrefs_Role);
    (void)QTSS_AddRole(QTSS_RTSPAuthorize_Role);
    
    (void)QTSS_AddStaticAttribute(qtssRTSPRequestObjectType, sFlushingName, NULL, qtssAttrDataTypeBool16);
    (void)QTSS_IDForAttr(qtssRTSPRequestObjectType, sFlushingName, &sFlushingID);

    (void)QTSS_AddStaticAttribute(qtssRTSPRequestObjectType, sAuthenticatedName, NULL, qtssAttrDataTypeBool16);
    (void)QTSS_IDForAttr(qtssRTSPRequestObjectType, sAuthenticatedName, &sAuthenticatedID);

    // Tell the server our name!
    static char* sModuleName = "QTSSAdminModule";
    ::strcpy(inParams->outModuleName, sModuleName);

    return QTSS_NoErr;
}

QTSS_Error RereadPrefs()
{   

    delete [] sVersionHeader;
    sVersionHeader = QTSSModuleUtils::GetStringAttribute(sServer, "qtssSvrRTSPServerHeader", kDefaultHeader);

    delete [] sIPAccessList;
    sIPAccessList = QTSSModuleUtils::GetStringAttribute(sModulePrefs, "IPAccessList", sLocalLoopBackAddress);
    sIPAccessListID = QTSSModuleUtils::GetAttrID(sModulePrefs, "IPAccessList");

    QTSSModuleUtils::GetAttribute(sModulePrefs, "Authenticate",     qtssAttrDataTypeBool16, &sAuthenticationEnabled, &sDefaultAuthenticationEnabled, sizeof(sAuthenticationEnabled));
    QTSSModuleUtils::GetAttribute(sModulePrefs, "LocalAccessOnly",  qtssAttrDataTypeBool16, &sLocalLoopBackOnlyEnabled, &sDefaultLocalLoopBackOnlyEnabled, sizeof(sLocalLoopBackOnlyEnabled));
    QTSSModuleUtils::GetAttribute(sModulePrefs, "RequestTimeIntervalMilli",     qtssAttrDataTypeUInt32, &sRequestTimeIntervalMilli, &sDefaultRequestTimeIntervalMilli, sizeof(sRequestTimeIntervalMilli));
    QTSSModuleUtils::GetAttribute(sModulePrefs, "enable_remote_admin",  qtssAttrDataTypeBool16, &sEnableRemoteAdmin, &sDefaultEnableRemoteAdmin, sizeof(sDefaultEnableRemoteAdmin));

    delete [] sAdministratorGroup;
    sAdministratorGroup = QTSSModuleUtils::GetStringAttribute(sModulePrefs, "AdministratorGroup", sDefaultAdministratorGroup);
    
    if (sRequestTimeIntervalMilli > kMaxRequestTimeIntervalMilli) 
    {   sRequestTimeIntervalMilli = kMaxRequestTimeIntervalMilli;
    }

    (void)QTSS_SetValue(sModule, qtssModDesc, 0, sDesc, strlen(sDesc)+1);   
    (void)QTSS_SetValue(sModule, qtssModVersion, 0, &sVersion, sizeof(sVersion));   

    return QTSS_NoErr;
}

QTSS_Error Initialize(QTSS_Initialize_Params* inParams)
{   
    sAdminMutex = NEW OSMutex();
    ElementNode_InitPtrArray();
    // Setup module utils
    QTSSModuleUtils::Initialize(inParams->inMessages, inParams->inServer, inParams->inErrorLogStream);

    sQTSSparams = *inParams;
    sServer = inParams->inServer;
    sModule = inParams->inModule;
    sModulePrefs = QTSSModuleUtils::GetModulePrefsObject(sModule);
    sServerPrefs = inParams->inPrefs;
    
    RereadPrefs();
    
    return QTSS_NoErr;
}

    
void ReportErr(QTSS_Filter_Params* inParams, UInt32 err)
{   
    StrPtrLen* urlPtr = sQueryPtr->GetURL();
    StrPtrLen* evalMessagePtr = sQueryPtr->GetEvalMsg();
    char temp[32];
    
    if (urlPtr && evalMessagePtr)   
    {   qtss_sprintf(temp,"(%lu)",err);
        (void)QTSS_Write(inParams->inRTSPRequest, "error:", strlen("error:"), NULL, 0);
        (void)QTSS_Write(inParams->inRTSPRequest, temp, strlen(temp), NULL, 0);
        if (sQueryPtr->VerboseParam())
        {   (void)QTSS_Write(inParams->inRTSPRequest, ";URL=", strlen(";URL="), NULL, 0);
            if (urlPtr) (void)QTSS_Write(inParams->inRTSPRequest, urlPtr->Ptr, urlPtr->Len, NULL, 0);
        }
        if (sQueryPtr->DebugParam())
        {
            (void)QTSS_Write(inParams->inRTSPRequest, ";", strlen(";"), NULL, 0);
            (void)QTSS_Write(inParams->inRTSPRequest, evalMessagePtr->Ptr, evalMessagePtr->Len, NULL, 0);               
        }
        (void)QTSS_Write(inParams->inRTSPRequest, "\r\n\r\n", 4, NULL, 0);
    }
}


inline Bool16 AcceptAddress(StrPtrLen *theAddressPtr)
{
    IPComponentStr ipComponentStr(theAddressPtr);
   
    Bool16 isLocalRequest = ipComponentStr.IsLocal();   
    if (sLocalLoopBackOnlyEnabled && isLocalRequest)
        return true;
     
     if (sLocalLoopBackOnlyEnabled && !isLocalRequest)
        return false;
    
    if  (QTSSModuleUtils::AddressInList(sModulePrefs, sIPAccessListID, theAddressPtr))
        return true;

    return false;
}

inline Bool16 IsAdminRequest(StringParser *theFullRequestPtr)
{
    Bool16 handleRequest = false;
    if (theFullRequestPtr != NULL) do
    {
        StrPtrLen   strPtr;
        theFullRequestPtr->ConsumeWord(&strPtr);
        if ( !strPtr.Equal(StrPtrLen("GET")) ) break;   //it's a "Get" request
        
        theFullRequestPtr->ConsumeWhitespace();
        if ( !theFullRequestPtr->Expect('/') ) break;   
                
        theFullRequestPtr->ConsumeWord(&strPtr);
        if ( strPtr.Len == 0 || !strPtr.Equal(StrPtrLen("modules") )    ) break;
        if (!theFullRequestPtr->Expect('/') ) break;
            
        theFullRequestPtr->ConsumeWord(&strPtr);
        if ( strPtr.Len == 0 || !strPtr.Equal(StrPtrLen("admin") ) ) break;
        handleRequest = true;
        
    } while (false);

    return handleRequest;
}

inline void ParseAuthNameAndPassword(StrPtrLen *codedStrPtr, StrPtrLen* namePtr, StrPtrLen* passwordPtr)
 {
    
    if (!codedStrPtr || (codedStrPtr->Len >= kAuthNameAndPasswordBuffSize) ) 
    {   return; 
    }
    
    StrPtrLen   codedLineStr;
    StrPtrLen   nameAndPassword;
    memset(decodedLine,0,kAuthNameAndPasswordBuffSize);
    memset(codedLine,0,kAuthNameAndPasswordBuffSize);
    
    memcpy (codedLine,codedStrPtr->Ptr,codedStrPtr->Len);
    codedLineStr.Set((char*) codedLine, codedStrPtr->Len);  
    (void) Base64decode(decodedLine, codedLineStr.Ptr);
    
    nameAndPassword.Set((char*) decodedLine, strlen(decodedLine));
    StringParser parsedNameAndPassword(&nameAndPassword);
    
    parsedNameAndPassword.ConsumeUntil(namePtr,':');            
    parsedNameAndPassword.ConsumeLength(NULL, 1);

    // password can have whitespace, so read until the end of the line, not just until whitespace
    parsedNameAndPassword.ConsumeUntil(passwordPtr, StringParser::sEOLMask);
    
    namePtr->Ptr[namePtr->Len]= 0;
    passwordPtr->Ptr[passwordPtr->Len]= 0;
    
    //qtss_printf("decoded nameAndPassword="); PRINT_STR(&nameAndPassword); 
    //qtss_printf("decoded name="); PRINT_STR(namePtr); 
    //qtss_printf("decoded password="); PRINT_STR(passwordPtr); 

    return;
};


inline Bool16 OSXAuthenticate(StrPtrLen *keyStrPtr)
{
#if __MacOSX__
//  Authorization: AuthRef QWxhZGRpbjpvcGVuIHNlc2FtZQ==
    Bool16 result = false;
    
    if (keyStrPtr == NULL || keyStrPtr->Len  == 0)
        return result;
    
    char *encodedKey = keyStrPtr->GetAsCString();
    OSCharArrayDeleter encodedKeyDeleter(encodedKey);

    char *decodedKey = NEW char[Base64decode_len(encodedKey) + 1];
    OSCharArrayDeleter decodedKeyDeleter(decodedKey);

    (void) Base64decode(decodedKey, encodedKey);

    AuthorizationExternalForm  *receivedExtFormPtr = (AuthorizationExternalForm  *) decodedKey;
    AuthorizationRef  receivedAuthorization;
    OSStatus status = AuthorizationCreateFromExternalForm(receivedExtFormPtr, &receivedAuthorization);

    if (status != errAuthorizationSuccess) 
        return result;
        
    status = AuthorizationCopyRights(receivedAuthorization, &sRightSet, kAuthorizationEmptyEnvironment, kAuthorizationFlagExtendRights , NULL);
    if (status == errAuthorizationSuccess)
    {
        result = true;
    }

    AuthorizationFree(receivedAuthorization, kAuthorizationFlagDestroyRights);

    return result;

#else

    return false;

#endif

}

inline Bool16 HasAuthentication(StringParser *theFullRequestPtr, StrPtrLen* namePtr, StrPtrLen* passwordPtr, StrPtrLen* outAuthTypePtr)
{
//  Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
    Bool16 hasAuthentication = false;
    StrPtrLen   strPtr; 
    StrPtrLen   authType;
    StrPtrLen   authString;
    while (theFullRequestPtr->GetDataRemaining() > 0)
    {
        theFullRequestPtr->ConsumeWhitespace();     
        theFullRequestPtr->ConsumeUntilWhitespace(&strPtr);
        if ( strPtr.Len == 0 || !strPtr.Equal(StrPtrLen("Authorization:")) ) 
            continue;   
                
        theFullRequestPtr->ConsumeWhitespace();     
        theFullRequestPtr->ConsumeUntilWhitespace(&authType);
        if ( authType.Len == 0 ) 
            continue;

        theFullRequestPtr->ConsumeWhitespace();                 
        theFullRequestPtr->ConsumeUntil(&authString, StringParser::sEOLMask);
        if ( authString.Len == 0 ) 
            continue;

        if (outAuthTypePtr != NULL)
            outAuthTypePtr->Set(authType.Ptr, authType.Len);

        if (authType.Equal(StrPtrLen("Basic") ) )
        {
            (void) ParseAuthNameAndPassword(&authString,namePtr, passwordPtr);
            if (namePtr->Len == 0) 
                continue;

            hasAuthentication = true;
            break;
        }
        else if (authType.Equal(sAuthRef) )
        {
            namePtr->Set(NULL,0);
            passwordPtr->Set(authString.Ptr, authString.Len);
            hasAuthentication = true;
            break;
        }
        
        
    };
    
    return hasAuthentication;
}

Bool16  Authenticate(QTSS_RTSPRequestObject request, StrPtrLen* namePtr, StrPtrLen* passwordPtr)
{
    Bool16 authenticated = true;
    
    char* authName = namePtr->GetAsCString();
    OSCharArrayDeleter authNameDeleter(authName);
    
    QTSS_ActionFlags authAction = qtssActionFlagsRead | qtssActionFlagsWrite;
    
    // authenticate callback to retrieve the password 
    QTSS_Error err = QTSS_Authenticate(authName, sAuthResourceLocalPath, sAuthResourceLocalPath, authAction, qtssAuthBasic, request);
    if (err != QTSS_NoErr) {
        qtss_printf("QTSSAdminModule::Authenticate: QTSS_Authenticate failed\n");
        return false; // Couldn't even call QTSS_Authenticate...abandon!
    }
    
    // Get the user profile object from the request object that was created in the authenticate callback
    QTSS_UserProfileObject theUserProfile = NULL;
    UInt32 len = sizeof(QTSS_UserProfileObject);
    err = QTSS_GetValue(request, qtssRTSPReqUserProfile, 0, (void*)&theUserProfile, &len);
    Assert(len == sizeof(QTSS_UserProfileObject));
    if (err != QTSS_NoErr)
        authenticated = false;

    if(err == QTSS_NoErr) {
        char* reqPassword = passwordPtr->GetAsCString();
        OSCharArrayDeleter reqPasswordDeleter(reqPassword);
        char* userPassword = NULL;  
        (void) QTSS_GetValueAsString(theUserProfile, qtssUserPassword, 0, &userPassword);
        OSCharArrayDeleter userPasswordDeleter(userPassword);
    
        if(userPassword == NULL) {
            authenticated = false;
        }
        else {
#ifdef __Win32__
            // The password is md5 encoded for win32
            char md5EncodeResult[120];
            MD5Encode(reqPassword, userPassword, md5EncodeResult, sizeof(md5EncodeResult));
            if(::strcmp(userPassword, md5EncodeResult) != 0)
                authenticated = false;
#else
            if(::strcmp(userPassword, (char*)crypt(reqPassword, userPassword)) != 0)
                authenticated = false;
#endif
        }
    }
    
    char* realm = NULL;
    Bool16 allowed = true;
    //authorize callback to check authorization
    // allocates memory for realm
	err = QTSS_Authorize(request, &realm, &allowed);
	// QTSS_Authorize allocates memory for the realm string
	// we don't use the realm returned by the callback, but instead 
	// use our own.
	// delete the memory allocated for realm because we don't need it!
	OSCharArrayDeleter realmDeleter(realm);
    
    if(err != QTSS_NoErr) {
        qtss_printf("QTSSAdminModule::Authenticate: QTSS_Authorize failed\n");
        return false; // Couldn't even call QTSS_Authorize...abandon!
    }
    
    if(authenticated && allowed)
        return true;
        
    return false;
}


QTSS_Error AuthorizeAdminRequest(QTSS_StandardRTSP_Params* inParams)
{
    Bool16 allowed = false;

    QTSS_RTSPRequestObject request = inParams->inRTSPRequest;
    
    // get the resource path
    // if the path does not match the admin path, don't handle the request
    char* resourcePath = QTSSModuleUtils::GetLocalPath_Copy(request);
    OSCharArrayDeleter resourcePathDeleter(resourcePath);
    if(strcmp(sAuthResourceLocalPath, resourcePath) != 0)
        return QTSS_NoErr;
    
    // get the type of request
    QTSS_ActionFlags action = QTSSModuleUtils::GetRequestActions(request);
    if(!(action & (qtssActionFlagsRead | qtssActionFlagsWrite)))
        return QTSS_NoErr;
        
    QTSS_UserProfileObject theUserProfile = QTSSModuleUtils::GetUserProfileObject(request);
    if (NULL == theUserProfile)
        return QTSS_RequestFailed;
           
    (void) QTSS_SetValue(request,qtssRTSPReqURLRealm, 0, sAuthRealm, ::strlen(sAuthRealm));
    
    // Authorize the user if the user belongs to the AdministratorGroup (this is an admin module pref)
    UInt32 numGroups = 0;
    char** groupsArray = QTSSModuleUtils::GetGroupsArray_Copy(theUserProfile, &numGroups);
    
    if ((groupsArray != NULL) && (numGroups != 0))
    {
        UInt32 index = 0;
        for (index = 0; index < numGroups; index++)
        {
            if(strcmp(sAdministratorGroup, groupsArray[index]) == 0)
            {
                allowed = true;
                break;
            }
        }
    
        // delete the memory allocated in QTSSModuleUtils::GetGroupsArray_Copy call 
        delete [] groupsArray;
    }
    
    if(!allowed)
    {
        if (QTSS_NoErr != QTSS_SetValue(request,qtssRTSPReqUserAllowed, 0, &allowed, sizeof(allowed)))
            return QTSS_RequestFailed; // Bail on the request. The Server will handle the error
    }
    
    return QTSS_NoErr;
}


Bool16 AcceptSession(QTSS_RTSPSessionObject inRTSPSession)
{   
    char remoteAddress[20] = {0};
    StrPtrLen theClientIPAddressStr(remoteAddress,sizeof(remoteAddress));
    QTSS_Error err = QTSS_GetValue(inRTSPSession, qtssRTSPSesRemoteAddrStr, 0, (void*)theClientIPAddressStr.Ptr, &theClientIPAddressStr.Len);
    if (err != QTSS_NoErr) return false;
    
    return AcceptAddress(&theClientIPAddressStr);    
}

Bool16 StillFlushing(QTSS_Filter_Params* inParams,Bool16 flushing)
{   

    QTSS_Error err = QTSS_NoErr;
    if (flushing) 
    {   
        err = QTSS_Flush(inParams->inRTSPRequest);
        //qtss_printf("Flushing session=%lu QTSS_Flush err =%ld\n",sSessID,err); 
    }
    if (err == QTSS_WouldBlock) // more to flush later
    {   
        sFlushing = true;
        (void) QTSS_SetValue(inParams->inRTSPRequest, sFlushingID, 0, (void*)&sFlushing, sFlushingLen);
        err = QTSS_RequestEvent(inParams->inRTSPRequest, QTSS_WriteableEvent);
        KeepSession(inParams->inRTSPRequest,true);
        //qtss_printf("Flushing session=%lu QTSS_RequestEvent err =%ld\n",sSessID,err);
    }
    else 
    {
        sFlushing = false;
        (void) QTSS_SetValue(inParams->inRTSPRequest, sFlushingID, 0, (void*)&sFlushing, sFlushingLen);
        KeepSession(inParams->inRTSPRequest,false);
    
        if (flushing) // we were flushing so reset the LastRequestTime
        {   
            sLastRequestTime = QTSS_Milliseconds();
            //qtss_printf("Done Flushing session=%lu\n",sSessID);
            return true;
        }
    }
    
    return sFlushing;
}

Bool16 IsAuthentic(QTSS_Filter_Params* inParams,StringParser *fullRequestPtr)
{
    Bool16 isAuthentic = false;

    if (!sAuthenticationEnabled) // no authentication
    {
        isAuthentic = true;
    }
    else // must authenticate
    {
        StrPtrLen theClientIPAddressStr;
        (void) QTSS_GetValuePtr(inParams->inRTSPSession, qtssRTSPSesRemoteAddrStr, 0, (void**)&theClientIPAddressStr.Ptr, &theClientIPAddressStr.Len);
        Bool16 isLocal =  IPComponentStr(&theClientIPAddressStr).IsLocal(); 
        
        StrPtrLen authenticateName;
        StrPtrLen authenticatePassword;
        StrPtrLen authType;
        Bool16 hasAuthentication = HasAuthentication(fullRequestPtr,&authenticateName,&authenticatePassword, &authType);               
        if (hasAuthentication) 
        {
            if (authType.Equal(sAuthRef))
            {    
                if (isLocal)
                    isAuthentic = OSXAuthenticate(&authenticatePassword);
            }
            else
                isAuthentic = Authenticate(inParams->inRTSPRequest, &authenticateName,&authenticatePassword);
       }
    }

    (void) QTSS_SetValue(inParams->inRTSPRequest, sAuthenticatedID, 0, (void*)&isAuthentic, sizeof(isAuthentic));

    return isAuthentic;
}

inline Bool16 InWaitInterval(QTSS_Filter_Params* inParams)
{
    QTSS_TimeVal nextExecuteTime = sLastRequestTime + sRequestTimeIntervalMilli;
    QTSS_TimeVal currentTime = QTSS_Milliseconds();
    SInt32 waitTime = 0;
    if (currentTime < nextExecuteTime)
    {   
        waitTime = (SInt32) (nextExecuteTime - currentTime) + 1;
        //qtss_printf("(currentTime < nextExecuteTime) sSessID = %lu waitTime =%ld currentTime = %qd nextExecute = %qd interval=%lu\n",sSessID, waitTime, currentTime, nextExecuteTime,sRequestTimeIntervalMilli);
        (void)QTSS_SetIdleTimer(waitTime);
        KeepSession(inParams->inRTSPRequest,true);
        
        //qtss_printf("-- call me again after %ld millisecs session=%lu \n",waitTime,sSessID);
        return true;
    }
    sLastRequestTime = QTSS_Milliseconds();
    //qtss_printf("handle sessID=%lu time=%qd \n",sSessID,currentTime);
    return false;
}

inline void GetQueryData(QTSS_RTSPRequestObject theRequest)
{
    sAdminPtr = NEW AdminClass();
    Assert(sAdminPtr != NULL);
    if (sAdminPtr == NULL) 
    {   //qtss_printf ("NEW AdminClass() failed!! \n");
        return;
    }
    if (sAdminPtr != NULL) 
    {
        sAdminPtr->Initialize(&sQTSSparams,sQueryPtr);  // Get theData
    }
}

inline void SendHeader(QTSS_StreamRef inStream)
{
    (void)QTSS_Write(inStream, sResponseHeader, ::strlen(sResponseHeader), NULL, 0);
    (void)QTSS_Write(inStream, sEOL, ::strlen(sEOL), NULL, 0);              
    (void)QTSS_Write(inStream, sVersionHeader, ::strlen(sVersionHeader), NULL, 0);      
    (void)QTSS_Write(inStream, sEOL, ::strlen(sEOL), NULL, 0);              
    (void)QTSS_Write(inStream, sConnectionHeader, ::strlen(sConnectionHeader), NULL, 0);        
    (void)QTSS_Write(inStream, sEOL, ::strlen(sEOL), NULL, 0);      
    (void)QTSS_Write(inStream, sContentType, ::strlen(sContentType), NULL, 0);      
    (void)QTSS_Write(inStream, sEOM, ::strlen(sEOM), NULL, 0);      
}

inline void SendResult(QTSS_StreamRef inStream)
{
    SendHeader(inStream);       
    if (sAdminPtr != NULL)
        sAdminPtr->RespondToQuery(inStream,sQueryPtr,sQueryPtr->GetRootID());
        
}

inline Bool16 GetRequestAuthenticatedState(QTSS_Filter_Params* inParams) 
{
    Bool16 result = false;
    UInt32 paramLen = sizeof(result);
    QTSS_Error err = QTSS_GetValue(inParams->inRTSPRequest, sAuthenticatedID, 0, (void*)&result, &paramLen);
    if(err != QTSS_NoErr)
    {
           paramLen = sizeof(result);
           result = false;
           err =QTSS_SetValue(inParams->inRTSPRequest, sAuthenticatedID, 0, (void*)&result, paramLen);
    }     
    return result;
}

inline Bool16 GetRequestFlushState(QTSS_Filter_Params* inParams)
{   Bool16 result = false;
    UInt32 paramLen = sizeof(result);
    QTSS_Error err = QTSS_GetValue(inParams->inRTSPRequest, sFlushingID, 0, (void*)&result, &paramLen);
    if (err != QTSS_NoErr)
    {   paramLen = sizeof(result);
        result = false;
        //qtss_printf("no flush val so set to false session=%lu err =%ld\n",sSessID, err);
        err =QTSS_SetValue(inParams->inRTSPRequest, sFlushingID, 0, (void*)&result, paramLen);
        //qtss_printf("QTSS_SetValue flush session=%lu err =%ld\n",sSessID, err);
    }
    return result;
}

QTSS_Error FilterRequest(QTSS_Filter_Params* inParams)
{
    if (NULL == inParams || NULL == inParams->inRTSPSession || NULL == inParams->inRTSPRequest)
    {   Assert(0);
        return QTSS_NoErr;
    }

    OSMutexLocker locker(sAdminMutex);
    //check to see if we should handle this request. Invokation is triggered
    //by a "GET /" request
    
    QTSS_Error err = QTSS_NoErr;
    QTSS_RTSPRequestObject theRequest = inParams->inRTSPRequest;

    UInt32 paramLen = sizeof(sSessID);
    err = QTSS_GetValue(inParams->inRTSPSession, qtssRTSPSesID, 0, (void*)&sSessID, &paramLen);     
    if (err != QTSS_NoErr) 
        return QTSS_NoErr;

    StrPtrLen theFullRequest;
    err = QTSS_GetValuePtr(theRequest, qtssRTSPReqFullRequest, 0, (void**)&theFullRequest.Ptr, &theFullRequest.Len);
    if (err != QTSS_NoErr) 
        return QTSS_NoErr;
        
    
    StringParser fullRequest(&theFullRequest);
        
    if ( !IsAdminRequest(&fullRequest) ) 
        return QTSS_NoErr;
        
    if ( !AcceptSession(inParams->inRTSPSession) )
    {   (void)QTSS_Write(inParams->inRTSPRequest, sPermissionDeniedHeader, ::strlen(sPermissionDeniedHeader), NULL, 0);     
        (void)QTSS_Write(inParams->inRTSPRequest, sHTMLBody, ::strlen(sHTMLBody), NULL, 0);
        KeepSession(theRequest,false);
        return QTSS_NoErr;
    }
    
    if(!GetRequestAuthenticatedState(inParams)) // must authenticate before handling
    {
        if(QTSS_IsGlobalLocked()) // must NOT be global locked
            return QTSS_RequestFailed;
            
        if (!IsAuthentic(inParams,&fullRequest)) 
        {   
            (void)QTSS_Write(inParams->inRTSPRequest, sUnauthorizedResponseHeader, ::strlen(sUnauthorizedResponseHeader), NULL, 0);     
            (void)QTSS_Write(inParams->inRTSPRequest, sHTMLBody, ::strlen(sHTMLBody), NULL, 0);
            KeepSession(theRequest,false);
            return QTSS_NoErr;
        }
    }
    
    if (GetRequestFlushState(inParams)) 
    {   StillFlushing(inParams,true);
        return QTSS_NoErr;
    }
        
    if (!QTSS_IsGlobalLocked())
    {       
        if (InWaitInterval(inParams)) 
            return QTSS_NoErr; 

        //qtss_printf("New Request Wait for GlobalLock session=%lu\n",sSessID);
        (void)QTSS_RequestGlobalLock();
        KeepSession(theRequest,true);
        return QTSS_NoErr; 
    }
    
    //qtss_printf("Handle request session=%lu\n",sSessID);
    APITests_DEBUG();
    
    if (sQueryPtr != NULL) 
    {   delete sQueryPtr;
        sQueryPtr = NULL;   
    }
    sQueryPtr = NEW QueryURI(&theFullRequest);
    if (sQueryPtr == NULL) return QTSS_NoErr;
    
    ShowQuery_DEBUG();
    
    if (sAdminPtr != NULL) 
    {   delete sAdminPtr;
        sAdminPtr = NULL;
    }
    UInt32 result = sQueryPtr->EvalQuery(NULL, NULL);
    if (result == 0) do
    {
        if( ElementNode_CountPtrs() > 0)
        {   ElementNode_ShowPtrs();
            Assert(0);
        }
            
        GetQueryData(theRequest);
        
        SendResult(theRequest); 
        delete sAdminPtr;
        sAdminPtr = NULL;
        
        if (sQueryPtr && !sQueryPtr->QueryHasReponse())
        {   UInt32 err = 404;
            (void) sQueryPtr->EvalQuery(&err,NULL);
            ReportErr(inParams, err);
            break;
        }

        if (sQueryPtr && sQueryPtr->QueryHasReponse())
        {   ReportErr(inParams, sQueryPtr->GetEvaluResult());
        }
        
        if (sQueryPtr->fIsPref && sQueryPtr->GetEvaluResult() == 0)
        {   QTSS_ServiceID id;
            (void) QTSS_IDForService(QTSS_REREAD_PREFS_SERVICE, &id);           
            (void) QTSS_DoService(id, NULL);
        }
    } while(false);
    else
    {
        SendHeader(theRequest);         
        ReportErr(inParams, sQueryPtr->GetEvaluResult());
    }
    
    if (sQueryPtr != NULL) 
    {   delete sQueryPtr;
        sQueryPtr = NULL;
    }
    
    (void) StillFlushing(inParams,true);
    return QTSS_NoErr;

}




