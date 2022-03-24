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
    File:       QTSS_Private.h

    Contains:   Implementation-specific structures and typedefs used by the implementation of QTSS API in the VideoRelayServer
	            DSS中，QTSS API所使用的结构和类型定义在本文件中实现
    
*/


#ifndef QTSS_PRIVATE_H
#define QTSS_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "OSHeaders.h"
#include "QTSS.h"

class QTSSModule;
class Task;

typedef QTSS_Error  (*QTSS_CallbackProcPtr)(...);
typedef void*       (*QTSS_CallbackPtrProcPtr)(...);

enum
{
    // 每个回调例程的索引枚举，回具体的回调例程地址放在数组内
    // 注: 新的callbacks只能增加在枚举尾部，并增加kLastCallback值，改变或插入索引顺序将bread其它release编译生成的动态模块
    
    kNewCallback                    = 0,
    kDeleteCallback                 = 1,
    kMillisecondsCallback           = 2,
    kConvertToUnixTimeCallback      = 3,
    kAddRoleCallback                = 4,
    kAddAttributeCallback           = 5,
    kIDForTagCallback               = 6,
    kGetAttributePtrByIDCallback    = 7,
    kGetAttributeByIDCallback       = 8,
    kSetAttributeByIDCallback       = 9,
    kWriteCallback                  = 10,
    kWriteVCallback                 = 11,
    kFlushCallback                  = 12,
    kAddServiceCallback             = 13,
    kIDForServiceCallback           = 14,
    kDoServiceCallback              = 15,
    kSendRTSPHeadersCallback        = 16,
    kAppendRTSPHeadersCallback      = 17,
    kSendStandardRTSPCallback       = 18,
    kAddRTPStreamCallback           = 19,
    kPlayCallback                   = 20,
    kPauseCallback                  = 21,
    kTeardownCallback               = 22,
    kRequestEventCallback           = 23,
    kSetIdleTimerCallback           = 24,
    kOpenFileObjectCallback         = 25,
    kCloseFileObjectCallback        = 26,
    kReadCallback                   = 27,
    kSeekCallback                   = 28,
    kAdviseCallback                 = 29,
    kGetNumValuesCallback           = 30,
    kGetNumAttributesCallback       = 31,
    kSignalStreamCallback           = 32,
    kCreateSocketStreamCallback     = 33,
    kDestroySocketStreamCallback    = 34,
    kAddStaticAttributeCallback     = 35,
    kAddInstanceAttributeCallback   = 36,
    kRemoveInstanceAttributeCallback= 37,
    kGetAttrInfoByIndexCallback     = 38,
    kGetAttrInfoByNameCallback      = 39,
    kGetAttrInfoByIDCallback        = 40,
    kGetValueAsStringCallback       = 41,
    kTypeToTypeStringCallback       = 42,
    kTypeStringToTypeCallback       = 43,
    kStringToValueCallback          = 44,       
    kValueToStringCallback          = 45,       
    kRemoveValueCallback            = 46,
    kRequestGlobalLockCallback      = 47, 
    kIsGlobalLockedCallback         = 48, 
    kUnlockGlobalLock               = 49, 
    kAuthenticateCallback           = 50,
    kAuthorizeCallback              = 51,   
    kRefreshTimeOutCallback         = 52,
    kCreateObjectValueCallback      = 53,
    kCreateObjectTypeCallback       = 54,
    kLockObjectCallback             = 55,
    kUnlockObjectCallback           = 56,
    kSetAttributePtrCallback        = 57,
    kSetIntervalRoleTimerCallback   = 58,
    kLockStdLibCallback             = 59,
    kUnlockStdLibCallback           = 60,
    kLastCallback                   = 61
};

typedef struct {
    // Callback function pointer array
    QTSS_CallbackProcPtr addr [kLastCallback];
} QTSS_Callbacks, *QTSS_CallbacksPtr;

//1. 在QTSSModule::SetupModule处使用了该数据结构，
typedef struct		
{
    UInt32                  inServerAPIVersion;	//QTSS API Version
    QTSS_CallbacksPtr       inCallbacks;		//1. 模块需调用的回调函数
    QTSS_StreamRef          inErrorLogStream;	//用于写入错误日志
    UInt32                  outStubLibraryVersion;	//模块适应用的Streaming Server版本
    QTSS_DispatchFuncPtr    outDispatchFunction;	//模块通过该值传回分发函数
    
} QTSS_PrivateArgs, *QTSS_PrivateArgsPtr;


typedef struct
{
	QTSSModule* curModule;  // 该结构在每个线程中建立，在以某一角色invoke模块以前，
	QTSS_Role   curRole;    // 有时该信息帮助回调实现
	Task*       curTask;    // 当前任务
	Bool16      eventRequested;         //已经请求事件
	Bool16      globalLockRequested;    // 表示请求事件全局锁		//
	Bool16      isGlobalLocked;			// 对全局锁的操作分为两步，1. 请求全局锁，2. 锁定，具体见QTSSModule::Run()，
	SInt64      idleTime;   // If a module has requested idle time.，模块可用此来请求idleTime
	
} QTSS_ModuleState, *QTSS_ModuleStatePtr;

QTSS_StreamRef  GetErrorLogStream();


#ifdef __cplusplus
}
#endif

#endif
