;// MessageDLL.mc

;// Header defining the Severities, Facilities and Languages 

MessageIdTypedef=DWORD

SeverityNames=
(
 Success=0x0:STATUS_SEVERITY_SUCCESS
 Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
 Warning=0x2:STATUS_SEVERITY_WARNING
 Error=0x3:STATUS_SEVERITY_ERROR
)

FacilityNames=
			(	Application=0x0:FACILITY_APPLICATION
				Libraries=0x1:FACILITY_LIBRARIES
				Network=0x2:FACILITY_NETWORK
				RtspFmt=0xE03
			)
 
LanguageNames=(English=0x409:MSG00409)
LanguageNames=(Chinese=0x0804:MSG00804)


; // The following are message definitions.

;//General Error   0xE00～0xE2F

MessageId=0xE00 	Severity=Error 	Facility=Libraries 	symbolicName=ERROR_RequestFailed
Language=English
Request Failed.
.
Language=Chinese
请求失败
.

MessageId=0xE01		Severity=Error	Facility=Libraries	SymbolicName=ERROR_Unimplemented
Language=English
Unimplemented.
.
Language=Chinese
功能未实现
.

MessageId=0xE02		Severity=Error	Facility=Libraries	SymbolicName=ERROR_RequestArrived 
Language=English
Request has arrived.
.
Language=Chinese
请求已到达
.

MessageId=0xE03		Severity=Error	Facility=Libraries	SymbolicName=ERROR_OutOfState
Language=English
Out Of State
.
Language=Chinese
状态异常
.


MessageId=0xE04		Severity=Error	Facility=Libraries	SymbolicName=ERROR_NotAModule
Language=English
Not a Module
.
Language=Chinese
不是模块
.


MessageId=0xE05		Severity=Error	Facility=Libraries	SymbolicName=ERROR_WrongVersion
Language=English
Wrong Version
.
Language=Chinese
错误的版本
.


MessageId=0xE06		Severity=Error	Facility=Libraries	SymbolicName=ERROR_IllegalService
Language=English
Illegal Service
.
Language=Chinese
不合法的服务
.

MessageId=0xE07		Severity=Error	Facility=Libraries	SymbolicName=ERROR_BadIndex
Language=English
Bad Index
.
Language=Chinese
错误的引用
.

MessageId=0xE08		Severity=Error	Facility=Libraries	SymbolicName=ERROR_ValueNotFound 
Language=English
Value Not Found
.
Language=Chinese
未发现值
.

MessageId=0xE09		Severity=Error	 Facility=Libraries	SymbolicName=ERROR_BadArgument
Language=English
Bad Argument
.
Language=Chinese
错误的参数
.

MessageId=0xE0A		Severity=Error	Facility=Libraries	SymbolicName=ERROR_ReadOnly
Language=English
Read Only
.
Language=Chinese
只读状态
.

MessageId=0xE0B		Severity=Error	Facility=Libraries	SymbolicName=ERROR_NotPreemptiveSafe
Language=English
Not Preemptive Safe
.
Language=Chinese
非优先安全
.

MessageId=0xE0C		Severity=Error	Facility=Libraries	SymbolicName=ERROR_NotEnoughSpace
Language=English
Not Enough Space
.
Language=Chinese
没有足够的空间
.

MessageId=0xE0D		Severity=Error	Facility=Libraries	SymbolicName=ERROR_WouldBlock	
Language=English
Would Block
.
Language=Chinese
阻塞状态
.

MessageId=0xE0E		Severity=Error	Facility=Libraries	SymbolicName=ERROR_NotConnected
Language=English
Not Connected
.
Language=Chinese
未连接
.

MessageId=0xE0F		Severity=Error	Facility=Libraries	SymbolicName=ERROR_FileNotFound
Language=English
File Not Found
.
Language=Chinese
文件未找到
.

MessageId=0xE10		Severity=Error	Facility=Libraries	SymbolicName=ERROR_FileNotFound
Language=English
No More Data
.
Language=Chinese
没有更多的数据
.

MessageId=0xE11		Severity=Error	Facility=Libraries	SymbolicName=ERROR_AttrDoesntExist
Language=English
Attribute Doesn't Exist
.
Language=Chinese
属性不存在
.

MessageId=0xE12		Severity=Error	Facility=Libraries	SymbolicName=ERROR_AttrNameExists
Language=English
Attribute Name Exists
.
Language=Chinese
属性名已存在
.

MessageId=0xE13		Severity=Error	Facility=Libraries	SymbolicName=ERROR_InstanceAttrsNotAllowed
Language=English
Instance Attributes Not Allowed
.
Language=Chinese
属性实例不被允许
.



;//TBD


;// VHikDVR Error  0xE30~0xE4F

MessageId=0xE30		Severity=ERROR	Facility=Libraries	SymbolicName=ERROR_DVRInitFail
Language=English
DVR Init Failed
.
Language=Chinese
初始化DVR失败
.

