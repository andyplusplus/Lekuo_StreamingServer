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
    File:       QTSServer.h

    Contains:   This object is responsible for bringing up & shutting down
                the server. It also loads & initializes all modules.
���������رշ�����, Ҳ������ؼ���ʼ������ģ��
    

*/

#ifndef __QTSSERVER_H__
#define __QTSSERVER_H__

#include "QTSServerInterface.h"
#include "Task.h"

class RTCPTask;
class RTSPListenerSocket;
class RTPSocketPool;

class QTSServer : public QTSServerInterface
{
    public:

        QTSServer() {}
        virtual ~QTSServer();		//�������߳�����Ϊģ��״̬, ��QTSS_Shutdown_Role��ɫ�����Ѿ�ע��ģ��

        //
        // Initialize
        //
        // This function starts the server. If it returns true, the server has started up sucessfully. If it returns false, a fatal error occurred
        // while attempting to start the server.
        //
        // This function *must* be called before the server creates any threads, because one of its actions is to change the server to the right UID / GID.
        // Threads will only inherit these if they are created afterwards.
		//1. ��ʼ���ص�����		2. ��ص��ֵ��ʼ��		3. �����������ʼ��		4. ����ȫ�ֶ���fSocketPool, fRTPMap 
		//5. ���ش�����־ģ��	g)	����Ĭ��IP��ַ������¼����ʱ��		h)	������������, ����ʼ����
        Bool16 Initialize(XMLPrefsParser* inPrefsSource, PrefsSource* inMessagesSource,
                            UInt16 inPortOverride, Bool16 createListeners);
        
        //
        // InitModules
        //
        // Initialize *does not* do much of the module initialization tasks. This
        // function may be called after the server has created threads, but the
        // server must not be in a state where it can do real work. In other words,
        // call this function right after calling Initialize.                   
        void InitModules(QTSS_ServerState inEndState);	//�ú������ز���ʼ������ģ��
        
        //
        // StartTasks
        //
        // The server has certain global tasks that it runs for things like stats
        // updating and RTCP processing. This function must be called to start those
        // going, and it must be called after Initialize                
		//�ú�����Ҫʵ��
		//  1. ����RTCPTask����
		//  2. ����RTPStatsUpdaterTask����
		//  3. ����RTSP�¼�֪ͨ����, ͨ��::WSAAsyncSelect(req->er_handle, sMsgWindow, theMsg, theEvent)
        void StartTasks();


        //
        // RereadPrefsService
        //
        // This service is registered by the server (calling "RereadPreferences").
        // It rereads the preferences. Anyone can call this to reread the preferences,
        // and it may be called safely at any time, though it will fail with a
        // QTSS_OutOfState if the server isn't in the qtssRunningState.
		// ��ȡ�������Լ�����ģ�������, ����Ĭ��IP, ����������, ��������ģ���QTSS_RereadPrefs_Role��ɫ
        static QTSS_Error RereadPrefsService(QTSS_ServiceFunctionArgsPtr inArgs);

        //
        // CreateListeners
        //
        // This function may be called multiple times & at any time.
        // It updates the server's listeners to reflect what the preferences say.
        // Returns false if server couldn't listen on one or more of the ports, true otherwise
		//�ú��������������
		//Bool16 startListeningNow		//�Ƿ�ʼ����  
		//QTSServerPrefs* inPrefs		//��������������  
		//UInt16 inPortOverride			//�Ƿ����ض˿ڣ������ֵΪ0����ʹ��inPrefs�еĶ˿�  
		//
		//������Ҫ���:
		//1.	�Ӳ���ѡ���ȡIP��ַ���ѵ�ַ�Ͷ˿ڱ�����thePortTrackers[currentIndex]�С�
		//2.	���µ�TCPListenerSocket���� 
		//3.	���Ŷ϶�����������Щ�˿ڣ�������������Ķ˿��뵱ǰ�����õ������˿�һ�£��������Ķ˿ڱ��浽�½�����newListenerArray ���飬�ù�����Ҫͨ������Ƕ�׷���ʵ��(���Ӷ�Ϊm*n)������ �� 
		//4.	Ȼ�����µ������˿ڣ��ж��Ƿ���Ҫ���������˿ڣ������Ҫ������Ȼ���ʼ��:
		//5.	ɾ������Ҫ���Ѿ��������Ķ˿�
		//6.	�Ѷ˿ڼ��뵽������������
		//��֮, ����Ϊֹ, �Ѿ������RTSP Listener�Ľ���, ���ʱ��Ӧ���Ѿ����Խ���RTSP������. �˿�ΪĬ�ϼ���˿�554, 7070, 8000, �Լ�8001��
        Bool16                  CreateListeners(Bool16 startListeningNow, QTSServerPrefs* inPrefs, UInt16 inPortOverride);

