// MessageDLL.mc
// Header defining the Severities, Facilities and Languages 
 // The following are message definitions.
//General Error   0xE00бл0xE2F
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_NETWORK                 0x2
#define FACILITY_LIBRARIES               0x1
#define FACILITY_APPLICATION             0x0


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3




//RTSP KeyWord Message Base
#define RTSP_KEYWORD_MESSAGE_BASE		((DWORD)0xE30)




//
// MessageId: ERROR_RequestFailed
//
// MessageText:
//
//  Request Failed.
//
#define ERROR_RequestFailed              ((DWORD)0xC0010E00L)

//
// MessageId: ERROR_Unimplemented
//
// MessageText:
//
//  Unimplemented.
//
#define ERROR_Unimplemented              ((DWORD)0xC0010E01L)

//
// MessageId: ERROR_RequestArrived
//
// MessageText:
//
//  Request has arrived.
//
#define ERROR_RequestArrived             ((DWORD)0xC0010E02L)

//
// MessageId: ERROR_OutOfState
//
// MessageText:
//
//  Out Of State
//
#define ERROR_OutOfState                 ((DWORD)0xC0010E03L)

//
// MessageId: ERROR_NotAModule
//
// MessageText:
//
//  Not a Module
//
#define ERROR_NotAModule                 ((DWORD)0xC0010E04L)

//
// MessageId: ERROR_WrongVersion
//
// MessageText:
//
//  Wrong Version
//
#define ERROR_WrongVersion               ((DWORD)0xC0010E05L)

//
// MessageId: ERROR_IllegalService
//
// MessageText:
//
//  Illegal Service
//
#define ERROR_IllegalService             ((DWORD)0xC0010E06L)

//
// MessageId: ERROR_BadIndex
//
// MessageText:
//
//  Bad Index
//
#define ERROR_BadIndex                   ((DWORD)0xC0010E07L)

//
// MessageId: ERROR_ValueNotFound
//
// MessageText:
//
//  Value Not Found
//
#define ERROR_ValueNotFound              ((DWORD)0xC0010E08L)

//
// MessageId: ERROR_BadArgument
//
// MessageText:
//
//  Bad Argument
//
#define ERROR_BadArgument                ((DWORD)0xC0010E09L)

//
// MessageId: ERROR_ReadOnly
//
// MessageText:
//
//  Read Only
//
#define ERROR_ReadOnly                   ((DWORD)0xC0010E0AL)

//
// MessageId: ERROR_NotPreemptiveSafe
//
// MessageText:
//
//  Not Preemptive Safe
//
#define ERROR_NotPreemptiveSafe          ((DWORD)0xC0010E0BL)

//
// MessageId: ERROR_NotEnoughSpace
//
// MessageText:
//
//  Not Enough Space
//
#define ERROR_NotEnoughSpace             ((DWORD)0xC0010E0CL)

//
// MessageId: ERROR_WouldBlock
//
// MessageText:
//
//  Would Block
//
#define ERROR_WouldBlock                 ((DWORD)0xC0010E0DL)

//
// MessageId: ERROR_NotConnected
//
// MessageText:
//
//  Not Connected
//
#define ERROR_NotConnected               ((DWORD)0xC0010E0EL)

//
// MessageId: ERROR_FileNotFound
//
// MessageText:
//
//  File Not Found
//
#define ERROR_FileNotFound               ((DWORD)0xC0010E0FL)


//
// MessageId: ERROR_AttrDoesntExist
//
// MessageText:
//
//  Attribute Doesn't Exist
//
#define ERROR_AttrDoesntExist            ((DWORD)0xC0010E11L)

//
// MessageId: ERROR_AttrNameExists
//
// MessageText:
//
//  Attribute Name Exists
//
#define ERROR_AttrNameExists             ((DWORD)0xC0010E12L)

//
// MessageId: ERROR_InstanceAttrsNotAllowed
//
// MessageText:
//
//  Instance Attributes Not Allowed
//
#define ERROR_InstanceAttrsNotAllowed    ((DWORD)0xC0010E13L)

//TBD
// VHikDVR Error  0xE30~0xE4F
//
// MessageId: ERROR_DVRInitFail
//
// MessageText:
//
//  DVR Init Failed
//
#define ERROR_DVRInitFail                ((DWORD)0xC0010E30L)

