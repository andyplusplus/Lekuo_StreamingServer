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

 */
/*
    File:       QTSSModule.cpp

    Contains:   Implements object defined in QTSSModule.h
                    
    
*/

#include <errno.h>

#include "QTSSModule.h"
#include "OSArrayObjectDeleter.h"
#include "OSMemory.h"
#include "StringParser.h"
#include "Socket.h"
#include "QTSServerInterface.h"


Bool16  QTSSModule::sHasRTSPRequestModule = false;
Bool16  QTSSModule::sHasOpenFileModule = false;
Bool16  QTSSModule::sHasRTSPAuthenticateModule = false;

QTSSAttrInfoDict::AttrInfo  QTSSModule::sAttributes[] =
{   /*fields:   fAttrName, fFuncPtr, fAttrDataType, fAttrPermission??????qtssModName, qtssModDesc, qtssModVersion, qtssModRoles, qtssModPrefs, qtssModAttributes, */
    /* 0 */ { "qtssModName",            NULL,                   qtssAttrDataTypeCharArray,  qtssAttrModeRead | qtssAttrModePreempSafe },
    /* 1 */ { "qtssModDesc",            NULL,                   qtssAttrDataTypeCharArray,  qtssAttrModeRead | qtssAttrModeWrite },
    /* 2 */ { "qtssModVersion",         NULL,                   qtssAttrDataTypeUInt32,     qtssAttrModeRead | qtssAttrModeWrite },
    /* 3 */ { "qtssModRoles",           NULL,                   qtssAttrDataTypeUInt32,     qtssAttrModeRead | qtssAttrModePreempSafe },
    /* 4 */ { "qtssModPrefs",           NULL,                   qtssAttrDataTypeQTSS_Object,qtssAttrModeRead | qtssAttrModePreempSafe  | qtssAttrModeInstanceAttrAllowed }, //ģ??ע???Ľ?ɫ
    /* 5 */ { "qtssModAttributes",      NULL,                   qtssAttrDataTypeQTSS_Object, qtssAttrModeRead | qtssAttrModePreempSafe | qtssAttrModeInstanceAttrAllowed }
};

void QTSSModule::Initialize()
{
    //Setup all the dictionary stuff
    for (UInt32 x = 0; x < qtssModNumParams; x++)
        QTSSDictionaryMap::GetMap(QTSSDictionaryMap::kModuleDictIndex)->
            SetAttribute(x, sAttributes[x].fAttrName, sAttributes[x].fFuncPtr,
                            sAttributes[x].fAttrDataType, sAttributes[x].fAttrPermission);
}

QTSSModule::QTSSModule(char* inName, char* inPath)
:   QTSSDictionary(QTSSDictionaryMap::GetMap(QTSSDictionaryMap::kModuleDictIndex)),
    fQueueElem(NULL),
    fPath(NULL),
    fFragment(NULL),
    fDispatchFunc(NULL),
    fPrefs(NULL),
    fAttributes(NULL)
{
    fQueueElem.SetEnclosingObject(this);
    this->SetTaskName("QTSSModule");
    if ((inPath != NULL) && (inPath[0] != '\0'))
    {
        // Create a code fragment if this module is being loaded from disk
        
        fFragment = NEW OSCodeFragment(inPath);
        fPath = NEW char[::strlen(inPath) + 2];
        ::strcpy(fPath, inPath);
    }
    
    fAttributes = NEW QTSSDictionary( NULL, &fAttributesMutex );	//?½??ֵ?????
    
    this->SetVal(qtssModPrefs,      &fPrefs,            sizeof(fPrefs));		//???????????º????????ֵ????ԣ?????????????????
    this->SetVal(qtssModAttributes, &fAttributes,       sizeof(fAttributes));	//SetVal( QTSS_AttributeID inAttrID, void* inValueBuffer, UInt32 inBufferLen)
    
    // ?????Ѿ??и????֣???copy??ģ?????????ڲ?????
    if (inName != NULL)
        this->SetValue(qtssModName, 0, inName, ::strlen(inName), QTSSDictionary::kDontObeyReadOnly);
                
    ::memset(fRoleArray, 0, sizeof(fRoleArray));
    ::memset(&fModuleState, 0, sizeof(fModuleState));

}