        //
        // SetDefaultIPAddr
        //
        // Sets the IP address related attributes of the server.
        Bool16                  SetDefaultIPAddr();
        
        Bool16                  SetupUDPSockets();
                
        Bool16                  SwitchPersonality();	//NOT USED

     private:
    
        //
        // GLOBAL TASKS
        RTCPTask*           fRTCPTask;				//�����������Ե�RTCP����, ��Task��̴�����
        RTPStatsUpdaterTask*fStatsTask;				//����ΪRTPStatsUpdaterTask(), �������Ժ�,�ںδ�ʹ��??
        SessionTimeoutTask  *fSessionTimeoutTask;	//??
        static char*        sPortPrefString;		//��̬, ����Ϊ"rtsp_port"
        static XMLPrefsParser* sPrefsSource;
        static PrefsSource* sMessagesSource;		
        
        //
        // Module loading & unloading routines
        
        static QTSS_Callbacks   sCallbacks;			//ָ������ص�������������ݽṹ
        
        // Sets up QTSS API callback routines
        void                    InitCallbacks();	//��ʼ���ص�����, ʹsCallbacks���е�����Ԫ��ָ��QTSSCallbacks�еĳ�Ա�����ĺ�����ַ
        
        // Loads compiled-in modules
        void                    LoadCompiledInModules();	//�����Ѿ������ģ��

        // Loads modules from disk
        void                    LoadModules(QTSServerPrefs* inPrefs);	//�Ӷ�̬���ӿ����ģ��
        void                    CreateModule(char* inModuleFolderPath, char* inModuleName);
				//�ú�����ģ���ļ�����ģ��
				//  1.	�½�ģ�����
				//  2.	����ģ��ص�����, ������ģ��������
				//  3.	��ģ��������������������ģ�������������������ֵ䣬��ģ�����ģ�����顢�Ѳ��������  
        



        // Adds a module to the module array
        Bool16                  AddModule(QTSSModule* inModule);
			//�ú�������Ҫ�������ǰ�ģ����뵽���������ݽṹ��
				// 1.	�������߳�����
				// 2.	����inModule->CallDispatch(QTSS_Register_Role, &theRegParams)������ͬ��ģ�鼰���ɫ������Ӧ�ĺ���. ����QTSSErrorLogModuleDispatch��QTSS_Register_Role��˵������QTSSErrorLogModuleDispatch�Ժ��ٵ���::Register(&inParamBlock->regParams)
				// 4.	Ϊģ�齨�������ֵ䣬����ģ���ʵ������
				// 5.	��ģ�����ģ�����飬ͨ��this->SetValue(��)
				// 6.	��ģ�������У�����������ά��        

        // Call module init roles
        void                    DoInitRole();		// �ú�����Ҫ����ע���ɫ����ģ��
        void                    SetupPublicHeader();	//���õ�ֵ��sPublicHeaderStr������DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, OPTIONS, ANNOUNCE, RECORD
        UInt32*                 GetRTSPIPAddrs(QTSServerPrefs* inPrefs, UInt32* outNumAddrsPtr);// ��ǰϵͳ������RTSP��IP��ַ��, �ѻ�õ�IP��ַ��Ŀ����outNumAddrsPtr��, ����IP��ַ����ָ��
        UInt16*                 GetRTSPPorts(QTSServerPrefs* inPrefs, UInt32* outNumPortsPtr);
        
        // Build & destroy the optimized role / module arrays for invoking modules
        void                    BuildModuleRoleArrays();
				//��ģ�����sModuleQueue����theIter(����Ϊ)
				//1.	����DestroyModuleRoleArrays();//��ɾ��QTSServer::sModuleArray[x]��ָ������ݣ���[6.1.7.1.5]
				//2.	Ȼ���ÿ�ֽ�ɫ����(����xΪĳ�ֽ�ɫ��yΪ�ý�ɫ��ģ���±�(���ڼ���ģ��))��
				//a)	��ģ����б�����ͳ�Ƹý�ɫ��Ӧ��ģ��������sNumModulesInRole[x]
				//b)	���ģ����>0�������¶�ģ����б�����
				//i.	�����иý�ɫ��ģ�����sModuleArray[x][y++]
				//ʵ������������Ƕ�ģ�鰴ע��Ľ�ɫ���з���Ĺ��̡�

        void                    DestroyModuleRoleArrays();

        		
#ifndef __Win32__
        static pid_t            sMainPid;
#endif

                
        friend class RTPSocketPool;
};

class RereadPrefsTask : public Task
{
public:
    virtual SInt64 Run()
    {
        QTSServer::RereadPrefsService(NULL);		//��ȡ�������Լ�����ģ�������, ����Ĭ��IP, ����������, ��������ģ���QTSS_RereadPrefs_Role��ɫ
        return -1;
    }
};


#endif // __QTSSERVER_H__


