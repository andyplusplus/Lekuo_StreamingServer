#pragma once
#include <algorithm>
#include <ostream>
#include <strstream>
#include <cassert>

using std::ostream;

#ifndef TRACE_ASSERT
//#	include <boost/static_assert.hpp> // if you have no boost library installed, comment this line out
#	ifdef BOOST_STATIC_ASSERT
#		define TRACE_ASSERT BOOST_STATIC_ASSERT
#	else
#		define TRACE_ASSERT assert
#	endif // BOOST_STATIC_ASSERT
#endif // TRACE_STATIC_ASSERT

template<typename T>
struct ResultPolicyBase
{
	typedef T result_type;
	static bool succeeded( const result_type& result, const result_type& ref_value )
	{
		return result == ref_value;
	}
};

template<typename T> struct ResultPolicy {};

template<>
struct ResultPolicy<int> : public ResultPolicyBase<int>
{
	static const char* get_type_desc()
	{
		return "int";
	}
	static void get_result_desc(const result_type& result, std::string& desc)
	{
		char tmp[20];
		desc = _itoa(result, tmp, 10);
	}
};

template<>
struct ResultPolicy<bool> : public ResultPolicyBase<bool>
{
	static const char* get_type_desc()
	{
		return "bool";
	}
	static void get_result_desc(const result_type& result, std::string& desc)
	{
		desc = result ? "true" : "false";
	}
};

struct Win32BOOLResultPolicy : public ResultPolicyBase<BOOL>
{
	static const char* get_type_desc()
	{
		return "BOOL";
	}
	static void get_result_desc(const result_type& result, std::string& desc)
	{
		desc = result ? "TRUE" : "FALSE";
	}
};

struct Win32ErrorResultPolicy : public ResultPolicyBase<DWORD>
{
	static const char* get_type_desc()
	{
		return "WIN32ERR";
	}
	static void get_result_desc(const result_type& result, std::string& desc);
};

struct HRESULTResultPolicy : public ResultPolicyBase<HRESULT>
{
	static const char* get_type_desc()
	{
		return "HRESULT";
	}
	static bool succeeded( const result_type& result, const result_type&)
	{
		return SUCCEEDED(result);

	}
	static void get_result_desc(const result_type& result, std::string& desc)
	{
		Win32ErrorResultPolicy::get_result_desc(result, desc);
	}
};

class ResultObjectBase
{
public:
	virtual bool result_ok()						const = 0;
	virtual const char* get_type_desc()				const = 0;
	virtual void get_result_desc(std::string& desc)	const = 0;
};

template<typename Policy>
class ResultObject : public ResultObjectBase
{
	typedef Policy							policy_type;
	typedef typename Policy::result_type	result_type;
	const result_type& result_;
	const result_type& res_ok_;
public:
	explicit ResultObject( const result_type& result, const result_type& res_ok = 0):result_(result),res_ok_(res_ok){}
	virtual bool result_ok() const
	{
		return policy_type::succeeded(result_, res_ok_);
	}
	virtual const char* get_type_desc() const
	{
		return policy_type::get_type_desc();
	}
	virtual void get_result_desc(std::string& desc) const
	{
		policy_type::get_result_desc(result_, desc);
	}
};

struct BasicTraceInfo {
	int line_;
	const char* file_;
	const char* function_;
	const char* text_;
	BasicTraceInfo(int line, const char* file, const char* function, const char* text)
		:line_(line),file_(file),function_(function),text_(text){}
};

// 注意:该类不允许继承
class AutoScopeTrace
{
	BasicTraceInfo trinfo_;
	ResultObjectBase* resobj_;
	char resobj_buf_[sizeof(ResultObject<ResultPolicy<int> >)];
	typedef enum{ ST_ENTER, ST_LEAVE_OK, ST_LEAVE_FAILED} ST_FLAG;
public:
	explicit AutoScopeTrace(int line, const char* file, const char* function, const char* text)
	:trinfo_(line,file,function,text)
	,resobj_(NULL)
	{
		assert(text);
		FormatAndOutputString("Enter", ST_ENTER);
	}
	template<typename T>
	explicit AutoScopeTrace(int line, const char* file, const char* function, const char* text, const T& result, const T& res_ok = 0)
	:trinfo_(line,file,function,text)
	{
		assert(text);
		typedef ResultObject<ResultPolicy<T> > resobj_type;
		TRACE_ASSERT( sizeof(resobj_buf_) == sizeof(resobj_type) );

		resobj_type* p = reinterpret_cast<resobj_type*>(resobj_buf_);
		std::allocator<resobj_type> alloc;
		alloc.construct( p, resobj_type(result,res_ok) );
		resobj_ = p;

		FormatAndOutputString("Enter", ST_ENTER);
	}
	template<typename Policy>
	AutoScopeTrace(int line, const char* file, const char* function, const char* text, Policy*, const typename Policy::result_type& result, const typename Policy::result_type& res_ok)
	:trinfo_(line,file,function,text)
	{
		assert(text);
		typedef ResultObject<Policy> resobj_type;
		TRACE_ASSERT( sizeof(resobj_buf_) == sizeof(resobj_type) );

		resobj_type* p = reinterpret_cast<resobj_type*>(resobj_buf_);
		std::allocator<resobj_type> alloc;
		alloc.construct( p, resobj_type(result,res_ok) );
		resobj_ = p;

		FormatAndOutputString("Enter", ST_ENTER);
	}
	~AutoScopeTrace();
private:
	void FormatAndOutputString(const char* event_text, ST_FLAG flag );

	// declared but not implemented
	// allocated on stack only
	static void* operator new(size_t);
	static void operator delete(void*);
};

