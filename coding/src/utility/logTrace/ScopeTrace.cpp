#pragma warning( disable : 4996)

#include "stdafx.h"
#include <Windows.h>
#include <strstream>
#include "scopetrace.h"
#include "TraceoutMan.h"

const BYTE INST_RELATIVE_CALL = 0xE8;
const BYTE INST_RELATIVE_CALL_LENGTH = 5;

//确认是否正常退出
inline static BOOL IsNormalCall(PBYTE callAddr, PBYTE entryAddr)
{
	if( callAddr[0] == INST_RELATIVE_CALL ) // verify if relative call
	{
		// verify call address
		// call/jump Addr + call/jump Offset + 5 = ret Addr + call/jump Offset = target function entry
		int callOffset = *(int*)(callAddr+1);
		PBYTE calcEntryAddr = callAddr+callOffset+INST_RELATIVE_CALL_LENGTH;
		return calcEntryAddr == entryAddr;
	}
	return FALSE;
}



#if _MSC_VER < 1300
BOOL InitDtorAddr()
{
	APPLOG_FUNCTION_ENTRY;    //宏APPLOG_FUNCTION_ENTRY和APPLOG_FUNCTION_ENTRY2分别在栈上定义一个本地C++对象
	return TRUE;
}

static BOOL _Init = InitDtorAddr();
static PBYTE g_pDtorAddr = NULL;
#endif


// disable global optimization and setup frame pointer to get return address
#pragma optimize( "g" , off )
#pragma optimize( "y" , on )
#pragma optimize("", off)
AutoScopeTrace::~AutoScopeTrace()
{
	PBYTE callAddr, entryAddr;
	__asm {
		mov eax, [ebp+4];
		sub eax, 5;
		mov callAddr, eax;
#if _MSC_VER >= 1300
		mov eax, AutoScopeTrace::~AutoScopeTrace;
		mov entryAddr, eax;
#endif
	}

#if _MSC_VER < 1300 //vc6
	if (g_pDtorAddr)
		entryAddr = g_pDtorAddr;
	else
	{
		if( callAddr[0] == INST_RELATIVE_CALL )
		{
			int callOffset = *(int*)(callAddr+1);
			g_pDtorAddr = callAddr+callOffset+INST_RELATIVE_CALL_LENGTH;
		}
		return;
	}
#endif
	

	// 判断正常析构还是异常析构
	if( IsNormalCall(callAddr, entryAddr) )
	{
		if( resobj_ != NULL )
		{
			if( resobj_->result_ok() )
			{
				FormatAndOutputString("Leave ok", ST_LEAVE_OK);
			}
			else
			{
				std::string str = "Leave failed due to return value: ";
				str += resobj_->get_type_desc();
				str += '=';
				std::string desc;
				resobj_->get_result_desc(desc);
				str += desc;
				FormatAndOutputString(str.c_str(), ST_LEAVE_FAILED);
			}
		}
		else
			FormatAndOutputString("Leave", ST_LEAVE_OK);
	}
	else
	{
		if( resobj_ == NULL )
			FormatAndOutputString("Leave failed due to exception", ST_LEAVE_FAILED);
		else
		{
			std::string str = "Leave failed due to exception, return value: ";
			str += resobj_->get_type_desc();
			str += '=';
			std::string desc;
			resobj_->get_result_desc(desc);
			str += desc;
			FormatAndOutputString(str.c_str(), ST_LEAVE_FAILED);
		}
	}
}
// use project default optimization settings
#pragma optimize("", on)


void AutoScopeTrace::FormatAndOutputString(const char* event_text, AutoScopeTrace::ST_FLAG flag )
{
#if _MSC_VER < 1300
	if (!g_pDtorAddr)
		return;
#endif
	
	TraceoutNM::TR_FLAG trflag;
	int deltaDepth;
	switch(flag)
	{
	case ST_ENTER:
		trflag = TraceoutNM::TR_QUEUED;
		deltaDepth = 1;
		break;
	case ST_LEAVE_OK:
		trflag = TraceoutNM::TR_DISCARD;
		deltaDepth = -1;
		break;
	case ST_LEAVE_FAILED:
		trflag = TraceoutNM::TR_OUTPUT;
		deltaDepth = -1;
		break;
	default:
		return;
	}

	const int BUFFER_SIZE = 1024;
	char szMessage[BUFFER_SIZE];
#if _MSC_VER < 1300
	int len = _snprintf( szMessage, sizeof(szMessage), "%s(%d): %s %s\n", trinfo_.file_, trinfo_.line_, event_text, trinfo_.text_);
#else
	int len = _snprintf( szMessage, sizeof(szMessage), "%s(%d): %s() %s %s\n", trinfo_.file_, trinfo_.line_, trinfo_.function_, event_text, trinfo_.text_);
#endif
	if( len < sizeof(szMessage) )
	{
		TraceoutMan::OutputString(szMessage, trflag, deltaDepth);
	}
	else
	{
		std::strstream os;
		os << trinfo_.file_ << '(' << trinfo_.line_ << "): " << trinfo_.function_ << "() " << event_text << ' ' << trinfo_.text_ << '\n' << std::ends;
		const char *ptr = os.str();
		os.freeze( false );

		TraceoutMan::OutputString(ptr);
	}
}


void trstream::OutputString( const char* text, bool queued )
{
	TraceoutNM::TR_FLAG trflag = queued ? TraceoutNM::TR_QUEUED : TraceoutNM::TR_OUTPUT;
	TraceoutMan::OutputString(text, trflag, 0);
}

void Win32ErrorResultPolicy::get_result_desc(const DWORD& result, std::string& desc)
{
	LPSTR lpMsgBuf;
	if (FormatMessageA( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		result,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPSTR)&lpMsgBuf,
		0,
		NULL ))
	{
		desc = lpMsgBuf;
		LocalFree( lpMsgBuf );
	}
}

int trstream::priorityline_ = PL_DEBUG;