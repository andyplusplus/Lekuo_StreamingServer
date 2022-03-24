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
负责建立及关闭服务器, 也负责加载及初始化所有模块
    

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
        virtual ~QTSServer();		//设置主线程数据为模块状态, 以QTSS_Shutdown_Role角色触发已经注册模块

        //
        // Initialize
        //
        // This function starts the server. If it returns true, the server has started up sucessfully. If it returns false, a fatal error occurred
        // while attempting to start the server.
        //
        // This function *must* be called before the server creates any threads, because one of its actions is to change the server to the right UID / GID.
        // Threads will only inherit these if they are created afterwards.
		//1. 初始化回调函数		2. 相关的字典初始化		3. 服务器存根初始化		4. 创建全局对象fSocketPool, fRTPMap 
		//5. 加载错误日志模块	g)	设置默认IP地址、并记录启动时间		h)	创建侦听对象, 并开始侦听
        Bool16 Initialize(XMLPrefsParser* inPrefsSource, PrefsSource* inMessagesSource,
                            UInt16 inPortOverride, Bool16 createListeners);
        
        //
        // InitModules
        //
        // Initialize *does not* do much of the module initialization tasks. This
        // function may be called after the server has created threads, but the
        // server must not be in a state where it can do real work. In other words,
        // call this function right after calling Initialize.                   
        void InitModules(QTSS_ServerState inEndState);	//该函数加载并初始化所有模块
        
        //
        // StartTasks
        //
        // The server has certain global tasks that it runs for things like stats
        // updating and RTCP processing. This function must be called to start those
        // going, and it must be called after Initialize                
		//该函数主要实现
		//  1. 启动RTCPTask任务
		//  2. 启动RTPStatsUpdaterTask任务
		//  3. 启动RTSP事件通知机制, 通过::WSAAsyncSelect(req->er_handle, sMsgWindow, theMsg, theEvent)
        void StartTasks();


        //
        // RereadPrefsService
        //
        // This service is registered by the server (calling "RereadPreferences").
        // It rereads the preferences. Anyone can call this to reread the preferences,
        // and it may be called safely at any time, though it will fail with a
        // QTSS_OutOfState if the server isn't in the qtssRunningState.
		// 读取服务器以及所有模块的配置, 设置默认IP, 创建侦听者, 调用所有模块的QTSS_RereadPrefs_Role角色
        static QTSS_Error RereadPrefsService(QTSS_ServiceFunctionArgsPtr inArgs);

        //
        // CreateListeners
        //
        // This function may be called multiple times & at any time.
        // It updates the server's listeners to reflect what the preferences say.
        // Returns false if server couldn't listen on one or more of the ports, true otherwise
		//该函数包括三项参数
		//Bool16 startListeningNow		//是否开始侦听  
		//QTSServerPrefs* inPrefs		//服务器参数对象  
		//UInt16 inPortOverride			//是否重载端口，如果该值为0，则使用inPrefs中的端口  
		//
		//函数主要完成:
		//1.	从参数选项读取IP地址，把地址和端口保存在thePortTrackers[currentIndex]中。
		//2.	建新的TCPListenerSocket数组 
		//3.	接着断定正在侦听哪些端口，如果正在侦听的端口与当前的设置的侦听端口一致，侧侦听的端口保存到新建立的newListenerArray 数组，该过程主要通过两种嵌套方法实现(复杂度为m*n)，否则 。 
		//4.	然后建立新的侦听端口，判断是否需要建立侦听端口，如果需要则建立，然后初始化:
		//5.	删除不需要的已经在侦听的端口
		//6.	把端口加入到服务器属性中
		//总之, 到此为止, 已经完成了RTSP Listener的建立, 这个时候应该已经可以接收RTSP请求了. 端口为默认激活端口554, 7070, 8000, 以及8001。
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
        RTCPTask*           fRTCPTask;				//服务器所属性的RTCP对象, 从Task类继存下来
        RTPStatsUpdaterTask*fStatsTask;				//定义为RTPStatsUpdaterTask(), 定义了以后,在何处使用??
        SessionTimeoutTask  *fSessionTimeoutTask;	//??
        static char*        sPortPrefString;		//静态, 定义为"rtsp_port"
        static XMLPrefsParser* sPrefsSource;
        static PrefsSource* sMessagesSource;		
        
        //
        // Module loading & unloading routines
        
        static QTSS_Callbacks   sCallbacks;			//指向包含回调函数数组的数据结构
        
        // Sets up QTSS API callback routines
        void                    InitCallbacks();	//初始化回调函数, 使sCallbacks的中的数组元素指向QTSSCallbacks中的成员函数的函数地址
        
        // Loads compiled-in modules
        void                    LoadCompiledInModules();	//加载已经编译的模块

        // Loads modules from disk
        void                    LoadModules(QTSServerPrefs* inPrefs);	//从动态链接库加载模块
        void                    CreateModule(char* inModuleFolderPath, char* inModuleName);
				//该函数从模块文件创建模块
				//  1.	新建模块对象
				//  2.	设置模块回调函数, 并调用模块主函数
				//  3.	把模块加入服务器，包括调用模块主函数，建立参数字典，把模块加入模块数组、把并加入队列  
        



        // Adds a module to the module array
        Bool16                  AddModule(QTSSModule* inModule);
			//该函数的主要工作就是把模块加入到服务器数据结构中
				// 1.	设置主线程数据
				// 2.	调用inModule->CallDispatch(QTSS_Register_Role, &theRegParams)。按不同的模块及其角色调用相应的函数. 对于QTSSErrorLogModuleDispatch的QTSS_Register_Role来说，调用QTSSErrorLogModuleDispatch以后，再调用::Register(&inParamBlock->regParams)
				// 4.	为模块建立参数字典，允许模块的实例属性
				// 5.	把模块加入模块数组，通过this->SetValue(…)
				// 6.	把模块加入队列，队列由链表维护        

        // Call module init roles
        void                    DoInitRole();		// 该函数主要是以注册角色调用模块
        void                    SetupPublicHeader();	//设置的值，sPublicHeaderStr，比如DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, OPTIONS, ANNOUNCE, RECORD
        UInt32*                 GetRTSPIPAddrs(QTSServerPrefs* inPrefs, UInt32* outNumAddrsPtr);// 获当前系统设置中RTSP的IP地址数, 把获得的IP地址数目放在outNumAddrsPtr中, 返回IP地址数的指针
        UInt16*                 GetRTSPPorts(QTSServerPrefs* inPrefs, UInt32* outNumPortsPtr);
        
        // Build & destroy the optimized role / module arrays for invoking modules
        void                    BuildModuleRoleArrays();
				//由模块队列sModuleQueue构造theIter(类型为)
				//1.	调用DestroyModuleRoleArrays();//即删除QTSServer::sModuleArray[x]所指向的内容，见[6.1.7.1.5]
				//2.	然后对每种角色遍历(以下x为某种角色，y为该角色的模块下标(即第几个模块))。
				//a)	对模块队列遍历，统计该角色对应的模块数存入sNumModulesInRole[x]
				//b)	如果模块数>0，则重新对模块队列遍历，
				//i.	则所有该角色的模块存入sModuleArray[x][y++]
				//实际上这个过程是对模块按注册的角色进行分类的过程。

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
        QTSServer::RereadPrefsService(NULL);		//读取服务器以及所有模块的配置, 设置默认IP, 创建侦听者, 调用所有模块的QTSS_RereadPrefs_Role角色
        return -1;
    }
};


#endif // __QTSSERVER_H__