#if _MSC_VER < 1300 //vc6
#ifndef __FUNCTION__
#define __FUNCTION__ ""
#endif // __FUNCTION__
#endif

//#ifdef _DEBUG
#	define __APPLOG_USE_SCOPE_ENTRY__
//#endif // _DEBUG

#ifdef __APPLOG_USE_SCOPE_ENTRY__
#define APPLOG_FUNCTION_ENTRY									\
		AutoScopeTrace											\
		AutoScopeTrace_0(__LINE__, __FILE__, __FUNCTION__,"")

#define APPLOG_FUNCTION_ENTRY1(retval)							\
		AutoScopeTrace											\
		AutoScopeTrace1_##retval(__LINE__, __FILE__, __FUNCTION__,"",	\
						retval)

#define APPLOG_FUNCTION_ENTRY2(retval,res_ok)					\
		AutoScopeTrace											\
		AutoScopeTrace2_##retval(__LINE__, __FILE__, __FUNCTION__,"",	\
						retval, res_ok)

#define APPLOG_FUNCTION_ENTRY3(retval,res_ok,policy_type)		\
		AutoScopeTrace											\
		AutoScopeTrace3_##retval(__LINE__, __FILE__, __FUNCTION__,"",	\
						((policy_type*)NULL), retval, res_ok)

#define APPLOG_SCOPE_ENTRY(varid, text)							\
		AutoScopeTrace											\
		AutoScopeTrace_##varid(__LINE__, __FILE__, __FUNCTION__, text)

#define APPLOG_FUNCTION_ENTRY_HRESULT(retval)					\
	APPLOG_FUNCTION_ENTRY3(retval, S_OK, HRESULTResultPolicy)

#define APPLOG_FUNCTION_ENTRY_WIN32(retval)						\
	APPLOG_FUNCTION_ENTRY3(retval, 0, Win32ErrorResultPolicy)

#define FUNCTION_TRACE_DECLARE_HRESULT(retval) HRESULT retval = S_OK; APPLOG_FUNCTION_ENTRY_HRESULT(retval);
#define FUNCTION_TRACE_DECLARE_HRESULT1(retval,initval) HRESULT retval = initval; APPLOG_FUNCTION_ENTRY_HRESULT(retval)

#else

#	define APPLOG_FUNCTION_ENTRY					do{}while(0)
#	define APPLOG_FUNCTION_ENTRY1(retval)			do{}while(0)
#	define APPLOG_FUNCTION_ENTRY2(retval,res_ok)	do{}while(0)
#	define APPLOG_FUNCTION_ENTRY3(retval,res_ok,policy_type) do{}while(0)
#	define APPLOG_SCOPE_ENTRY(varid, text)			do{}while(0)
#	define APPLOG_FUNCTION_ENTRY_HRESULT(retval)	do{}while(0)
#	define APPLOG_FUNCTION_ENTRY_WIN32(retval)		do{}while(0)
#	define FUNCTION_TRACE_DECLARE_HRESULT(retval) HRESULT retval = S_OK
#	define FUNCTION_TRACE_DECLARE_HRESULT1(retval,initval) HRESULT retval = initval
#endif // __APPLOG_USE_SCOPE_ENTRY__


//typedef enum {
//			PL_EMERG = 0, 
//			PL_FATAL = 0,
//			PL_ALERT = 100,
//			PL_CRITC = 200,
//			PL_ERROR = 300,
//			PL_WARNG = 400,
//			PL_NOTIC = 500,
//			PL_INFOO = 600,
//			PL_DEBUG = 700,
//			PL_NOTST = 800
//} PriorityLevel;
#include "LogPriorityLevel.h"

//
//该类表示日志的类
//
extern PriorityLevel gSTPriorityLevel;

class trstream : public std::ostream
{
	// declared but not implemented
	// non-copyable
	trstream( const trstream& );
	const trstream& operator=( const trstream& );
	// allocated on stack only
	static void* operator new(size_t);
	//	static void operator delete(void*);

public:

	//构造Streaming对象
	explicit trstream( int level, bool queued );
	~trstream();

	//返回对象指针
	trstream& l_value() { return *this; }

	// strstreambuf标准接口
	std::strstreambuf *rdbuf() const
		{return ( (std::strstreambuf *)&streambuf_ ); }

	void freeze(bool f = true)
		{streambuf_.freeze(f); }

	char *str()
	{return (streambuf_.str()); }

	std::streamsize pcount() const
	{return (streambuf_.pcount()); }

protected:
	static void OutputString( const char* text, bool queued = true );

private:
	std::strstreambuf streambuf_;
	int prioritylevel_;
	bool queued_;

public:
	static int priorityline_;

};

//构造ostream对象，并设定优先级
inline
trstream::trstream( int level, bool queued )
		: ostream(&streambuf_)
		, prioritylevel_(level)
		, queued_(queued)
{
}

inline
trstream::~trstream()
{
	*this << std::endl << std::ends;
	if( prioritylevel_ <= priorityline_ )
		OutputString( this->str(), queued_ );
	this->freeze( false );
}

#ifdef __APPLOG_USE_SCOPE_ENTRY__

//构造ostream对象，把数据写出Ostreaming流
//#define TROUT(message, level, queued)	do{ trstream( level, queued  ).l_value()  << #level << " : " << GetTickCount() << " : " << message; }while(0)
#define TROUT(message, level, queued)	do{ trstream( level, queued  ).l_value()  << GetTickCount() << " : " << message; }while(0)
#define SETTRLEVEL(level) do{ trstream::priorityline_ = level; }while(0)

#else

#define TROUT(message) do{}while(0)

#endif // __APPLOG_USE_SCOPE_ENTRY__
