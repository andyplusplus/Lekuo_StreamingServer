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

 /* File: QTSSModule.h
    Contains:   This object represents a single QTSS API compliant module.
                A module may either be compiled directly into the server,
                or loaded from a code fragment residing on the disk.
                
                Object does the loading and initialization of a module, and
                stores all per-module data.
                    

*/

#ifndef __QTSSMODULE_H__
#define __QTSSMODULE_H__

#include "QTSS.h"
#include "QTSS_Private.h"
#include "QTSSDictionary.h"
#include "Task.h"
#include "QTSSPrefs.h"

#include "OSCodeFragment.h"
#include "OSQueue.h"
#include "StrPtrLen.h"

#ifdef WUZENGDE
//#include "QTSSModuleExt.h"
#include "QTSSLogging.h"

#endif

class QTSSModule : public QTSSDictionary, public Task
{
    public:
    
        //
        // 初始化模块字典，包含qtssModName, qtssModDesc, qtssModVersion, qtssModRoles, qtssModPrefs, qtssModAttributes
        static void     Initialize();
    
        // CONSTRUCTOR / SETUP / DESTRUCTOR
        
        // 如果模块从磁盘装载，需要说明代码路径，如果模块从磁盘装载，则模块名就是文件名，否则模块名设置为inName的名字
		// 建立代码片断，初始化角色数组fRoleArray，以及fModuleState

        QTSSModule(char* inName, char* inPath = NULL);

        // 该函数完成所有的模块设置工作，如果模块从disk加载，你不需要传入main entrypoint，系统会自动从代码片断获取；否则，你必须传入main entrypoint.
		// 加载模块，获得模块的主入口点，并由主入口点获得分发规程
        //
        // 注意该函数不invoke所有公共模块角色
        QTSS_Error  SetupModule(QTSS_CallbacksPtr inCallbacks, QTSS_MainEntryPointPtr inEntrypoint = NULL);

        // 不清除内部分配的stuff
        virtual ~QTSSModule(){
			//_WZD_ADD_Start 2008.06.15
				if(fAttributes)
				{
					delete fAttributes;
					fAttributes = NULL;
				}
				if(fFragment)
				{
					delete fFragment;
					fFragment = NULL;
				}
				if(fPath)
				{
					delete fPath;
					fPath = NULL;
				}
			//_WZD_ADD_End
		}
        
        //
        // MODIFIERS
        void            SetPrefsDict(QTSSPrefs* inPrefs) { fPrefs = inPrefs; }
        void            SetAttributesDict(QTSSDictionary* inAttributes) { fAttributes = inAttributes; }
        //
        // ACCESSORS
        
        OSQueueElem*    GetQueueElem()  { return &fQueueElem; }
        Bool16          IsInitialized() { return fDispatchFunc != NULL; }
        QTSSPrefs*      GetPrefsDict()  { return fPrefs; }
        QTSSDictionary* GetAttributesDict() { return fAttributes; }
        OSMutex*        GetAttributesMutex() { return &fAttributesMutex; }
        
        // This calls into the module.
        QTSS_Error  CallDispatch(QTSS_Role inRole, QTSS_RoleParamPtr inParams)
            {  
				#ifdef WUZENGDE
					QTSSLogging::ModuleDispatchLog(this, inRole, inParams);
				#endif
				return (fDispatchFunc)(inRole, inParams);    
			}
        

        // These enums allow roles to be stored in a more optimized way
                
        enum
        {
            kInitializeRole =           0,
            kShutdownRole =             1,
            kRTSPFilterRole =           2,
            kRTSPRouteRole =            3,
            kRTSPAthnRole =             4,          
            kRTSPAuthRole =             5,
            kRTSPPreProcessorRole =     6,
            kRTSPRequestRole =          7,
            kRTSPPostProcessorRole =    8,
            kRTSPSessionClosingRole =   9,
            kRTPSendPacketsRole =       10,
            kClientSessionClosingRole = 11,
            kRTCPProcessRole =          12,
            kErrorLogRole =             13,
            kRereadPrefsRole =          14,
            kOpenFileRole =             15,
            kOpenFilePreProcessRole =   16,
            kAdviseFileRole =           17,
            kReadFileRole =             18,
            kCloseFileRole =            19,
            kRequestEventFileRole =     20,
            kRTSPIncomingDataRole =     21,
            kStateChangeRole =          22,
            kTimedIntervalRole =        23,
            