MessageId=0xE31		Severity=ERROR	Facility=Libraries	SymbolicName=ERROR_DVRLoginFail
Language=English
DVR Login Failed
.
Language=Chinese
登录DVR失败
.

MessageId=0xE32		Severity=ERROR	Facility=Libraries	SymbolicName=ERROR_DVRPlayFail
Language=English
DVR Play Failed
.
Language=Chinese
播放DVR失败
.

MessageId=0xE33		Severity=ERROR	Facility=Libraries	SymbolicName=ERROR_DVRStopFail
Language=English
DVR Stop Failed
.
Language=Chinese
停止DVR失败
.

MessageId=0xE34		Severity=ERROR	Facility=Libraries	SymbolicName=ERROR_DVRLogOutFail
Language=English
DVR LogOut Failed
.
Language=Chinese
登出DVR失败
.

MessageId=0xE35		Severity=ERROR	Facility=Libraries	SymbolicName=ERROR_DVRDestroyFail
Language=English
DVR Destroy Failed.
.
Language=Chinese
销毁DVR失败
.

;// Common Error 0xE50~0xEFF
MessageId=0xE50		Severity=ERROR	Facility=Application SymbolicName=ERROR_InvalidPath
Language=English
Invalid path
.
Language=Chinese
路径非法
.

;// RSGUI Error    0x600~0x61F

MessageId=0x600		Severity=Informational	Facility=Application	SymbolicName=MSG_DVRInitFail
Language=English
DVR Init Failed
.
Language=Chinese
初始化DVR失败
.

MessageId=0x601		Severity=Informational	Facility=Application	SymbolicName=MSG_DVRLoginFail
Language=English	DVR Login Failed	
.
Language=Chinese
登录DVR失败
.

MessageId=0x602		Severity=Informational	Facility=Application	SymbolicName=MSG_DVRPlayFail
Language=English
DVR Play Failed
.
Language=Chinese
播放DVR失败
.

MessageId=0x603		Severity=Informational	Facility=Application	SymbolicName=MSG_DVRStopFail
Language=English
DVR Stop Failed
.
Language=Chinese
停止DVR失败
.

MessageId=0x604		Severity=Informational	Facility=Application	SymbolicName=MSG_DVRLogOutFail
Language=English
DVR LogOut Failed
.
Language=Chinese
登出DVR失败
.

MessageId=0x605		Severity=Informational	Facility=Application	SymbolicName=MSG_DVRDestroyFail
Language=English
DVR Destroy Failed.
.
Language=Chinese
销毁DVR失败
.

;//TBD


;// ControlPanel Error  0x620~0x63F

MessageId=0x620		Severity=Informational	Facility=Application	SymbolicName=MSG_StreamServerCreateSucceed
Language=English
Create Stream Server Succeed.
.
Language=Chinese
流媒体服务创建成功
.

MessageId=0x621		Severity=Informational	Facility=Application	SymbolicName=MSG_StreamServerStartSucceed
Language=English
Start Stream Server Succeed.
.
Language=Chinese
流媒体服务启动成功
.

MessageId=0x622		Severity=Informational	Facility=Application	SymbolicName=MSG_StreamServerStopSucceed
Language=English
Stop Stream Server Succeed.
.
Language=Chinese
流媒体服务停止成功
.

MessageId=0x623		Severity=Informational	Facility=Application	SymbolicName=MSG_StreamServerCreateFail
Language=English
Create Stream Server Failed.
.
Language=Chinese
流媒体服务创建失败
.


MessageId=0x624		Severity=Informational	Facility=Application	SymbolicName=MSG_StreamServerStartFail
Language=English
Start Stream Server Failed.
.
Language=Chinese
流媒体服务启动失败
.

MessageId=0x625		Severity=Informational	Facility=Application	SymbolicName=MSG_StreamServerStopFail
Language=English
Stop Stream Server Failed.
.
Language=Chinese
流媒体服务停止失败
.

MessageId=0x626		Severity=Informational	Facility=Application	SymbolicName=MSG_ClientLaunchFail
Language=English
Client Can't launch.
.
Language=Chinese
客户端启动失败
.

MessageId=0x627		Severity=Informational	Facility=Application	SymbolicName=MSG_DiagnosisLaunchFail
Language=English
Diagnosis Can't launch.
.
Language=Chinese
诊断程序启动失败
.

;//TBD

MessageId=0x0000	Severity=Error	Facility=RtspFmt	SymbolicName=RTSP_ERROR_BASE
Language=English
This is Only a Reminder for RTSP_ERROR_BASE.
.
Language=Chinese
RTSP错误的基地址
.

MessageId=0x0010	Severity=Error	Facility=RtspFmt	SymbolicName=RTSP_ERROR_MissingServerIPAddress
Language=English
IP parameters is missing from the RTSP URL.
.
Language=Chinese
RTSP URL参数中缺少服务器IP地址.
.