QTSS_Error  QTSSModule::SetupModule(QTSS_CallbacksPtr inCallbacks, QTSS_MainEntryPointPtr inEntrypoint)
{
    QTSS_Error theErr = QTSS_NoErr;
    
    // Load fragment from disk if necessary
    
    if ((fFragment != NULL) && (inEntrypoint == NULL))
        theErr = this->LoadFromDisk(&inEntrypoint);		//?Զ???OSCodeFragment??ȡQTSS_MainEntryPointPtr
    if (theErr != QTSS_NoErr)
        return theErr;
        
    // At this point, we must have an entrypoint
    if (inEntrypoint == NULL)
        return QTSS_NotAModule;
        
    // Invoke the private initialization routine
    QTSS_PrivateArgs thePrivateArgs;
    thePrivateArgs.inServerAPIVersion = QTSS_API_VERSION;
    thePrivateArgs.inCallbacks = inCallbacks;
    thePrivateArgs.outStubLibraryVersion = 0;
    thePrivateArgs.outDispatchFunction = NULL;
    //???ļ?ģ??Ϊ???????º????ȵ???QTSSFileModule_Main(void* inPrivateArgs)
	//QTSSFileModule_Main??һ??????_stublibrary_main(...)??????????Ҫ?ж?:
	//1. ????QTSS_Private.cpp??sCallbacks = inCallbacks??inCallbacks??QTSServer::sCallbacks
	//2. ????thePrivateArgs.outDispatchFunction = QTSSFileModuleDispatch
    theErr = (inEntrypoint)(&thePrivateArgs);	//????ģ?????????ڵ㣬????dll, ?????ڵ????ļ??????ɣ?ͨ??thePrivateArgs????ģ???ķַ?????
    if (theErr != QTSS_NoErr)
        return theErr;
        
    if (thePrivateArgs.outStubLibraryVersion > thePrivateArgs.inServerAPIVersion)
        return QTSS_WrongVersion;
    
    // Set the dispatch function so we'll be able to invoke this module later on
    
    fDispatchFunc = thePrivateArgs.outDispatchFunction;	//?ַ?????
    	
    //Log 
    char msgStr[2048];
    char* moduleName = NULL;
    (void)this->GetValueAsString (qtssModName, 0, &moduleName);
    qtss_snprintf(msgStr, sizeof(msgStr), "Module Loaded...%s [%s]", moduleName, (fFragment==NULL)?"static":"dynamic");
    delete [] moduleName;
    QTSServerInterface::LogError(qtssMessageVerbosity, msgStr);
	
     return QTSS_NoErr;
}

QTSS_Error QTSSModule::LoadFromDisk(QTSS_MainEntryPointPtr* outEntrypoint)
{
    static StrPtrLen sMainEntrypointName("_Main");
    
    Assert(outEntrypoint != NULL);
    
    // Modules only need to be initialized if they reside on disk. 
    if (fFragment == NULL)
        return QTSS_NoErr;
    
    if (!fFragment->IsValid())
        return QTSS_NotAModule;
        
    // fPath is actually a path. Extract the file name.
    
    StrPtrLen theFileName(fPath);
    StringParser thePathParser(&theFileName);
    
    while (thePathParser.GetThru(&theFileName, kPathDelimiterChar))
        ;
    Assert(theFileName.Len > 0);
    Assert(theFileName.Ptr != NULL);

#ifdef __Win32__
    StringParser theDLLTruncator(&theFileName);
    theDLLTruncator.ConsumeUntil(&theFileName, '.'); // strip off the ".DLL"
#endif

    /** 08/16/01 quellish **/

#if __MacOSX__
        StringParser theBundleTruncator(&theFileName);
        theBundleTruncator.ConsumeUntil(&theFileName, '.'); // strip off the ".bundle"
#endif

    // At this point, theFileName points to the file name. Make this the module name.
    this->SetValue(qtssModName, 0, theFileName.Ptr, theFileName.Len, QTSSDictionary::kDontObeyReadOnly);
    
    // 
    // The main entrypoint symbol name is the file name plus that _Main__ string up there.
    OSCharArrayDeleter theSymbolName(NEW char[theFileName.Len + sMainEntrypointName.Len + 2]);
    ::memcpy(theSymbolName, theFileName.Ptr, theFileName.Len);
    theSymbolName[theFileName.Len] = '\0';
    
    ::strcat(theSymbolName, sMainEntrypointName.Ptr);
    *outEntrypoint = (QTSS_MainEntryPointPtr)fFragment->GetSymbol(theSymbolName.GetObject());
    return QTSS_NoErr;
}