            kNumRoles =                 24
        };
        typedef UInt32 RoleIndex;
        
        // Call this to activate this module in the specified role.
		//  设置相应角色的fRoleArray[...]为TRUE，并把角色加入字典
        QTSS_Error  AddRole(QTSS_Role inRole);
        
        // This returns true if this module is supposed to run in the specified role.
        Bool16  RunsInRole(RoleIndex inIndex) { Assert(inIndex < kNumRoles); return fRoleArray[inIndex]; }
        
        //该函数读取事件，如果是Task::kUpdateEvent，则请求idleTime
		//                如果fRoleArray[kTimedIntervalRole]
		//                     设置模块状态 -->  调用(void)this->CallDispatch(QTSS_Interval_Role, NULL) --> 如果在前一调用中有globalLockRequested，则...
		SInt64 Run();
        
        QTSS_ModuleState* GetModuleState() { return &fModuleState;}
        
    private:
		//从磁盘文件加载模块，并传递出模块入口, tobe
        QTSS_Error LoadFromDisk(QTSS_MainEntryPointPtr* outEntrypoint);

        OSQueueElem                 fQueueElem;				//模块队列元素
        char*                       fPath;					//模块路径	
        OSCodeFragment*             fFragment;				//对模块进行简单封装，包括加载动态链接库，获得函数地址，Unload动态连接库
        QTSS_DispatchFuncPtr        fDispatchFunc;			//qtssModDesc 模块功能描述。		//模块调用的回调函数，不同的模块有不同的回调函数，如QTSSFileModuleDispatch
        Bool16                      fRoleArray[kNumRoles];	//qtssModRoles当前模块注册的所有角色的列表。	//模块的角色数组??
        QTSSPrefs*                  fPrefs;					//qtssModPrefs这是一个对象，其属性存储当前模块的预置信息。
        QTSSDictionary*             fAttributes;			//qtssModAttributes 这是一个对象，模块可以用这个对象来存储除了预置信息之外的本地属性。模块的属性数组，初始化设置到模块对应的QTSSDictionaryMap中
        OSMutex                     fAttributesMutex;		//属性访问的Mutex量

        static Bool16       sHasRTSPRequestModule;			//是否已经打开QTSS_RTSPRequest_Role
        static Bool16       sHasOpenFileModule;				//............QTSS_OpenFile_Role
        static Bool16       sHasRTSPAuthenticateModule;		//............QTSS_RTSPAuthenticate_Role
    
        static QTSSAttrInfoDict::AttrInfo   sAttributes[];	//模块的属性数组，描述的属性的元数据

        		//qtssModVersion 模块的版本号，格式为0xMM.m.v.bbbb，其中MM = 大版本， m = 小版本，v = 非常小的版本 b = 连编（build）号。

        QTSS_ModuleState    fModuleState;					//
			//typedef struct
			//{
			//    QTSSModule* curModule;  // 该结构在每个线程中建立，在以某一角色invoke模块以前，
			//    QTSS_Role   curRole;    // 有时该信息帮助回调实现
			//    Task*       curTask;    //
			//    Bool16      eventRequested;
			//    Bool16      globalLockRequested;  // 请示事件全局锁，先请求以后才能锁定
			//    Bool16      isGlobalLocked;		//是否已经锁定全局锁
			//    SInt64      idleTime;   // If a module has requested idle time.
			//    
			//} QTSS_ModuleState, *QTSS_ModuleStatePtr;

};



#endif //__QTSSMODULE_H__
