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
        // ��ʼ��ģ���ֵ䣬����qtssModName, qtssModDesc, qtssModVersion, qtssModRoles, qtssModPrefs, qtssModAttributes
        static void     Initialize();
    
        // CONSTRUCTOR / SETUP / DESTRUCTOR
        
        // ���ģ��Ӵ���װ�أ���Ҫ˵������·�������ģ��Ӵ���װ�أ���ģ���������ļ���������ģ��������ΪinName������
		// ��������Ƭ�ϣ���ʼ����ɫ����fRoleArray���Լ�fModuleState

        QTSSModule(char* inName, char* inPath = NULL);

        // �ú���������е�ģ�����ù��������ģ���disk���أ��㲻��Ҫ����main entrypoint��ϵͳ���Զ��Ӵ���Ƭ�ϻ�ȡ����������봫��main entrypoint.
		// ����ģ�飬���ģ�������ڵ㣬��������ڵ��÷ַ����
        //
        // ע��ú�����invoke���й���ģ���ɫ
        QTSS_Error  SetupModule(QTSS_CallbacksPtr inCallbacks, QTSS_MainEntryPointPtr inEntrypoint = NULL);

        // ������ڲ������stuff
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
		//  ������Ӧ��ɫ��fRoleArray[...]ΪTRUE�����ѽ�ɫ�����ֵ�
        QTSS_Error  AddRole(QTSS_Role inRole);
        
        // This returns true if this module is supposed to run in the specified role.
        Bool16  RunsInRole(RoleIndex inIndex) { Assert(inIndex < kNumRoles); return fRoleArray[inIndex]; }
        
        //�ú�����ȡ�¼��������Task::kUpdateEvent��������idleTime
		//                ���fRoleArray[kTimedIntervalRole]
		//                     ����ģ��״̬ -->  ����(void)this->CallDispatch(QTSS_Interval_Role, NULL) --> �����ǰһ��������globalLockRequested����...
		SInt64 Run();
        
        QTSS_ModuleState* GetModuleState() { return &fModuleState;}
        
    private:
		//�Ӵ����ļ�����ģ�飬�����ݳ�ģ�����, tobe
        QTSS_Error LoadFromDisk(QTSS_MainEntryPointPtr* outEntrypoint);

        OSQueueElem                 fQueueElem;				//ģ�����Ԫ��
        char*                       fPath;					//ģ��·��	
        OSCodeFragment*             fFragment;				//��ģ����м򵥷�װ���������ض�̬���ӿ⣬��ú�����ַ��Unload��̬���ӿ�
        QTSS_DispatchFuncPtr        fDispatchFunc;			//qtssModDesc ģ�鹦��������		//ģ����õĻص���������ͬ��ģ���в�ͬ�Ļص���������QTSSFileModuleDispatch
        Bool16                      fRoleArray[kNumRoles];	//qtssModRoles��ǰģ��ע������н�ɫ���б�	//ģ��Ľ�ɫ����??
        QTSSPrefs*                  fPrefs;					//qtssModPrefs����һ�����������Դ洢��ǰģ���Ԥ����Ϣ��
        QTSSDictionary*             fAttributes;			//qtssModAttributes ����һ������ģ�����������������洢����Ԥ����Ϣ֮��ı������ԡ�ģ����������飬��ʼ�����õ�ģ���Ӧ��QTSSDictionaryMap��
        OSMutex                     fAttributesMutex;		//���Է��ʵ�Mutex��

        static Bool16       sHasRTSPRequestModule;			//�Ƿ��Ѿ���QTSS_RTSPRequest_Role
        static Bool16       sHasOpenFileModule;				//............QTSS_OpenFile_Role
        static Bool16       sHasRTSPAuthenticateModule;		//............QTSS_RTSPAuthenticate_Role
    
        static QTSSAttrInfoDict::AttrInfo   sAttributes[];	//ģ����������飬���������Ե�Ԫ����

        		//qtssModVersion ģ��İ汾�ţ���ʽΪ0xMM.m.v.bbbb������MM = ��汾�� m = С�汾��v = �ǳ�С�İ汾 b = ���ࣨbuild���š�

        QTSS_ModuleState    fModuleState;					//
			//typedef struct
			//{
			//    QTSSModule* curModule;  // �ýṹ��ÿ���߳��н���������ĳһ��ɫinvokeģ����ǰ��
			//    QTSS_Role   curRole;    // ��ʱ����Ϣ�����ص�ʵ��
			//    Task*       curTask;    //
			//    Bool16      eventRequested;
			//    Bool16      globalLockRequested;  // ��ʾ�¼�ȫ�������������Ժ��������
			//    Bool16      isGlobalLocked;		//�Ƿ��Ѿ�����ȫ����
			//    SInt64      idleTime;   // If a module has requested idle time.
			//    
			//} QTSS_ModuleState, *QTSS_ModuleStatePtr;

};



#endif //__QTSSMODULE_H__