QTSS_Error  QTSSModule::AddRole(QTSS_Role inRole)
{
    // There can only be one QTSS_RTSPRequest processing module
    if ((inRole == QTSS_RTSPRequest_Role) && (sHasRTSPRequestModule))
        return QTSS_RequestFailed;
    if ((inRole == QTSS_OpenFilePreProcess_Role) && (sHasOpenFileModule))
        return QTSS_RequestFailed;
    // There can be only one module registered for QTSS_RTSPAuthenticate_Role 
    if ((inRole == QTSS_RTSPAuthenticate_Role) && (sHasRTSPAuthenticateModule))
        return QTSS_RequestFailed;

    switch (inRole)
    {
        // Map actual QTSS Role names to our private enum values. Turn on the proper one
        // in the role array
        case QTSS_Initialize_Role:          fRoleArray[kInitializeRole] = true;         break;
        case QTSS_Shutdown_Role:            fRoleArray[kShutdownRole] = true;           break;
        case QTSS_RTSPFilter_Role:          fRoleArray[kRTSPFilterRole] = true;         break;
        case QTSS_RTSPRoute_Role:           fRoleArray[kRTSPRouteRole] = true;          break;
        case QTSS_RTSPAuthenticate_Role:    fRoleArray[kRTSPAthnRole] = true;           break;
        case QTSS_RTSPAuthorize_Role:       fRoleArray[kRTSPAuthRole] = true;           break;
        case QTSS_RTSPPreProcessor_Role:    fRoleArray[kRTSPPreProcessorRole] = true;   break;
        case QTSS_RTSPRequest_Role:         fRoleArray[kRTSPRequestRole] = true;        break;
        case QTSS_RTSPPostProcessor_Role:   fRoleArray[kRTSPPostProcessorRole] = true;  break;
        case QTSS_RTSPSessionClosing_Role:  fRoleArray[kRTSPSessionClosingRole] = true; break;
        case QTSS_RTPSendPackets_Role:      fRoleArray[kRTPSendPacketsRole] = true;     break;
        case QTSS_ClientSessionClosing_Role:fRoleArray[kClientSessionClosingRole] = true;break;
        case QTSS_RTCPProcess_Role:         fRoleArray[kRTCPProcessRole] = true;        break;
        case QTSS_ErrorLog_Role:            fRoleArray[kErrorLogRole] = true;           break;
        case QTSS_RereadPrefs_Role:         fRoleArray[kRereadPrefsRole] = true;        break;
        case QTSS_OpenFile_Role:            fRoleArray[kOpenFileRole] = true;           break;
        case QTSS_OpenFilePreProcess_Role:  fRoleArray[kOpenFilePreProcessRole] = true; break;
        case QTSS_AdviseFile_Role:          fRoleArray[kAdviseFileRole] = true;         break;
        case QTSS_ReadFile_Role:            fRoleArray[kReadFileRole] = true;           break;
        case QTSS_CloseFile_Role:           fRoleArray[kCloseFileRole] = true;          break;
        case QTSS_RequestEventFile_Role:    fRoleArray[kRequestEventFileRole] = true;   break;
        case QTSS_RTSPIncomingData_Role:    fRoleArray[kRTSPIncomingDataRole] = true;   break;      
        case QTSS_StateChange_Role:         fRoleArray[kStateChangeRole] = true;        break;      
        case QTSS_Interval_Role:            fRoleArray[kTimedIntervalRole] = true;      break;      
        default:
            return QTSS_BadArgument;
    }
    
    if (inRole == QTSS_RTSPRequest_Role)
        sHasRTSPRequestModule = true;
    if (inRole == QTSS_OpenFile_Role)
        sHasOpenFileModule = true;
    if (inRole == QTSS_RTSPAuthenticate_Role)
        sHasRTSPAuthenticateModule = true;
        
    //
    // Add this role to the array of roles attribute
    QTSS_Error theErr = this->SetValue(qtssModRoles, this->GetNumValues(qtssModRoles), &inRole, sizeof(inRole), QTSSDictionary::kDontObeyReadOnly);
    Assert(theErr == QTSS_NoErr);
    return QTSS_NoErr;
}

SInt64 QTSSModule::Run()
{
    EventFlags events = this->GetEvents();
    //OSThreadDataSetter(void* inInitialValue, void* inFinalValue)????ʼ?߳?????ΪfModuleState???˳??ú???ʱ???ָ??߳?????NULL
 	OSThreadDataSetter theSetter(&fModuleState, NULL);		
    if (events & Task::kUpdateEvent)
    {   // force us to update to a new idle time ????idleTime
        return fModuleState.idleTime;// If the module has requested idle time...
    }
    
    if (fRoleArray[kTimedIntervalRole])
    {
        if (events & Task::kIdleEvent || fModuleState.globalLockRequested)
        {
            fModuleState.curModule = this;  // this structure is setup in each thread
            fModuleState.curRole = QTSS_Interval_Role;    // before invoking a module in a role. Sometimes
            fModuleState.eventRequested = false;
            fModuleState.curTask = this;
            if (fModuleState.globalLockRequested )
            {   fModuleState.globalLockRequested = false;
                fModuleState.isGlobalLocked = true;
            } 
            
            (void)this->CallDispatch(QTSS_Interval_Role, NULL);
            fModuleState.isGlobalLocked = false;
    
            if (fModuleState.globalLockRequested) // call this request back locked
                return this->CallLocked();    //????Task??ForceSameThread();fWriteLock = true;return 0;
            
            return fModuleState.idleTime; // If the module has requested idle time...
        }
 	}
	
	return 0;
  }  