//
// MessageId: ERROR_DVRLoginFail
//
// MessageText:
//
//  DVR Login Failed
//
#define ERROR_DVRLoginFail               ((DWORD)0xC0010E31L)

//
// MessageId: ERROR_DVRPlayFail
//
// MessageText:
//
//  DVR Play Failed
//
#define ERROR_DVRPlayFail                ((DWORD)0xC0010E32L)

//
// MessageId: ERROR_DVRStopFail
//
// MessageText:
//
//  DVR Stop Failed
//
#define ERROR_DVRStopFail                ((DWORD)0xC0010E33L)

//
// MessageId: ERROR_DVRLogOutFail
//
// MessageText:
//
//  DVR LogOut Failed
//
#define ERROR_DVRLogOutFail              ((DWORD)0xC0010E34L)

//
// MessageId: ERROR_DVRDestroyFail
//
// MessageText:
//
//  DVR Destroy Failed.
//
#define ERROR_DVRDestroyFail             ((DWORD)0xC0010E35L)

// Common Error 0xE50~0xEFF
//
// MessageId: ERROR_InvalidPath
//
// MessageText:
//
//  Invalid path
//
#define ERROR_InvalidPath                ((DWORD)0xC0000E50L)

// RSGUI Error    0x600~0x61F
//
// MessageId: MSG_DVRInitFail
//
// MessageText:
//
//  DVR Init Failed
//
#define MSG_DVRInitFail                  ((DWORD)0x40000600L)

//
// MessageId: MSG_DVRLoginFail
//
// MessageText:
//
//  DVR Login Failed
//
#define MSG_DVRLoginFail                 ((DWORD)0x40000601L)

//
// MessageId: MSG_DVRPlayFail
//
// MessageText:
//
//  DVR Play Failed
//
#define MSG_DVRPlayFail                  ((DWORD)0x40000602L)

//
// MessageId: MSG_DVRStopFail
//
// MessageText:
//
//  DVR Stop Failed
//
#define MSG_DVRStopFail                  ((DWORD)0x40000603L)

//
// MessageId: MSG_DVRLogOutFail
//
// MessageText:
//
//  DVR LogOut Failed
//
#define MSG_DVRLogOutFail                ((DWORD)0x40000604L)

//
// MessageId: MSG_DVRDestroyFail
//
// MessageText:
//
//  DVR Destroy Failed.
//
#define MSG_DVRDestroyFail               ((DWORD)0x40000605L)

//TBD
// ControlPanel Error  0x620~0x63F
//
// MessageId: MSG_StreamServerCreateSucceed
//
// MessageText:
//
//  Create Stream Server Succeed.
//
#define MSG_StreamServerCreateSucceed    ((DWORD)0x40000620L)

//
// MessageId: MSG_StreamServerStartSucceed
//
// MessageText:
//
//  Start Stream Server Succeed.
//
#define MSG_StreamServerStartSucceed     ((DWORD)0x40000621L)

//
// MessageId: MSG_StreamServerStopSucceed
//
// MessageText:
//
//  Stop Stream Server Succeed.
//
#define MSG_StreamServerStopSucceed      ((DWORD)0x40000622L)

//
// MessageId: MSG_StreamServerCreateFail
//
// MessageText:
//
//  Create Stream Server Failed.
//
#define MSG_StreamServerCreateFail       ((DWORD)0x40000623L)

//
// MessageId: MSG_StreamServerStartFail
//
// MessageText:
//
//  Start Stream Server Failed.
//
#define MSG_StreamServerStartFail        ((DWORD)0x40000624L)

//
// MessageId: MSG_StreamServerStopFail
//
// MessageText:
//
//  Stop Stream Server Failed.
//
#define MSG_StreamServerStopFail         ((DWORD)0x40000625L)

//
// MessageId: MSG_ClientLaunchFail
//
// MessageText:
//
//  Client Can't launch.
//
#define MSG_ClientLaunchFail             ((DWORD)0x40000626L)

//
// MessageId: MSG_DiagnosisLaunchFail
//
// MessageText:
//
//  Diagnosis Can't launch.
//
#define MSG_DiagnosisLaunchFail          ((DWORD)0x40000627L)

//TBD
